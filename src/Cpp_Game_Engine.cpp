#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

#include "imgui.h"
#include "imgui-SFML.h"

class AShape
{
public:
    sf::CircleShape circle{};
    sf::RectangleShape rectangle{};
    sf::Text text{};
    float speedX, speedY;

    void checkBounce(int windowWidth, int windowHeight)
    {
        auto& circlePos = circle.getPosition();
        auto& rectSize = rectangle.getSize();
        auto& rectanglePos = rectangle.getPosition();
        auto circleRadius = circle.getRadius();
        if (circleRadius > 0)
        {
            if (circlePos.x <= 0 || circlePos.x + circleRadius * 2 >= windowWidth)
            {
                speedX = -speedX;
            }
            if (circlePos.y <= 0 || circlePos.y + circleRadius * 2 >= windowHeight)
            {
                speedY = -speedY;
            }
        }
        if (rectSize.x > 0 && rectSize.y > 0)
        {
            if (rectanglePos.x <= 0 || rectanglePos.x + rectSize.x >= windowWidth)
            {
                speedX = -speedX;
            }

            if (rectanglePos.y <= 0 || rectanglePos.y + rectSize.y >= windowHeight)
            {
                speedY = -speedY;
            }
        }
    }

    void move()
    {
        circle.move(speedX, speedY);
        rectangle.move(speedX, speedY);
        text.move(speedX, speedY);
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(circle);
        window.draw(rectangle);
        window.draw(text);
    }
};

int main()
{
    int windowWidth = 800;
    int windowHeight = 600;

    std::vector<AShape> shapes;

    auto filePath = "C:\\Projekty\\Cpp_Game_Engine\\Cpp_Game_Engine\\src\\resources\\config.txt";

    std::ifstream fin(filePath);
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
            std::string fontsFolderPath = "C:\\Projekty\\Cpp_Game_Engine\\Cpp_Game_Engine\\src\\resources\\fonts\\";
            fin >> fontPath;
            if (!font.loadFromFile(fontsFolderPath + fontPath))
            {
                std::cerr << "Error loading font\n";
                exit(-1);
            }
            int r, g, b;
            fin >> textSize >> r >> g >> b;
            textColor = sf::Color(r, g, b);
        }
        if (configType == "Circle")
        {
            AShape shape{};
            std::string name;
            float x, y, speedX, speedY, radius;
            int r, g, b;
            fin >> name >> x >> y >> speedX >> speedY >> r >> g >> b >> radius;
            shape.circle = sf::CircleShape(radius);
            shape.circle.setFillColor(sf::Color(r, g, b));
            shape.circle.setPosition(x, y);
            shape.speedX = speedX;
            shape.speedY = speedY;
            shape.text = sf::Text(name, font, textSize);
            auto bound = shape.text.getLocalBounds();
            shape.text.setPosition(x + radius - bound.width / 2, y + radius - bound.height / 2);
            shapes.push_back(shape);
        }
        if (configType == "Rectangle")
        {
            AShape shape{};
            std::string name;
            float x, y, speedX, speedY, width, height;
            int r, g, b;
            fin >> name >> x >> y >> speedX >> speedY >> r >> g >> b >> width >> height;
            shape.rectangle = sf::RectangleShape(sf::Vector2f(width, height));
            shape.rectangle.setFillColor(sf::Color(r, g, b));
            shape.rectangle.setPosition(x, y);
            shape.speedX = speedX;
            shape.speedY = speedY;
            shape.text = sf::Text(name, font, textSize);
            auto bound = shape.text.getLocalBounds();
            shape.text.setPosition(x + width / 2 - bound.width / 2, y + height / 2 - bound.height / 2);
            shapes.push_back(shape);
        }
    }

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML works!");
    ImGui::SFML::Init(window);
    //window.setFramerateLimit(60);

    sf::Clock deltaClock;
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

        // Dorobić ImGui, w którym będzie combobox do wybrania odpowiedniego kształtu, a także pola do:
        // Zaznaczenia czy jest widoczny, zmiany koloru, Skalowania, Obu prędkości, Koloru i nazwy
        ImGui::Begin("Shapes");
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