#include "Game.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <memory>
#include "imgui-SFML.h"
#include "Components.h"
#include "Vector2.h"

#define BULLET "Bullet"
#define ENEMY "Enemy"
#define SMALL_ENEMY "Small Enemy"
#define PLAYER "Player"
#define PI 3.14159265359

Game::Game(const std::string &config, const std::string &font)
{
    init(config, font);
}

void Game::init(const std::string &config, const std::string &font)
{
    srand(time(NULL));
    std::ifstream fin(config);
    if (!fin)
    {
        std::cerr << "Error opening file\n";
        exit(-1);
    }

    std::string configType;
    while (fin >> configType)
    {
        if (configType == "Window")
        {
            int wWidth, wHeight, frames, isWindowed;
            fin >> wWidth >> wHeight >> frames >> isWindowed;

            auto style = isWindowed ? sf::Style::Default : sf::Style::Fullscreen;

            m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment2", style);
            m_window.setFramerateLimit(frames);

        }
        if (configType == "Font")
        {
            std::string fontPath;
            fin >> fontPath;
            if (!m_font.loadFromFile(font + fontPath))
            {
                std::cerr << "Error loading font\n";
                exit(-1);
            }
            int textSize, r, g, b;
            fin >> textSize >> r >> g >> b;
            auto textColor = sf::Color(r, g, b);
            m_text = sf::Text("SCORE: ", m_font, textSize);
            m_text.setPosition(10, 10);
            m_text.setFillColor(textColor);
            m_textSpecialWeapon = sf::Text("", m_font, textSize);
            m_textSpecialWeapon.setPosition(10, 40);
            m_textSpecialWeapon.setFillColor(textColor);

        }
        if (configType == PLAYER)
        {
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
        }
        if (configType == ENEMY)
        {
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
            m_enemySpawnerTime = m_enemyConfig.SI;
        }
        if (configType == BULLET) {
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }

    ImGui::SFML::Init(m_window);

    createBackgroundImage();
    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        m_entities.update();

        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        sEnemySpawner();
        sLifespan();
        sCollision();
        sMovement();
        sUserInput();
        sGUI();
        sRender();

        if (!m_paused)
            m_currentFrame++;
    }
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::spawnPlayer()
{
    // We create every entity by calling EntityManager.addEntity(tag) —
    // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing _
    auto entity = m_entities.addEntity(PLAYER);

    entity->cTransform = std::make_shared<CTransform>(Vector2(m_window.getSize().x/2, m_window.getSize().y/2), Vector2(m_playerConfig.S, m_playerConfig.S), 0.0f);

    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    // The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness u
    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
    // Add an input component to the player so that we can use inputs

    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    auto vertices = e->cShape->circle.getPointCount();
    auto angle = 2 * PI / vertices;

    for (int i = 0; i < vertices; i++)
    {
        auto smallEnemy = m_entities.addEntity(SMALL_ENEMY);
        auto position = e->cTransform->position;
        auto velocity = e->cTransform->velocity;
        auto enemyVelocity = Vector2(std::cos(angle * i) * velocity.x, std::sin(angle * i) * velocity.y);
        smallEnemy->cTransform = std::make_shared<CTransform>(position, enemyVelocity, 0.0f);
        smallEnemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR / 2);
        smallEnemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR / 2, vertices, sf::Color(e->cShape->circle.getFillColor()), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
        smallEnemy->cScore = std::make_shared<CScore>(vertices * 2);
        smallEnemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vector2 &mousePosition)
{
    auto d = mousePosition - entity->cTransform->position;
    d.normalize();
    auto bulletVelocity = d * m_bulletConfig.S;
    auto bullet = m_entities.addEntity(BULLET);

    bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->position.clone(), bulletVelocity, 0.0f);

    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

    bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vector2 &mousePosition)
{
    if (m_currentFrame - m_lastSpecialWeaponTime < m_specialWeaponCooldown && isSpecialWeaponCooldownActive) { return; }
    m_lastSpecialWeaponTime = m_currentFrame;

    auto dir = mousePosition - entity->cTransform->position;
    dir.normalize();
    auto flameVelocity = dir * 10;
    int flameHeight = 100;
    int radius = 10;
    auto sqrt3 = std::sqrt(3);

    auto upColor = sf::Color(255, 0, 0);
    auto middleColor = sf::Color(255, 165, 0);
    auto downColor = sf::Color(255, 255, 0);



    for (int i = flameHeight; i >= 0; i -= radius * 2)
    {
        auto flameBaseMiddlePoint = (dir * i) + entity->cTransform->position;
        auto perpendicular = (flameBaseMiddlePoint - entity->cTransform->position).perpendicular().normalize();

        for (int j = 0; j < sqrt3 * i / 3; j += radius * 2)
        {
            for (int k = -1; k < 2; k += 2)
            {
                auto flame = m_entities.addEntity(BULLET);
                auto position = Vector2(flameBaseMiddlePoint.x, flameBaseMiddlePoint.y) + perpendicular * j * k;
                flame->cTransform = std::make_shared<CTransform>(position, flameVelocity, 0.0f);
                flame->cCollision = std::make_shared<CCollision>(radius);
                if (i >= flameHeight * 2 / 3)
                {
                    flame->cShape = std::make_shared<CShape>(radius, 3, upColor, sf::Color::Black, 1);
                }
                else if (i >= flameHeight / 3)
                {
                    flame->cShape = std::make_shared<CShape>(radius, 3, middleColor, sf::Color::Black, 1);
                }
                else
                {
                    flame->cShape = std::make_shared<CShape>(radius, 3, downColor, sf::Color::Black, 1);
                }
                flame->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L * 3);
            }
    
        }
    }

}

void Game::sMovement()
{
    if (m_paused || !isMovementActive) { return; }
    // Player movement
    auto sqrt2 = std::sqrt(2);
    if (m_player->cInput->left && m_player->cInput->up)
    {
        m_player->cTransform->position.y -= m_player->cTransform->velocity.y / sqrt2;
        m_player->cTransform->position.x -= m_player->cTransform->velocity.x / sqrt2;
    }
    else if (m_player->cInput->left && m_player->cInput->down)
    {
        m_player->cTransform->position.y += m_player->cTransform->velocity.y / sqrt2;
        m_player->cTransform->position.x -= m_player->cTransform->velocity.x / sqrt2;
    }
    else if (m_player->cInput->right && m_player->cInput->up)
    {
        m_player->cTransform->position.y -= m_player->cTransform->velocity.y / sqrt2;
        m_player->cTransform->position.x += m_player->cTransform->velocity.x / sqrt2;
    }
    else if (m_player->cInput->right && m_player->cInput->down)
    {
        m_player->cTransform->position.y += m_player->cTransform->velocity.y / sqrt2;
        m_player->cTransform->position.x += m_player->cTransform->velocity.x / sqrt2;
    }
    else if (m_player->cInput->left)
    {
        m_player->cTransform->position.x -= m_player->cTransform->velocity.x;
    }
    else if (m_player->cInput->right)
    {
        m_player->cTransform->position.x += m_player->cTransform->velocity.x;
    }
    else if (m_player->cInput->down)
    {
        m_player->cTransform->position.y += m_player->cTransform->velocity.y;
    }
    else if (m_player->cInput->up)
    {
        m_player->cTransform->position.y -= m_player->cTransform->velocity.y;
    }

    for (auto& e: m_entities.getEntities())
    {
        if (e == m_player) { continue; }

        e->cTransform->position += e->cTransform->velocity;
    }

}

void Game::sLifespan()
{
    if (!isLifespanActive) { return; }
    for (auto &e : m_entities.getEntities())
    {
        if (!e->cLifespan) continue;

        e->cLifespan->remaining--;
        if (e->cLifespan->remaining <= 0)
        {
            e->destroy();
        }
        else
        {
            auto alpha = 255 * e->cLifespan->remaining / e->cLifespan->total;
            auto fillColor = e->cShape->circle.getFillColor();
            auto outlineColor = e->cShape->circle.getOutlineColor();
            e->cShape->circle.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b, alpha ));
            e->cShape->circle.setOutlineColor(sf::Color(outlineColor.r, outlineColor.g, outlineColor.b, alpha));
        }

    }
}

void Game::sCollision()
{
    for (auto& e: m_entities.getEntities())
    {
        // Check global bound and bounce if entity is not player
        const auto left = e->cTransform->position.x - e->cCollision->radius;
        const auto right = e->cTransform->position.x + e->cCollision->radius;
        const auto bot = e->cTransform->position.y + e->cCollision->radius;
        const auto top = e->cTransform->position.y - e->cCollision->radius;
        
        auto windowSize = m_window.getSize();
        
        if (e != m_player)
        {
            if (left <= 0 || right >= windowSize.x)
            {
                e->cTransform->velocity.x *= -1;
            }
            if (top <= 0 || bot >= windowSize.y)
            {
                e->cTransform->velocity.y *= -1;
            }
        }
        // Check if player is out of bounds and stop movement
        else
        {
            if (left <= 0)
            {
                e->cInput->left = false;
            }
            if (right >= windowSize.x)
            {
                e->cInput->right = false;
            }
            if (top <= 0)
            {
                e->cInput->up = false;
            }
            if (bot >= windowSize.y)
            {
                e->cInput->down = false;
            }
        }

        if (isCollisionActive && (e->tag() == ENEMY || e->tag() == SMALL_ENEMY) && e->isActive())
        {
            auto distanceToPlayer = m_player->cTransform->position - e->cTransform->position;
            if (distanceToPlayer.x * distanceToPlayer.x + distanceToPlayer.y * distanceToPlayer.y <= (m_player->cCollision->radius + e->cCollision->radius) * (m_player->cCollision->radius + e->cCollision->radius))
            {
                e->destroy();
                m_player->destroy();
                m_score = 0;
                spawnPlayer();
                if (e->tag() == ENEMY)
                    spawnSmallEnemies(e);
                continue;
            }

            for (auto &bullet: m_entities.getEntities(BULLET))
            {
                if (!e->isActive()) { break; }
                if (!bullet->isActive()) { continue; }
                auto distanceToBullet = bullet->cTransform->position - e->cTransform->position;
                if (distanceToBullet.x * distanceToBullet.x + distanceToBullet.y * distanceToBullet.y <= (bullet->cCollision->radius + e->cCollision->radius) * (bullet->cCollision->radius + e->cCollision->radius))
                {
                    m_score += e->cScore->score * 100;
                    e->destroy();
                    bullet->destroy();
                    if (e->tag() == ENEMY)
                        spawnSmallEnemies(e);
                }
            }
        }
    }
}

void Game::sEnemySpawner()
{
    if (m_paused || !isEnemySpawnerActive || m_currentFrame - m_lastEnemySpawnTime < m_enemySpawnerTime) { return; }

    m_lastEnemySpawnTime = m_currentFrame;
    int speedDiff = 1 + m_enemyConfig.SMAX - m_enemyConfig.SMIN;
    auto speed = rand() % speedDiff + m_enemyConfig.SMIN;
    auto vertices = rand() % (1 + m_enemyConfig.VMAX - m_enemyConfig.VMIN) + m_enemyConfig.VMIN;
    auto position = Vector2(0, 0);
    do
    {
        position.x = m_enemyConfig.SR + rand() % (m_window.getSize().x - m_enemyConfig.SR * 2);
        position.y = m_enemyConfig.SR + rand() % (m_window.getSize().y - m_enemyConfig.SR * 2);
    }
    while (((position - m_player->cTransform->position).length() < m_player->cCollision->radius * 2));
    auto enemyVelocity = Vector2(speed, speed);

    auto enemy = m_entities.addEntity(ENEMY);

    enemy->cTransform = std::make_shared<CTransform>(position, enemyVelocity, 0.0f);

    enemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

    enemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, sf::Color(rand() % 255, rand() % 255, rand() % 255), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

    enemy->cScore = std::make_shared<CScore>(vertices);

}

void Game::sGUI()
{
    ImGui::Begin("Geometry Wars");
    if (ImGui::BeginTabBar("Geometry Wars"))
    {
        if (ImGui::BeginTabItem("Systems"))
        {
            ImGui::Checkbox("EnemySpawner", &isEnemySpawnerActive);
            ImGui::Checkbox("Lifespan", &isLifespanActive);
            ImGui::Checkbox("Collision", &isCollisionActive);
            ImGui::Checkbox("Movement", &isMovementActive);
            ImGui::Checkbox("UserInput", &isUserInputActive);
            ImGui::Checkbox("Render", &isRenderActive);
            ImGui::Checkbox("Special Weapon Cooldown", &isSpecialWeaponCooldownActive);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Entities"))
        {
            if (ImGui::CollapsingHeader("Entites by tag"))
            {
                ImGui::Indent(10);
                if (ImGui::CollapsingHeader(PLAYER))
                {
                    ImGui::Indent(10);
                    if (ImGui::Button("D"))
                    {
                        m_player->destroy();
                        spawnPlayer();
                    }
                    ImGui::SameLine();
                    ImGui::Text("%zu", m_player->id());
                    ImGui::SameLine();
                    ImGui::Text(PLAYER);
                    ImGui::SameLine();
                    ImGui::Text("Position: (%.2f, %.2f)", m_player->cTransform->position.x, m_player->cTransform->position.y);
                }
                if (ImGui::CollapsingHeader(ENEMY))
                {
                    ImGui::Indent(10);
                    for (auto &e: m_entities.getEntities(ENEMY))
                    {
                        if (ImGui::Button("D"))
                        {
                            e->destroy();
                            spawnSmallEnemies(e);
                        }
                        ImGui::SameLine();
                        ImGui::Text("%zu", e->id());
                        ImGui::SameLine();
                        ImGui::Text(ENEMY);
                        ImGui::SameLine();
                        ImGui::Text("Position: (%.2f, %.2f)", e->cTransform->position.x, e->cTransform->position.y);
                    }
                }
                if (ImGui::CollapsingHeader(BULLET))
                {
                    ImGui::Indent(10);
                    for (auto &e: m_entities.getEntities(BULLET))
                    {
                        if (ImGui::Button("D"))
                        {
                            e->destroy();
                            spawnSmallEnemies(e);
                        }
                        ImGui::SameLine();
                        ImGui::Text("%zu", e->id());
                        ImGui::SameLine();
                        ImGui::Text(ENEMY);
                        ImGui::SameLine();
                        ImGui::Text("Position: (%.2f, %.2f)", e->cTransform->position.x, e->cTransform->position.y);
                    }
                }
                if (ImGui::CollapsingHeader(SMALL_ENEMY))
                {
                    ImGui::Indent(10);
                    for (auto &e: m_entities.getEntities(SMALL_ENEMY))
                    {
                        if (ImGui::Button("D"))
                        {
                            e->destroy();
                            spawnSmallEnemies(e);
                        }
                        ImGui::SameLine();
                        ImGui::Text("%zu", e->id());
                        ImGui::SameLine();
                        ImGui::Text(ENEMY);
                        ImGui::SameLine();
                        ImGui::Text("Position: (%.2f, %.2f)", e->cTransform->position.x, e->cTransform->position.y);
                    }
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Game::sRender()
{
    if (!isRenderActive) { return; }
    m_window.clear();
    m_text.setString("SCORE: " + std::to_string(m_score));
    auto cooldown = std::ceil(float(m_specialWeaponCooldown - (m_currentFrame - m_lastSpecialWeaponTime))/60);
    cooldown = cooldown < 0 ? 0 : cooldown;
    m_textSpecialWeapon.setString("Cooldown: " + std::to_string(int(cooldown)) + " seconds");
    m_window.draw(m_text);
    m_window.draw(m_textSpecialWeapon);

    for (auto shape: m_backgroundShapes)
    {
        m_window.draw(*shape);
    }

    for (auto& e: m_entities.getEntities())
    {
        e->cShape->circle.setPosition(e->cTransform->position.x, e->cTransform->position.y);
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);
        m_window.draw(e->cShape->circle);
    }



    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput()
{
    sf::Event event;
    if (!isUserInputActive) { return; }
    while (m_window.pollEvent(event))
    {
        // pass the event to imgui to be parsed
        ImGui::SFML::ProcessEvent(m_window, event);
        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::Up:
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::Down:
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = true;
                    break;
                case sf::Keyboard::Right:
                    m_player->cInput->right = true;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::Left:
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::P:
                    m_paused = !m_paused;
                    break;
                case sf::Keyboard::Escape:
                    m_running = false;
                    break;
                default:
                    ;
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::Up:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::Down:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = false;
                    break;
                case sf::Keyboard::Right:
                    m_player->cInput->right = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::Left:
                    m_player->cInput->left = false;
                    break;
                default:
                    ;
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            // this line ignores mouse events if ImGui is the thing being clicked
            if (ImGui::GetIO().WantCaptureMouse || m_paused) { continue; }

            if (event.mouseButton.button == sf::Mouse::Left)
            {
                spawnBullet(m_player, Vector2(event.mouseButton.x, event.mouseButton.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                spawnSpecialWeapon(m_player, Vector2(event.mouseButton.x, event.mouseButton.y));
            }
        }
    }
}

void Game::createBackgroundImage()
{
    auto windowSize = m_window.getSize();

    int radius = 100;
    auto x = windowSize.x / radius;
    auto y = windowSize.y / radius;

    for (int i = 0; i <= x; i++)
    {
        for (int j = 0; j <= y; j++)
        {
            auto rgb = 255 - (255 / (x+y) * (i+j + 1));
            auto circle = new sf::CircleShape{ (float)radius };
            circle->setFillColor(sf::Color(rgb, rgb, rgb, 50));
            circle->setPosition(i * radius, j * radius);
            m_backgroundShapes.push_back(circle);
        }
    }
}
