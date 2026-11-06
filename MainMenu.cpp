#include <SFML/Graphics.hpp>

#include "MainMenu.h"

#include "Services/Button.hpp"
#include "Services/Text.hpp"

MainMenu::MainMenu(sf::RenderWindow& window, sf::Font& font, std::function<void()> play) :
    nameText(font),
    startGameButton(font),
    infoText(font),
    lowLevelModeButton(font),
    highLevelModeButton(font),
    mediumLevelModeButton(font)
{
    this->windowSize = window.getSize();

    // Настройка надписи с игрой
    this->nameText.setString("Minesweeper");
    this->nameText.setSize(128);
    this->nameText.setColor(sf::Color::Black);
    this->nameText.centerOrigin();
    this->nameText.setPosition(window.getSize().x / 2, window.getSize().y / 2 - 200);

    // Настройка надписи с информацией
    this->infoText.setSize(32);
    this->infoText.setColor(sf::Color::Black);
    this->setMedium();
    this->updateInfoText();

    //Настройка кнопки запуска
    this->startGameButton.setAction(play);
    this->startGameButton.setActionMode(Button::ActionMode::Release);
    this->startGameButton.setTextures("Sprites/Play Button/StartButton.png", "Sprites/Play Button/StartButtonHovered.png", "Sprites/Play Button/StartButtonPressed.png");
    this->startGameButton.centerOrigin();
    this->startGameButton.setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2 + 200));
    this->startGameButton.setSize(sf::Vector2f(150, 113));
    this->startGameButton.setHoverSound("Sounds/Hovered.ogg");
    this->startGameButton.setClickSound("Sounds/Pressed.mp3");

    //Настройка кнопки средней сложности
    this->mediumLevelModeButton.setAction([this]() { setMedium(); });
    this->mediumLevelModeButton.setTextures("Sprites/Button/Button.png", "Sprites/Button/ButtonHovered.png", "Sprites/Button/ButtonPressed.png");
    this->mediumLevelModeButton.setTextActive(true);
    this->mediumLevelModeButton.setAutoTextSize(true);
    this->mediumLevelModeButton.setString("Medium");
    this->mediumLevelModeButton.setColor(sf::Color(29.0, 29.0, 29.0));
    this->mediumLevelModeButton.centerOrigin();
    this->mediumLevelModeButton.setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2 + 50));
    this->mediumLevelModeButton.setSize(sf::Vector2f(100, 75));
    this->mediumLevelModeButton.setHoverSound("Sounds/Hovered.ogg");
    this->mediumLevelModeButton.setClickSound("Sounds/Pressed.mp3");


    //Настройка кнопки легкой сложности
    this->lowLevelModeButton.setAction([this]() { setEasy(); });
    this->lowLevelModeButton.setTextures("Sprites/Button/Button.png", "Sprites/Button/ButtonHovered.png", "Sprites/Button/ButtonPressed.png");
    this->lowLevelModeButton.setTextActive(true);
    this->lowLevelModeButton.setAutoTextSize(true);
    this->lowLevelModeButton.setString("Low");
    this->lowLevelModeButton.setColor(sf::Color(29.0, 29.0, 29.0));
    this->lowLevelModeButton.centerOrigin();
    this->lowLevelModeButton.setPosition(sf::Vector2f(window.getSize().x / 2 - 150, window.getSize().y / 2 + 50));
    this->lowLevelModeButton.setSize(sf::Vector2f(100, 75));
    this->lowLevelModeButton.setHoverSound("Sounds/Hovered.ogg");
    this->lowLevelModeButton.setClickSound("Sounds/Pressed.mp3");


    //Настройка кнопки сложной сложности
    this->highLevelModeButton.setAction([this]() { setHard(); });
    this->highLevelModeButton.setTextures("Sprites/Button/Button.png", "Sprites/Button/ButtonHovered.png", "Sprites/Button/ButtonPressed.png");
    this->highLevelModeButton.setTextActive(true);
    this->highLevelModeButton.setAutoTextSize(true);
    this->highLevelModeButton.setString("Hard");
    this->highLevelModeButton.setColor(sf::Color(29.0, 29.0, 29.0));
    this->highLevelModeButton.centerOrigin();
    this->highLevelModeButton.setPosition(sf::Vector2f(window.getSize().x / 2 + 150, window.getSize().y / 2 + 50));
    this->highLevelModeButton.setSize(sf::Vector2f(100, 75));
    this->highLevelModeButton.setHoverSound("Sounds/Hovered.ogg");
    this->highLevelModeButton.setClickSound("Sounds/Pressed.mp3");



    //Настройка фонового квадрата
    this->background.setFillColor(sf::Color(217, 217, 217));

    this->background.setSize({
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y),
    });

    this->background.setOrigin(
        this->background.getSize() / 2.f
    );

    this->background.setPosition({
        window.getSize().x / 2.f,
        window.getSize().y / 2.f
    });
}

void MainMenu::proccessButtons(sf::RenderWindow &targetWindow, sf::Vector2i mousePos) {
    this->startGameButton.update(targetWindow, mousePos);
    this->lowLevelModeButton.update(targetWindow, mousePos);
    this->highLevelModeButton.update(targetWindow, mousePos);
    this->mediumLevelModeButton.update(targetWindow, mousePos);
}

void MainMenu::draw(sf::RenderWindow& targetWindow) {
    targetWindow.draw(this->background);
    this->nameText.draw(targetWindow);
    this->infoText.draw(targetWindow);
    this->startGameButton.draw(targetWindow);
    this->lowLevelModeButton.draw(targetWindow);
    this->highLevelModeButton.draw(targetWindow);
    this->mediumLevelModeButton.draw(targetWindow);
}

std::pair<sf::Vector2u, unsigned> MainMenu::getGameInfo() {
    return std::pair<sf::Vector2u, unsigned>(this->fieldSize, mineAmount);
}

void MainMenu::setEasy() {
    this->fieldSize = sf::Vector2u(9, 9);
    this->mineAmount = 10;
    this->difficulty = "Easy";

    this->updateInfoText();
}

void MainMenu::setMedium() {
    this->fieldSize = sf::Vector2u(16, 16);
    this->mineAmount = 40;
    this->difficulty = "Medium";

    this->updateInfoText();
}

void MainMenu::setHard() {
    this->fieldSize = sf::Vector2u(30, 16);
    this->mineAmount = 99;
    this->difficulty = "Hard";

    this->updateInfoText();
}

void MainMenu::updateInfoText() {
    std::string text = std::format("Difficulty: {} | Field size: {}*{} | Mine amount: {}", this->difficulty, this->fieldSize.x, this->fieldSize.y, this->mineAmount);

    this->infoText.setString(text);
    this->infoText.centerOrigin();
    this->infoText.setPosition(windowSize.x / 2, windowSize.y / 2 - 75 );
}

void MainMenu::resize(sf::RenderWindow& window)
{
    this->windowSize = window.getSize();

    window.setView(sf::View(sf::FloatRect(
        {0.f, 0.f},
        {
            static_cast<float>(windowSize.x),
            static_cast<float>(windowSize.y)
        }
    )));

    float centerX = windowSize.x / 2.f;
    float centerY = windowSize.y / 2.f;

    this->nameText.setPosition({
        centerX,
        centerY - 200.f
    });

    this->infoText.setPosition({
        centerX,
        centerY - 75.f
    });

    this->lowLevelModeButton.setPosition({
        centerX - 150.f,
        centerY + 50.f
    });

    this->mediumLevelModeButton.setPosition({
        centerX,
        centerY + 50.f
    });

    this->highLevelModeButton.setPosition({
        centerX + 150.f,
        centerY + 50.f
    });

    this->startGameButton.setPosition({
        centerX,
        centerY + 200.f
    });

    this->background.setSize(sf::Vector2f(
        windowSize.x,
        windowSize.y
    ));
    this->background.setOrigin(
        this->background.getSize() / 2.f
    );

    this->background.setPosition({
    centerX,
    centerY
    });
}