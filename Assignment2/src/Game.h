#include "Entity.h"
#include "EntityManager.h"
#include "Vector2.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#ifndef GAME_H
#define GAME_H

struct PlayerConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S;
};
struct EnemyConfig
{
    int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};
struct BulletConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};

class Game
{
    sf::RenderWindow        m_window; // the window we will draw to
    EntityManager           m_entities;  // vector of entities to maintain
    sf::Font                m_font;           // the font we will use to draw
    sf::Text                m_text;           // the score text to be drawn to the screen
    sf::Text                m_textSpecialWeapon;           // the score text to be drawn to the screen
    PlayerConfig            m_playerConfig;
    EnemyConfig             m_enemyConfig;
    BulletConfig            m_bulletConfig;
    sf::Clock               m_deltaClock;
    std::vector<sf::Shape*> m_backgroundShapes;
    int                     m_score = 0;
    int                     m_currentFrame = 0;
    int                     m_lastEnemySpawnTime = 0;
    int                     m_enemySpawnerTime = 60;
    int                     m_specialWeaponCooldown = 60 * 3;
    int                     m_lastSpecialWeaponTime = -100000;
    bool                    m_paused = false; // whether we update game logic
    bool                    m_running = true; // whether the game is running
    std::shared_ptr<Entity> m_player;

    bool                    isEnemySpawnerActive = true;
    bool                    isLifespanActive = true;
    bool                    isCollisionActive = true;
    bool                    isMovementActive = true;
    bool                    isUserInputActive = true;
    bool                    isRenderActive = true;
    bool                    isSpecialWeaponCooldownActive = true;

    public:
    Game(const std::string &config, const std::string &font);
    void init(const std::string &config, const std::string &font);
    void run();
    void setPaused(bool paused);
    void spawnPlayer();
    void spawnSmallEnemies(std::shared_ptr<Entity> e);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vector2 &mousePosition);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity, const Vector2 &mousePosition);
    void sEnemySpawner();
    void sLifespan();
    void sGUI();
    void sMovement();
    void sRender();
    void sCollision();
    void sUserInput();
    void createBackgroundImage();

};

#endif // GAME_H