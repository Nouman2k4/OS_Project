#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 640), "Complex SFML Maze");

    // Define maze colors
    sf::Color wallColor = sf::Color::Blue;     // Blue
    sf::Color bgColor = sf::Color::Black;      // Black
    sf::Color pacmanColor = sf::Color::Yellow; // Pacman64's color
    sf::Color foodColor = sf::Color::Green;    // Food color

    // Create maze shapes
    sf::RectangleShape wall(sf::Vector2f(20.f, 20.f)); // Adjusted size for maze walls
    wall.setFillColor(wallColor);

    sf::CircleShape food(5.f); // Food shape
    food.setFillColor(foodColor);

    sf::Texture pacmanTexture;
    if (!pacmanTexture.loadFromFile("pacman.png"))
    {
        return EXIT_FAILURE;
    }

    // Create Pacman sprite
    sf::Sprite pacmanSprite(pacmanTexture);
    pacmanSprite.setScale(0.1f, 0.1f);

    // Define maze layout with food
    int maze[29][32] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
        {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
        {0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}};

    srand(time(0));
    for (int y = 0; y < 29; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            if (maze[y][x] == 1)
            {                   // 20% chance of placing food in an empty space
                maze[y][x] = 2; // Place food
            }
        }
    }
    // Pacman's initial position
    int pacmanX = 1;
    int pacmanY = 1;

    // Score
    int score = 0;

    // Draw the maze and Pacman
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle key presses to move Pacman
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Up)
                {
                    if (pacmanY == 0)
                        pacmanY = 28; // Wrap around if reaching the top
                    else if (maze[pacmanY - 1][pacmanX] == 1)
                        pacmanY--;
                    else if (maze[pacmanY - 1][pacmanX] == 2)
                    {
                        pacmanY--;
                        score++;
                        maze[pacmanY][pacmanX] = 1; // Remove food after eating
                    }
                }
                else if (event.key.code == sf::Keyboard::Down)
                {
                    if (pacmanY == 28)
                        pacmanY = 0; // Wrap around if reaching the bottom
                    else if (maze[pacmanY + 1][pacmanX] == 1)
                        pacmanY++;
                    else if (maze[pacmanY + 1][pacmanX] == 2)
                    {
                        pacmanY++;
                        score++;
                        maze[pacmanY][pacmanX] = 1; // Remove food after eating
                    }
                }
                else if (event.key.code == sf::Keyboard::Left)
                {
                    if (pacmanX == 0)
                        pacmanX = 31; // Wrap around if reaching the left
                    else if (maze[pacmanY][pacmanX - 1] == 1)
                        pacmanX--;
                    else if (maze[pacmanY][pacmanX - 1] == 2)
                    {
                        pacmanX--;
                        score++;
                        maze[pacmanY][pacmanX] = 1; // Remove food after eating
                    }
                }
                else if (event.key.code == sf::Keyboard::Right)
                {
                    if (pacmanX == 31)
                        pacmanX = 0; // Wrap around if reaching the right
                    else if (maze[pacmanY][pacmanX + 1] == 1)
                        pacmanX++;
                    else if (maze[pacmanY][pacmanX + 1] == 2)
                    {
                        pacmanX++;
                        score++;
                        maze[pacmanY][pacmanX] = 1; // Remove food after eating
                    }
                }
            }
        }

        window.clear(bgColor);

        // Draw maze walls and food
        for (int y = 0; y < 29; y++)
        {
            for (int x = 0; x < 32; x++)
            {
                if (maze[y][x] == 0)
                {
                    wall.setPosition(x * 20.f, y * 20.f);
                    window.draw(wall);
                }
                else if (maze[y][x] == 2)
                {
                    food.setPosition(x * 20.f + 8.f, y * 20.f + 8.f);
                    window.draw(food);
                }
            }
        }

        // Draw Pacman
        pacmanSprite.setPosition(pacmanX * 20.f + 6.f, pacmanY * 20.f + 5.f);
        window.draw(pacmanSprite);

        // Display score
        sf::Font font;
        if (!font.loadFromFile("times new roman.ttf"))
        {
            return EXIT_FAILURE;
        }
        sf::Text text;
        text.setFont(font);
        text.setString("Score: " + std::to_string(score));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.f, 600.f);
        window.draw(text);

        window.display();
    }

    return 0;
}
