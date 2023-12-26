#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>


int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 400), "SFML test 2!");
    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Yellow);

    sf::Font font;
    
    if ( !font.loadFromFile( "res/fonts/ComicSansMS3.ttf" ) )
    {
        std::cout << "Error loading file" << std::endl;
        
        //system( "pause" );
    }
    sf::Text text;
    text.setFont(font);
    
    text.setString("Hello world!!!");
    
     // in pixels, not points!
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);
    text.setPosition(10.0f, 10.0f);

    // For calculating FPS
    sf::Clock clock;
    float timeSinceLastUpdate = 0.f;
    int fps = 150;
    float timePerFrame = 1.f / fps; // 60 fps
    // window.setFramerateLimit(fps); // comment this our to uncap


    // The speed at which the ball moves
    float speed = 5.0f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle window resize
            if (event.type == sf::Event::Resized)
            {
                // Create a new view with the size of the window
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

         // Update the scene
        float elapsedTime = clock.restart().asSeconds();
        timeSinceLastUpdate += elapsedTime;
        if (timeSinceLastUpdate > timePerFrame)
        {
            // Calculate FPS
            float fps = 1.f / elapsedTime;
            text.setString("FPS: " + std::to_string(static_cast<int>(fps)));

            // Reset the timeSinceLastUpdate
            timeSinceLastUpdate = 0.f;
        }


        // Get the current position of the mouse
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // Convert it to world coordinates
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        // Get the current position of the shape
        sf::Vector2f shapePos = shape.getPosition();

        // Calculate the direction to move
        sf::Vector2f direction = worldPos - shapePos;

        // Normalize the direction
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0) 
        {
            direction /= length;
        }

        // Move the shape towards the mouse
        shape.move(direction * speed);

        window.clear();
        window.draw(shape);
        window.draw(text);
        window.display();
    }

    return 0;
}
