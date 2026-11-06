#include <SFML/Graphics.hpp>

#include "Mineswapers.h"

#include <iostream>

Mineswapers::Mineswapers()
    : window(sf::VideoMode({1280, 800}), "Minesweepers"),
      font("Fonts/IosevkaCharonMono-Light.ttf"),
      game(window, [this]() { home(); }),
      mainMenu(window, font, [this]() { startGame(); })
{
    window.setFramerateLimit(60);
    window.requestFocus();

}

void Mineswapers::run() {
    sf::Clock deltaClock;

    while (window.isOpen()) {
        float dt = deltaClock.restart().asSeconds();

        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            switch (stage) {
                case WindowStage::currentGame:
                    proccessGameUI(*event);
                    break;

                case WindowStage::mainMenu:
                    this->proccessMainMenu(*event);
                    break;

                default:
                    std::cerr << "Unknown stage\n";
                    break;
            }
        }

        window.clear(sf::Color::Black);

        switch (stage) {
            case WindowStage::currentGame:
                drawGame(dt);
                break;

            case WindowStage::mainMenu:
                this->drawHomeMenuUI();
                break;

            default:
                std::cerr << "Unknown stage\n";
                break;
        }

        window.display();
    }
}

void Mineswapers::drawGame(float deltaTime) {
    this->game.updateAnimations(deltaTime);
    this->game.UpdateUI(sf::Mouse::getPosition(this->window));
    this->game.draw();
}

void Mineswapers::proccessGameUI(const sf::Event& event) {
    if (event.is<sf::Event::Resized>())
        game.resize();

    if (event.is<sf::Event::MouseLeft>())
        game.HighlightCell({-1, -1});

    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
        game.HighlightCell(mouseMoved->position);

    if (const auto* mousePressed =
            event.getIf<sf::Event::MouseButtonPressed>()) {
        switch (mousePressed->button) {
            case sf::Mouse::Button::Left:
                game.OpenCell(mousePressed->position);
                break;

            case sf::Mouse::Button::Right:
                game.FlagCell(mousePressed->position);
                break;

            default:
                break;
        }
    }
}

void Mineswapers::proccessMainMenu(const sf::Event &event) {
    this->mainMenu.proccessButtons(this->window, sf::Mouse::getPosition(this->window));
    if (event.is<sf::Event::Resized>())
        this->mainMenu.resize(this->window);
}

void Mineswapers::drawHomeMenuUI() {
    mainMenu.draw(this->window);
}

void Mineswapers::prepareGame() {
    this->game.setGamePrefences(this->fieldSize, this->mineAmount);
}

void Mineswapers::startGame() {
    if (!this->isGameReadyToStart)
        return;

    this->fieldSize = this->mainMenu.getGameInfo().first;
    this->mineAmount = this->mainMenu.getGameInfo().second;

    this->prepareGame();

    this->stage = WindowStage::currentGame;
}

void Mineswapers::home() {
    this->game.restart();
    this->mainMenu.resize(this->window);
    this->stage = WindowStage::mainMenu;
}
