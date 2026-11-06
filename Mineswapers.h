#pragma once

#include <SFML/Graphics.hpp>

#include "Game.h"
#include "MainMenu.h"
#include "Services/Text.hpp"
#include "Services/Button.hpp"

enum WindowStage {
    mainMenu, currentGame
};

enum GameLevel {
    low, medium, high, configurable
};

class Mineswapers {
private:
    sf::RenderWindow window;
    sf::Font font;

    Game game;
    MainMenu mainMenu;

    GameLevel gameLevel = GameLevel::medium;
    WindowStage stage = WindowStage::mainMenu;

    sf::Vector2u fieldSize = sf::Vector2u(20, 15);
    unsigned int mineAmount = 30;

    bool isGameReadyToStart = true;

    void proccessGameUI(const sf::Event& event);
    void proccessMainMenu(const sf::Event& event);

    void drawGame(float deltaTime);
    void drawHomeMenuUI();

    void startGame();
    void prepareGame();

    void home();

public:
    Mineswapers();

    void run();
};