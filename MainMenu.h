#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

#include "Services/Button.hpp"
#include "Services/Text.hpp"

class MainMenu {
private:
    Text nameText;

    Text infoText;

    Button startGameButton;

    Button lowLevelModeButton;
    Button mediumLevelModeButton;
    Button highLevelModeButton;

    sf::Color BackgroundColor = sf::Color::White;

    sf::Texture normalPlayButtonTexture;
    sf::Texture hoveredPlayButtonTexture;
    sf::Texture pressedPlayButtonTexture;

    sf::RectangleShape background;

    void setEasy();
    void setMedium();
    void setHard();

    void updateInfoText();

    sf::Vector2u fieldSize;
    unsigned mineAmount;
    std::string difficulty;

    sf::Vector2u windowSize;

public:
    MainMenu(sf::RenderWindow& window, sf::Font& font, std::function<void()> play);

    void proccessButtons(sf::RenderWindow& targetWindow, sf::Vector2i mousePos);

    void draw(sf::RenderWindow& targetWindow);
    void resize(sf::RenderWindow& targetWindow);

    std::pair<sf::Vector2u, unsigned> getGameInfo();
};