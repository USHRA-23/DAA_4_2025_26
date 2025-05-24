// snake.cpp — Advanced Snake game logic (no main())
// ---------------------------------------------------
//  Call runSnakeGame() from your central main.cpp menu.
//  Build example (MinGW/Clang, 64-bit, dynamic linking):
//      g++ main.cpp snake.cpp maze.cpp hangman.cpp -std=c++17 \
//          -I<sfml_root>/include -L<sfml_root>/lib \
//          -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o 3Games.exe
//  Make sure a font file (arial.ttf or similar) sits next to the executable.
// ---------------------------------------------------
#include <SFML/Graphics.hpp>
#include <deque>
#include <random>
#include <string>
#include <iostream>
#include "snake.hpp"   // declares runSnakeGame()

// --- Constants -------------------------------------------------------------
constexpr int CELL   = 20;   // one logical grid square in pixels
constexpr int WIDTH  = 800;  // window width  (multiple of CELL)
constexpr int HEIGHT = 600;  // window height (> HEADER and multiple of CELL)
constexpr int HEADER = 40;   // UI bar height at top of window
const sf::Color BG_COLOR(30, 30, 30); // dark neutral background

// --- Data types ------------------------------------------------------------
struct Segment { int x, y; }; // integer grid position of each snake block

// --- Game class ------------------------------------------------------------
class Game {
public:
    Game() :
        window(sf::VideoMode(WIDTH, HEIGHT), "Snake – SFML", sf::Style::Titlebar | sf::Style::Close),
        rng(std::random_device{}()),
        distX(0, (WIDTH / CELL) - 1),
        distY(0, ((HEIGHT - HEADER) / CELL) - 1) {

        window.setVerticalSyncEnabled(true);
        if (!font.loadFromFile("arial.ttf")) {
            throw std::runtime_error("Failed to load arial.ttf — place a font file next to the executable or change the path.");
        }

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10.f, 5.f);

        scoreBox.setSize(sf::Vector2f(static_cast<float>(WIDTH), static_cast<float>(HEADER)));
        scoreBox.setFillColor(sf::Color(50, 50, 50)); // subtle grey bar

        reset();
    }

    void run() {
        sf::Clock deltaClock;
        while (window.isOpen()) {
            processEvents();

            if (!gameOver) {
                // accumulate time and advance the snake at a fixed rate
                timer += deltaClock.restart();
                while (timer.asSeconds() > 1.f / speed) {
                    timer -= sf::seconds(1.f / speed);
                    update();
                }
            } else {
                // pause timer so dt doesn’t explode when restarting
                deltaClock.restart();
            }

            render();
        }
    }

private:
    // ---------- helpers ----------------------------------------------------
    void reset() {
        snake.clear();
        snake.push_back({10, 10});
        snake.push_back({9, 10});
        snake.push_back({8, 10});

        dir = {1, 0};
        speed = 8.f;           // cells / second
        score = 0;
        gameOver = false;
        timer   = sf::Time::Zero;

        placeFood();
    }

    void placeFood() {
        while (true) {
            food = {distX(rng), distY(rng)};
            bool onSnake = false;
            for (auto &seg : snake) {
                if (seg.x == food.x && seg.y == food.y) { onSnake = true; break; }
            }
            if (!onSnake) break;
        }
    }

    void processEvents() {
        sf::Event e{};
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();

            if (e.type == sf::Event::KeyPressed) {
                if (!gameOver) {
                    if (e.key.code == sf::Keyboard::Up    && dir.y != 1)  dir = {0, -1};
                    if (e.key.code == sf::Keyboard::Down  && dir.y != -1) dir = {0, 1};
                    if (e.key.code == sf::Keyboard::Left  && dir.x != 1)  dir = {-1, 0};
                    if (e.key.code == sf::Keyboard::Right && dir.x != -1) dir = {1, 0};
                }
                if (e.key.code == sf::Keyboard::R && gameOver) reset();
            }
        }
    }

    void update() {
        Segment newHead{ snake.front().x + dir.x, snake.front().y + dir.y };

        int columns = WIDTH / CELL;
        int rows    = (HEIGHT - HEADER) / CELL;

        if (newHead.x < 0)            newHead.x = columns - 1;
        else if (newHead.x >= columns) newHead.x = 0;
        if (newHead.y < 0)            newHead.y = rows - 1;
        else if (newHead.y >= rows)   newHead.y = 0;

        for (auto &seg : snake) {
            if (seg.x == newHead.x && seg.y == newHead.y) { gameOver = true; return; }
        }

        snake.push_front(newHead);

        if (newHead.x == food.x && newHead.y == food.y) {
            ++score;
            speed += 0.5f; // ramp difficulty
            placeFood();
        } else {
            snake.pop_back();
        }
    }

    void render() {
        window.clear(BG_COLOR);

        window.draw(scoreBox);
        scoreText.setString("Score: " + std::to_string(score) + (gameOver ? "  –  Press R to Restart" : ""));
        window.draw(scoreText);

        sf::CircleShape foodShape(static_cast<float>(CELL) / 2.f - 1.f);
        foodShape.setFillColor(sf::Color::Red);
        foodShape.setPosition(static_cast<float>(food.x * CELL), static_cast<float>(food.y * CELL + HEADER));
        window.draw(foodShape);

        sf::RectangleShape block(sf::Vector2f(static_cast<float>(CELL - 1), static_cast<float>(CELL - 1)));
        block.setFillColor(sf::Color::Green);
        for (auto &seg : snake) {
            block.setPosition(static_cast<float>(seg.x * CELL), static_cast<float>(seg.y * CELL + HEADER));
            window.draw(block);
        }

        window.display();
    }

    // ---------- members ----------------------------------------------------
    sf::RenderWindow            window;
    sf::Font                    font;
    sf::Text                    scoreText;
    sf::RectangleShape          scoreBox;

    std::deque<Segment>         snake;
    Segment                     food{};
    sf::Vector2i                dir{1, 0};
    float                       speed = 8.f;
    int                         score = 0;
    bool                        gameOver = false;

    sf::Time                    timer{};

    std::mt19937                        rng;
    std::uniform_int_distribution<int>  distX;
    std::uniform_int_distribution<int>  distY;
};

// --------------------------------------------------------------------------
// Exposed function so main.cpp can launch the snake game.
// --------------------------------------------------------------------------
void runSnakeGame() {
    try {
        Game game;
        game.run();
    } catch (const std::exception &ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
    }
}


