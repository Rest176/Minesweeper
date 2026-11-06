#pragma once

#include "Cell.h"
#include "Services/Button.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <chrono>

class Game {
private:
    std::vector<std::vector<Cell>> field;

    std::vector<sf::Texture> mineNumberTexture;
    sf::Texture mineTexture;
    sf::Texture cellTexture;
    sf::Texture emptyTexture;
    sf::Texture flagTexture;
    sf::Texture highlightTexture;
    sf::Texture highlightFlagTexture;

    sf::Font font;

    sf::Text infoText;
    sf::Text endGameText;

    std::vector<std::vector<sf::Sprite>> fieldSprites;


    unsigned int desiredCellSize = 48;
    sf::Vector2f targetCellSize;
    sf::Vector2u fieldSize;
    unsigned int mineAmount = 0;
    unsigned int flagsAmount = 0;

    sf::Vector2i highlightedCellPosition = {-1, -1};

    sf::Vector2u fieldWindowPosition;
    sf::Vector2u fieldWindowSize;

    Button restartButton;
    Button homeButton;

    sf::RenderWindow& window;

    sf::SoundBuffer hoverSoundBuffer;
    sf::SoundBuffer clickSoundBuffer;
    sf::SoundBuffer flagSoundBuffer;
    sf::SoundBuffer mineSoundBuffer;

    sf::Sound hoverSound;
    sf::Sound clickSound;
    sf::Sound flagSound;
    sf::Sound mineSound;

    std::chrono::time_point<std::chrono::steady_clock> startTime;
    long long endTime;

    bool wasFirstMove = false;

    bool isGameOver = false;
    bool isWin = false;

    [[nodiscard]] static int getRandomNumber(int min, int max);

    void unitilizeField(sf::Vector2i illegalCellPos);

    sf::Vector2i getClickedPosition(sf::Vector2i mousePos);
    bool isMouseInField(sf::Vector2i mousePos);

    void open(sf::Vector2i position);

    void openEmpty(sf::Vector2i position);

    void openAll();

    void openOpenedCell(sf::Vector2i position);

    void checkWin();

    void gameOver();
    void winGame();

    void setFieldSize(sf::Vector2u fieldSize);
    void setMineAmount(unsigned int amount);

    void updateTextOrigin();
    void updateWindowSize();
    void updateInfoText();

    void setEndTime();

    void loadMusic();
    void loadSprites();
    void loadTexts();
    void unitilizeGrathics();

    // Структура для отслеживания анимации переворота
    struct FlipAnimation {
        sf::Vector2i gridPos;
        float progress = 0.0f;
        const sf::Texture* targetTexture = nullptr;
        bool textureSwapped = false;
        float delay = 0.0f;
    };
    std::vector<FlipAnimation> activeAnimations;

public:
    Game(sf::RenderWindow& window, std::function<void()> home);

    void setGamePrefences(sf::Vector2u fieldSize, unsigned int mineAmount);

    void OpenCell(sf::Vector2i mousePos);
    void FlagCell(sf::Vector2i mousePos);

    void HighlightCell(sf::Vector2i mousePos);
    void UpdateUI(sf::Vector2i mousePos);

    void draw();
    void resize();

    void setFieldWindowPosition(sf::Vector2u position);
    void setFieldWindowSize(sf::Vector2u size);
    void setTargetCellSize(unsigned int size);

    void updateAnimations(float dt);

    void restart();
};