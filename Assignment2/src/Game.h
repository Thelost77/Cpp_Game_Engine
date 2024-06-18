#include "Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

struct PlayerConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S, L;
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
    sf::RenderWindow    m_window; // the window we will draw to
    EntityManager       m_entities;  // vector of entities to maintain
    sf::Font            m_font;           // the font we will use to draw
    sf::Text            m_text;           // the score text to be drawn to the screen
    PlayerConfig        m_playerConfig;
    EnemyConfig         m_enemyConfig;
    BulletConfig        m_bulletConfig;
    sf::Clock           m_deltaClock;
    int                 m_score = 0;
    int                 m_currentFrame = 0;
    int                 m_lastEnemySpawnTime = 0;
    bool                m_paused = false; // whether we update game logic
    bool                m_running = true: // whether the game is running

    public:
    Game(const std::string &config);
    void init(const std::string &config);
    void run();
    void setPaused(bool paused);
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> e);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity)
    void sEnemySpawner();
    void sLifespan();
    void sGUI();
    void sMovement();
    void sRender();
    void sCollision();
    void sUserInput();

}