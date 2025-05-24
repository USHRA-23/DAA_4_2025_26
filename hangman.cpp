#include <iostream>
#include <string>
#include <set>
#include "hangman.hpp"

void runHangman() {
    std::string word = "dumbo";
    std::set<char> guessed;
    int lives = 6;

    std::cout << "Welcome to Hangman!\n";

    while (lives > 0) {
        bool won = true;

        // Display current guessed state
        for (char c : word) {
            if (guessed.count(c)) std::cout << c;
            else {
                std::cout << "_";
                won = false;
            }
        }
        std::cout << "\n";

        if (won) {
            std::cout << "You guessed it!\n";
            return;
        }

        std::cout << "Guess a letter: ";
        char guess;
        std::cin >> guess;

        if (word.find(guess) == std::string::npos) {
            lives--;
            std::cout << "Wrong! Lives left: " << lives << "\n";
        } else {
            guessed.insert(guess);
        }
    }

    std::cout << "You lost! Word was: " << word << "\n";
}
