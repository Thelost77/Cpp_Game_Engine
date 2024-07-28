#include "Game.h"
#include <iostream>
#include <fstream>
#include "imgui-SFML.h"
#include "Components.h"
#include "Vector2.h"

Game::Game(const std::string &config, const std::string &font)
{
    init(config, font);
}

void Game::init(const std::string &config, const std::string &font)
{
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
            m_text = sf::Text("", m_font, textSize);
            m_text.setFillColor(textColor);

        }
        if (configType == "Player")
        {
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V >> m_playerConfig.S >> m_playerConfig.L;
        }
        if (configType == "Enemy")
        {
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.SI;
        }
        if (configType == "Bullet") {
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L >> m_bulletConfig.S;
        }
    }

    ImGui::SFML::Init(m_window);

    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        m_entities.update();

        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        sEnemySpawner();
        sCollision();
        sMovement();
        sUserInput();
        sGUI();
        sRender();

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
    auto entity = m_entities.addEntity("player");

    // Give this entity a Transform so it spawns at (200,200) with velocity (1,1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vector2(200.0f, 200.0f), Vector2(10.0f, 10.0f), 0.0f);

    // The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness u
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color::Blue, sf::Color(255, 0, 0), 0.5f);
    // Add an input component to the player so that we can use inputs

    entity->cInput = std::make_shared<CInput>();

    // Since we want this Entity to be our player, set our Game's player variable to be this Entity
    // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
    m_player = entity;
}

void Game::spawnEnemy()
{
    // TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
    // the enemy must be spawned completely within the bounds of the window
    //
    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{

    // TODO: spawn small enemies at the location of the input enemy e
    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the original enemy
    // ~ set each small enemy to the same color as the original, half the size
    // - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vector2 &target)
{
    // TODO: implement the spawning of a bullet which travels toward target
    // ~ bullet speed is given as a scalar speed

    // _ you must set the velocity by using formula in notes I
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // TODO: implement your own special weapon
}

void Game::sMovement()
{
    // TODO: implement all entity movement in this function

    // ydp should read the m_player->cInput component to determine if the player is moving

    // Sample movement speed update
    m_player->cTransform->position.x += m_player->cTransform->velocity.x;
    m_player->cTransform->position.y += m_player->cTransform->velocity.y;
}

void Game::sLifespan()
{

    // TODO: implement all lifespan functionality
    //
    // for all entities
    // if entity has no lifespan component, skip it
    // if entity has > 0 remaining lifespan, subtract 1
    // if it has lifespan and is alive
    // scale its alpha channel properly

    // if it has lifespan and its time is up
    // destroy the entity I
}

void Game::sCollision()
{
    for (auto& e: m_entities.getEntities()) 
    {
        const sf::FloatRect& globalBounds = e->cShape->circle.getGlobalBounds();
        auto windowSize = m_window.getSize();
        if (globalBounds.left <= 0 || globalBounds.left + globalBounds.width >= windowSize.x)
        {
            e->cTransform->velocity.x *= -1;
        }
        if (globalBounds.top <= 0 || globalBounds.top + globalBounds.height >= windowSize.y)
        {
            e->cTransform->velocity.y *= -1;
        }
        // for (auto& e2: m_entities.getEntities()) 
        // {
        //     if (e == e2) { continue; }

        // }
    }
    // TODO: implement all proper collisions between entities
    // be sure to use the collision radius, NOT the shape radius
}

void Game::sEnemySpawner()
{
    // TODO: code which implements enemy spawning should go here
}

void Game::sGUI()
{
    ImGui::Begin("Geometry Wars");
    ImGui::Text("Stuff Goes Here");
    ImGui::End();
}

void Game::sRender()
{

    // TODO: change the code below to draw ALL of the entities
    // sample drawing of the player Entity that we have created
    m_window.clear();

    // set the position of the shape based on the entity's transform->pos

    m_player->cShape->circle.setPosition(m_player->cTransform->position.x, m_player->cTransform->position.y);

    // set the rotation of the shape based on the entity's transform->angle
    m_player->cTransform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);

    // draw the entity's sf::CircleShape
    m_window.draw(m_player->cShape->circle);

    // draw the ui last
    ImGui::SFML::Render(m_window);

    m_window.display();
}

void Game::sUserInput()
{
    // TODO: handle user input here
    // note that you should only be setting the players's input component variables here
    // you should not implement the player's movement logic here
    // the movement system will read the variables you set in this function
    sf::Event event;
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
                    std::cout << "W Key Pressed\n";
                    // TODO: set player's input component "up" to true
                    break;
                default:
                    std::cout << "Key Pressed\n";
                    break;
            }
        }
        
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    std::cout << "W Key Released\n";
                    // TODO: set player's input component "up" to false
                    break;
                default:
                    ;
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            // this line ignores mouse events if ImGui is the thing being clicked
            if (ImGui::GetIO().WantCaptureMouse) { continue; }

            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // call spawnBullet here
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                // call spawnSpecialWepon here
            }
        }
    }
}
