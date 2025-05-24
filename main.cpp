#include <SFML/Graphics.hpp>
#include <iostream>
#include "snake.hpp"
#include "maze.hpp"
#include "hangman.hpp"

int main() {
    int choice;

    while (true) {
        std::cout << "\n==== Welcome to 3Games ====\n";
        std::cout << "1. Snake Game\n";
        std::cout << "2. Maze Solver\n";
        std::cout << "3. Hangman\n";
        std::cout << "4. Exit\n";
        std::cout << "Choose a game to play: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                runSnakeGame();
                break;
            case 2:
                runMazeSolver();
                break;
            case 3:
                runHangman();
                break;
            case 4:
                std::cout << "Exiting. Bye!\n";
                return 0;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
//g++ main.cpp snake.cpp maze.cpp hangman.cpp -IC:/SFML/SFML-2.5.1/include -LC:/SFML/SFML-2.5.1/lib -lsfml-graphics -lsfml-window -lsfml-system