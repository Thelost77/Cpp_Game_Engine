#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

#include "imgui.h"
#include "imgui-SFML.h"


// Should be updated to the correct paths when first used
// Windows
#define FONT_PATH "C:\\Projekty\\Cpp_Game_Engine\\Cpp_Game_Engine\\src\\resources\\fonts\\"
#define CONFIG_PATH "C:\\Projekty\\Cpp_Game_Engine\\Cpp_Game_Engine\\src\\resources\\config.txt"

// Mac
// #define FONT_PATH "/Users/wiktorziebka/Projects/Cpp_Game_Engine/src/resources/fonts/"
// #define CONFIG_PATH "/Users/wiktorziebka/Projects/Cpp_Game_Engine/src/resources/config.txt"

class AShape
{
public:
    sf::Shape *shape = nullptr;
    sf::Text text{};
    std::string name;
    float color[3];

    float scale = 1.0f;
    bool visible = true;
    float speedX, speedY;

    void checkBounce(int windowWidth, int windowHeight)
    {
        const sf::FloatRect& globalBounds = shape->getGlobalBounds();
        if (globalBounds.left <= 0 || globalBounds.left + globalBounds.width >= windowWidth)
        {
            speedX = -speedX;
        }
        if (globalBounds.top <= 0 || globalBounds.top + globalBounds.height >= windowHeight)
        {
            speedY = -speedY;
        }
    }

    void move()
    {
        shape->move(speedX, speedY);
        setTextPosition();
    }

    void draw(sf::RenderWindow &window) const
    {
        if (!visible)
            return;

        shape->setScale(scale, scale);
        shape->setFillColor(sf::Color((color[0] * 255.0f), (color[1] * 255.0f), (color[2] * 255.0f)));

        window.draw(*shape);
        window.draw(text);
    }


    void setTextPosition()
    {
        auto textBound = text.getLocalBounds();
        const auto &shapeBounds = shape->getGlobalBounds();
        float x = shapeBounds.left + shapeBounds.width / 2 - textBound.width / 2;
        float y = shapeBounds.top + shapeBounds.height / 2 - textBound.height / 2;
        text.setPosition(x, y);
    }
};

int main()
{
    int windowWidth = 800;
    int windowHeight = 600;

    std::vector<AShape> shapes;

    std::ifstream fin(CONFIG_PATH);
    if (!fin)
    {
        std::cerr << "Error opening file\n";
        exit(-1);
    }

    sf::Font font;
    sf::Color textColor;
    int textSize;

    std::string configType;
    while (fin >> configType)
    {
        if (configType == "Window")
        {
            fin >> windowWidth >> windowHeight;
        }
        if (configType == "Font")
        {
            std::string fontPath;
            fin >> fontPath;
            if (!font.loadFromFile(FONT_PATH + fontPath))
            {
                std::cerr << "Error loading font\n";
                exit(-1);
            }
            int r, g, b;
            fin >> textSize >> r >> g >> b;
            textColor = sf::Color(r, g, b);
        }
        if (configType == "Circle" || configType == "Rectangle")
        {
            AShape shape{};
            std::string name;
            float x, y, speedX, speedY, width, height;
            int r, g, b;
            fin >> name >> x >> y >> speedX >> speedY >> r >> g >> b >> width;

            if (configType == "Circle") {
                shape.shape = new sf::CircleShape(width);
            }
            else 
            {
                fin >> height;
                shape.shape = new sf::RectangleShape(sf::Vector2f(width, height));
            }

            sf::Color color = sf::Color(r, g, b);
            shape.shape->setFillColor(color);
            shape.shape->setPosition(x, y);
            shape.speedX = speedX;
            shape.speedY = speedY;
            shape.text = sf::Text(name, font, textSize);
            shape.name = name;
            shape.color[0] = r / 255.0f;
            shape.color[1] = g / 255.0f;
            shape.color[2] = b / 255.0f;
            shape.setTextPosition();
            shapes.push_back(shape);
        }
    }

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML works!");
    ImGui::SFML::Init(window);
    //window.setFramerateLimit(60);

    sf::Clock deltaClock;
    int selectedShape = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("UI");
        std::vector<const char *> shapeNames;
        for (const auto &shape : shapes)
        {
            shapeNames.push_back(shape.name.c_str());
        }

        if (ImGui::BeginCombo("Shapes", shapeNames[selectedShape]))
        {
            for (int n = 0; n < shapeNames.size(); n++)
            {
                bool isSelected = (selectedShape == n);
                if (ImGui::Selectable(shapeNames[n], isSelected))
                {
                    selectedShape = n;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (shapes.size() > 0)
        {
            auto &selectedShapeObj = shapes[selectedShape];
            ImGui::Checkbox("Visible", &selectedShapeObj.visible);
            ImGui::ColorEdit3("Color", selectedShapeObj.color);
            ImGui::SliderFloat("Scale", &selectedShapeObj.scale, 0.0f, 2.0f);
            ImGui::InputFloat("SpeedX", &selectedShapeObj.speedX);
            ImGui::InputFloat("SpeedY", &selectedShapeObj.speedY);
        }
        ImGui::Text("Shapes");
        ImGui::End();

        window.clear();
        for (auto &shape : shapes)
        {
            shape.checkBounce(windowWidth, windowHeight);
            shape.move();
            shape.draw(window);
        }
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}

#include <SFML/Graphics.hpp>
#include "Game.h"

int main()
{
    Game g{CONFIG_PATH};
    g.run();
}