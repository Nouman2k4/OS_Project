#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <ctime>

std::mutex mtx;

struct EnemyThreadArgs {
    int& enemyX;
    int& enemyY;
    int (*maze)[32];
};

struct PlayerThreadArgs {
    int& pacmanX;
    int& pacmanY;
    int (*maze)[32];
    int& score;
};

struct collisionThreadArgs{
    sf::Sprite& pacman;
    int& enemyX;
    int& enemyY;
    int& pacmanX;
    int& pacmanY;
    int (*maze)[32];
    int& lives;
};

void drawMaze(sf::RenderWindow& window, int maze[29][32], sf::RectangleShape& wall, sf::CircleShape& food, sf::Sprite (&cherry)[3]) {
    mtx.lock();
    window.clear(sf::Color::Black);

    int count = 0;

    for (int y = 0; y < 29; y++) {
        for (int x = 0; x < 32; x++) {
            if (maze[y][x] == 0) {
                wall.setPosition(x * 20.f, y * 20.f);
                window.draw(wall);
            } else if (maze[y][x] == 2) {
                food.setPosition(x * 20.f + 8.f, y * 20.f + 8.f);
                window.draw(food);
            } else if (maze[y][x] == 3) {
                cherry[count].setPosition(x * 20.f + 5.f, y * 20.f + 5.f);
                window.draw(cherry[count]);
                count++;
            }
        }
    }

    mtx.unlock();
}


// Function to draw the player
void drawPlayer(sf::RenderWindow& window, sf::Sprite& pacmanSprite, int pacmanX, int pacmanY) {
    mtx.lock(); 

    pacmanSprite.setPosition(pacmanX * 20.f + 6.f, pacmanY * 20.f + 5.f);
    window.draw(pacmanSprite);

    mtx.unlock();
}

// Function to handle player movement
void* movePlayer(void* arg) {
    PlayerThreadArgs* args = static_cast<PlayerThreadArgs*>(arg);

    int& pacmanX = args->pacmanX;
    int& pacmanY = args->pacmanY;
    int (*maze)[32] = args->maze;
    int& score = args->score;

    while (true) {
        mtx.lock(); 
    
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            if (pacmanY > 0 && (maze[pacmanY - 1][pacmanX] == 1 || maze[pacmanY - 1][pacmanX] == 2 || maze[pacmanY - 1][pacmanX] == 3))
                pacmanY--;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            if (pacmanY < 28 && (maze[pacmanY + 1][pacmanX] == 1 || maze[pacmanY + 1][pacmanX] == 2 || maze[pacmanY + 1][pacmanX] == 3))
                pacmanY++;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (pacmanX > 0 && (maze[pacmanY][pacmanX - 1] == 1 || maze[pacmanY][pacmanX - 1] == 2 || maze[pacmanY][pacmanX - 1] == 3))
                pacmanX--;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (pacmanX < 32 && (maze[pacmanY][pacmanX + 1] == 1 || maze[pacmanY][pacmanX + 1] == 2 || maze[pacmanY][pacmanX + 1] == 3))
                pacmanX++;
        }

        mtx.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Pause for smoother movement
    }
}

void* checkCollision(void* arg) {
    collisionThreadArgs* args = static_cast<collisionThreadArgs*>(arg);

    sf::Sprite& pacman = args->pacman;
    int& enemyX = args->enemyX;
    int& enemyY = args->enemyY;
    int& pacmanX = args->pacmanX;
    int& pacmanY = args->pacmanY;
    int& lives =args->lives;
    int (*maze)[32] = args->maze;

    while (true) {
        mtx.lock();

        if (pacmanX==enemyX&&pacmanY==enemyY) {
            pacmanX=1;
            pacmanY=1;
            lives--;
            pacman.setPosition(pacmanX * 20.f + 6.f, pacmanY * 20.f + 5.f);
        }

        mtx.unlock();
    }
}

void* updateScore(void* arg){
    PlayerThreadArgs* args=static_cast<PlayerThreadArgs*>(arg);

    int& pacmanX = args->pacmanX;
    int& pacmanY = args->pacmanY;
    int (*maze)[32] = args->maze;
    int& score = args->score;


    while (true) {
        mtx.lock();

        if (maze[pacmanY][pacmanX]==2 ) {
            score++;
            maze[pacmanY][pacmanX]=1;
        }
        else if(maze[pacmanY][pacmanX]==3){
            score+=10;
            maze[pacmanY][pacmanX]=1;
        }

        mtx.unlock();
    }   
}

// Function to draw the Enemies
void drawEnemy(sf::RenderWindow& window, sf::Sprite& ghost, int enemyX, int enemyY) {
    mtx.lock();
    
    ghost.setPosition(enemyX * 20.f + 5.f, enemyY * 20.f + 5.f);
    window.draw(ghost);
    
    mtx.unlock();
}

void* moveEnemy(void* arg) {
    EnemyThreadArgs* args = static_cast<EnemyThreadArgs*>(arg);

    int& enemyX = args->enemyX;
    int& enemyY = args->enemyY;
    int (*maze)[32] = args->maze;
    
    int direction=0;

    while (true) {
        mtx.lock();
        
        bool canMoveUp = (enemyY > 0 && maze[enemyY - 1][enemyX] != 0);
        bool canMoveDown = (enemyY < 28 && maze[enemyY + 1][enemyX] != 0);
        bool canMoveLeft = (enemyX > 0 && maze[enemyY][enemyX - 1] != 0);
        bool canMoveRight = (enemyX > 31 && maze[enemyY][enemyX + 1] != 0);

        if (direction == 0 && canMoveUp)
            enemyY--;
        else if (direction == 1 && canMoveDown)
            enemyY++;
        else if (direction == 2 && canMoveLeft)
            enemyX--;
        else if (direction == 3 && canMoveRight)
            enemyX++;
        else {
            do {
                direction = rand() % 4; // 0: Up, 1: Down, 2: Left, 3: Right
            } while ((direction == 0 && !canMoveUp) || 
                     (direction == 1 && !canMoveDown) ||
                     (direction == 2 && !canMoveLeft) ||
                     (direction == 3 && !canMoveRight));
        }
        
        mtx.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return nullptr;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(640, 640), "Complex SFML Maze");

    sf::Color wallColor = sf::Color::Blue;
    sf::Color bgColor = sf::Color::Black;
    sf::Color pacmanColor = sf::Color::Yellow;
    sf::Color foodColor = sf::Color::Yellow;

    sf::RectangleShape wall(sf::Vector2f(20.f, 20.f));
    wall.setFillColor(wallColor);

    sf::CircleShape food(3.f);
    food.setFillColor(foodColor);

    sf::Texture pacmanTexture;
    if (!pacmanTexture.loadFromFile("pacman.png")) {
        return EXIT_FAILURE;
    }

    sf::Sprite pacmanSprite(pacmanTexture);
    pacmanSprite.setScale(0.02f, 0.02f);

    sf::FloatRect bounds = pacmanSprite.getLocalBounds();
    pacmanSprite.setOrigin(bounds.width / 3.f, bounds.height / 3.f);

    sf::Texture redghost, blueghost, pinkghost, greenghost, cherry;
    redghost.loadFromFile("ghost1.png");
    blueghost.loadFromFile("ghost3.png");
    pinkghost.loadFromFile("ghost4.png");
    greenghost.loadFromFile("ghost2.png");
    cherry.loadFromFile("cherry.png");

    sf::Sprite cherrysprite[3];
    for(int i=0 ; i<3 ; i++){
        
        cherrysprite[i].setTexture(cherry);
        cherrysprite[i].setScale(0.04f, 0.04f);

        sf::FloatRect bounds = cherrysprite[i].getLocalBounds();
        cherrysprite[i].setOrigin(bounds.width / 3.f, bounds.height / 3.f);

    }

    sf::Sprite redsprite(redghost), bluesprite(blueghost), pinksprite(pinkghost), greensprite(greenghost);
    redsprite.setScale(0.03f, 0.03f);
    bluesprite.setScale(0.01f, 0.01f);
    pinksprite.setScale(0.04f, 0.04f);
    greensprite.setScale(0.04f, 0.04f);


    sf::FloatRect redbounds = redsprite.getLocalBounds(), bluebounds = bluesprite.getLocalBounds(), 
    pinkbounds = pinksprite.getLocalBounds(), greenbounds = greensprite.getLocalBounds();

    redsprite.setOrigin(redbounds.width / 3.f, redbounds.height / 3.f);
    bluesprite.setOrigin(bluebounds.width / 3.f, bluebounds.height / 3.f);
    pinksprite.setOrigin(pinkbounds.width / 3.f, pinkbounds.height / 3.f);
    greensprite.setOrigin(greenbounds.width / 3.f, greenbounds.height / 3.f);

    int maze[29][32] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,2,2,2,2,2,2,2},
        {0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0},
        {0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0},
        {0,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,2,2,2,2,3,2,2,2,0},
        {0,2,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,2,0},
        {0,2,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,2,0},
        {0,2,0,0,2,2,2,2,2,2,0,0,2,2,2,2,0,0,0,2,2,2,2,2,2,2,2,2,2,0,2,0},
        {0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,2,0},
        {0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,2,0},  
        {0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,2,0},
        {0,2,0,0,2,0,0,0,0,3,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,2,0},
        {2,2,2,2,2,2,2,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,2,2,2},
        {0,0,0,0,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,0,0},
        {0,0,0,0,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,0,0},
        {0,2,2,2,2,2,2,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,2,2,2},
        {0,2,0,0,0,0,2,0,0,2,0,0,2,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,0,2},
        {0,2,2,2,0,0,2,2,2,2,0,0,2,0,0,2,0,0,0,2,2,2,2,0,0,0,0,0,2,0,0,2},
        {0,0,0,2,0,0,2,0,0,2,2,2,2,2,2,2,0,0,0,2,0,0,2,2,2,2,2,2,2,0,0,2},
        {0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,0,2},
        {0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,0,2},
        {0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2},        
        {0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0},
        {0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0},
        {2,2,2,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,2,2},
        {0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0},        
        {0,2,2,2,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,2,2,2,0},
        {0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0},
        {0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0}
    };

    srand(time(nullptr));

    int pacmanX = 1;
    int pacmanY = 1;
    int score = 0,lives=3;

    int enemyX1 = 10, enemyY1 = 10;
    int enemyX2 = 20, enemyY2 = 10;
    int enemyX3 = 10, enemyY3 = 20;
    int enemyX4 = 20, enemyY4 = 20;

    pthread_t player_id, Score_id;
    pthread_t t_id1, t_id2, t_id3, t_id4;
    pthread_t collision_id1, collision_id2, collision_id3, collision_id4;

    PlayerThreadArgs playerArgs = {pacmanX, pacmanY, maze, score};
    collisionThreadArgs collisionArgs1 = {pacmanSprite, enemyX1, enemyY1, pacmanX, pacmanY, maze,lives};
    collisionThreadArgs collisionArgs2 = {pacmanSprite, enemyX2, enemyY2, pacmanX, pacmanY, maze,lives};
    collisionThreadArgs collisionArgs3 = {pacmanSprite, enemyX3, enemyY3, pacmanX, pacmanY, maze,lives};
    collisionThreadArgs collisionArgs4 = {pacmanSprite, enemyX4, enemyY4, pacmanX, pacmanY, maze,lives};

    pthread_create(&player_id, NULL, movePlayer, &playerArgs);

    pthread_create(&Score_id, NULL, updateScore, &playerArgs);

    EnemyThreadArgs enemyArgs1 = {enemyX1, enemyY1, maze};
    EnemyThreadArgs enemyArgs2 = {enemyX2, enemyY2, maze};
    EnemyThreadArgs enemyArgs3 = {enemyX3, enemyY3, maze};
    EnemyThreadArgs enemyArgs4 = {enemyX4, enemyY4, maze};

    pthread_create(&t_id1, NULL, moveEnemy, &enemyArgs1);
    pthread_create(&t_id2, NULL, moveEnemy, &enemyArgs2);
    pthread_create(&t_id3, NULL, moveEnemy, &enemyArgs3);
    pthread_create(&t_id4, NULL, moveEnemy, &enemyArgs4);



    pthread_create(&collision_id1, NULL, checkCollision, &collisionArgs1);
    pthread_create(&collision_id2, NULL, checkCollision, &collisionArgs2);
    pthread_create(&collision_id3, NULL, checkCollision, &collisionArgs3);
    pthread_create(&collision_id4, NULL, checkCollision, &collisionArgs4);


    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if(lives==0)
        window.close();
        drawMaze(window, maze, wall, food, cherrysprite);

        drawPlayer(window, pacmanSprite, pacmanX, pacmanY);

        drawEnemy(window, redsprite, enemyX1, enemyY1);
        drawEnemy(window, bluesprite, enemyX2, enemyY2);
        drawEnemy(window, pinksprite, enemyX3, enemyY3);
        drawEnemy(window, greensprite, enemyX4, enemyY4);

        sf::Font font;
        if (!font.loadFromFile("times new roman.ttf")) {
            return EXIT_FAILURE;
        }

        sf::Text text;
        text.setFont(font);
        text.setString("Score: " + std::to_string(score));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.f, 590.f);
        window.draw(text);
        
        sf::Text text1;
        text1.setFont(font);
        text1.setString("Lives : " + std::to_string(lives));
        text1.setCharacterSize(24);
        text1.setFillColor(sf::Color::White);
        text1.setPosition(10.f, 610.f);
        window.draw(text1);
        
        window.display();
    }
    
    return 0;
}
