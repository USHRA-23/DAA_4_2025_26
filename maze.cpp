// maze.cpp — Procedural Maze Generator + Visual BFS Solver
// -----------------------------------------------------------
//  • Generates a perfect maze (depth-first search backtracker)
//  • Uses BFS to find the shortest path from Start → Goal
//  • Animated solving: press <Space> to watch the solver, <R> to regenerate
//  • Walls = dark grey, passages = white, path trail = green, solver head = cyan
//
//  Build (together with the other games):
//      g++ main.cpp snake.cpp maze.cpp hangman.cpp -std=c++17 \
//          -I<sfml_root>/include -L<sfml_root>/lib \
//          -lsfml-graphics -lsfml-window -lsfml-system -o 3Games.exe
// -----------------------------------------------------------
#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <queue>
#include <random>
#include "maze.hpp"   // declares runMazeSolver()

// --- Grid parameters ------------------------------------------------------
constexpr int CELL   = 20;          // pixel size of each maze cell
constexpr int COLS   = 31;          // must be odd  (horizontal cells)
constexpr int ROWS   = 23;          // must be odd  (vertical cells)
constexpr int WIDTH  = COLS * CELL; // window width
constexpr int HEIGHT = ROWS * CELL; // window height

enum Cell { WALL = 0, PASSAGE = 1 };

static int index(int x, int y) { return y * COLS + x; }

// --- Maze class -----------------------------------------------------------
class Maze {
public:
    Maze() { generate(); solve(); }

    void regenerate() {
        generate();
        solve();
    }

    // draw maze + (optionally) solver animation
    void draw(sf::RenderWindow &wnd, float dt) {
        wnd.clear(sf::Color::Black);

        // --- draw maze cells ---
        sf::RectangleShape rect(sf::Vector2f(static_cast<float>(CELL), static_cast<float>(CELL)));
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                if (grid[index(x, y)] == WALL) {
                    rect.setFillColor(sf::Color(60, 60, 60)); // dark grey walls
                    rect.setPosition(static_cast<float>(x * CELL), static_cast<float>(y * CELL));
                    wnd.draw(rect);
                }
            }
        }

        // --- animate solver path ---
        if (animate) {
            timer += dt;
            if (timer > stepTime && pathIndex < static_cast<int>(solution.size())) {
                timer = 0.f;
                ++pathIndex;
            }
        }

        // draw path so far
        rect.setFillColor(sf::Color::Green);
        for (int i = 0; i < pathIndex && i < static_cast<int>(solution.size()); ++i) {
            rect.setPosition(static_cast<float>(solution[i].x * CELL), static_cast<float>(solution[i].y * CELL));
            wnd.draw(rect);
        }

        // draw current solver head
        if (pathIndex < static_cast<int>(solution.size())) {
            rect.setFillColor(sf::Color::Cyan);
            rect.setPosition(static_cast<float>(solution[pathIndex].x * CELL), static_cast<float>(solution[pathIndex].y * CELL));
            wnd.draw(rect);
        }

        wnd.display();
    }

    void startAnimation() { animate = true; pathIndex = 0; timer = 0.f; }

private:
    // --- Maze generation (recursive backtracker) -------------------------
    void generate() {
        grid.assign(COLS * ROWS, WALL);

        std::mt19937 rng(std::random_device{}());
        std::stack<sf::Vector2i> st;
        st.push({1, 1});
        grid[index(1, 1)] = PASSAGE;

        const sf::Vector2i dirs[4] = {{2,0},{-2,0},{0,2},{0,-2}};
        std::uniform_int_distribution<int> dist(0, 3);

        while (!st.empty()) {
            auto cur = st.top();
            std::vector<int> shuffled{0,1,2,3};
            std::shuffle(shuffled.begin(), shuffled.end(), rng);

            bool carved = false;
            for (int i : shuffled) {
                sf::Vector2i nxt = cur + dirs[i];
                if (nxt.x <= 0 || nxt.x >= COLS-1 || nxt.y <= 0 || nxt.y >= ROWS-1) continue;
                if (grid[index(nxt.x, nxt.y)] == WALL) {
                    // carve passage and the wall between
                    grid[index(nxt.x, nxt.y)] = PASSAGE;
                    grid[index((cur.x + nxt.x)/2, (cur.y + nxt.y)/2)] = PASSAGE;
                    st.push(nxt);
                    carved = true;
                    break;
                }
            }
            if (!carved) st.pop();
        }
    }

    // --- BFS shortest-path solver ----------------------------------------
    void solve() {
        solution.clear();
        std::queue<sf::Vector2i> q;
        std::vector<int> prev(COLS * ROWS, -1);
        std::vector<int> dist(COLS * ROWS, -1);

        sf::Vector2i start(1, 1);
        sf::Vector2i goal(COLS - 2, ROWS - 2);

        q.push(start);
        dist[index(start.x, start.y)] = 0;

        const sf::Vector2i dirs4[4] = {{1,0},{-1,0},{0,1},{0,-1}};

        while (!q.empty()) {
            auto cur = q.front(); q.pop();
            if (cur == goal) break;

            for (auto d : dirs4) {
                sf::Vector2i nxt = cur + d;
                if (nxt.x < 0 || nxt.x >= COLS || nxt.y < 0 || nxt.y >= ROWS) continue;
                if (grid[index(nxt.x, nxt.y)] == WALL) continue;
                if (dist[index(nxt.x, nxt.y)] != -1) continue;
                dist[index(nxt.x, nxt.y)] = dist[index(cur.x, cur.y)] + 1;
                prev[index(nxt.x, nxt.y)] = index(cur.x, cur.y);
                q.push(nxt);
            }
        }

        // reconstruct path
        sf::Vector2i cur = goal;
        while (!(cur == start)) {
            solution.push_back(cur);
            int p = prev[index(cur.x, cur.y)];
            if (p == -1) break; // no path (shouldn’t happen in perfect maze)
            cur = { p % COLS, p / COLS };
        }
        solution.push_back(start);
        std::reverse(solution.begin(), solution.end());

        // reset animation counters
        pathIndex = 0;
        timer     = 0.f;
        animate   = false;
    }

    // --- fields -----------------------------------------------------------
    std::vector<int>           grid;       // WALL / PASSAGE
    std::vector<sf::Vector2i>  solution;   // BFS path cells

    // animation state
    bool  animate   = false;
    int   pathIndex = 0;
    float timer     = 0.f;
    const float stepTime = 0.05f; // seconds per step
};

// --------------------------------------------------------------------------
// Public entry used by main.cpp menu
// --------------------------------------------------------------------------
void runMazeSolver() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Maze Solver");
    window.setVerticalSyncEnabled(true);

    Maze maze;
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event e{};
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Space) maze.startAnimation();
                if (e.key.code == sf::Keyboard::R)     maze.regenerate();
            }
        }

        maze.draw(window, dt);
    }
}


