#include "Game.h"

#include "Cell.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <format>
#include <algorithm>
#include <cmath>

Game::Game(sf::RenderWindow& window, std::function<void()> home)
        : window(window),
          endGameText(this->font),
          infoText(this->font),
          fieldWindowSize(sf::Vector2u(window.getSize().x - fieldWindowPosition.x, window.getSize().y - fieldWindowPosition.y)),
          restartButton(this->font),
          homeButton(this->font),
            hoverSound(this->hoverSoundBuffer),
            clickSound(this->clickSoundBuffer),
            mineSound(this->mineSoundBuffer),
            flagSound(this->flagSoundBuffer)
{
    this->homeButton.setAction(home);

    this->loadTexts();
    this->loadMusic();
    this->loadSprites();

    // this->updateWindowSize();
    this->updateInfoText();

    this->startTime = std::chrono::steady_clock::now();
}

void Game::loadTexts() {
    if (!font.openFromFile("Fonts/IosevkaCharonMono-Light.ttf"))
    {
        std::cerr << "Не удалось загрузить шрифт!\n";
    }

    this->endGameText.setFillColor(sf::Color(250, 250, 250));

    float scale = static_cast<float>(this->window.getSize().y) / 350.f;
    unsigned int finalSize = static_cast<unsigned int>(32 * scale);
    this->endGameText.setCharacterSize(std::max(finalSize, 12u));

    this->updateTextOrigin();
    this->endGameText.setPosition(sf::Vector2f(std::round(this->window.getSize().x / 2), std::round(this->window.getSize().y / 2)));

    scale = static_cast<float>(this->window.getSize().y) / 350.f;
    finalSize = static_cast<unsigned int>(16 * scale);
    this->infoText.setPosition(sf::Vector2f(10.f, 5.f));
    this->infoText.setCharacterSize(std::max(finalSize, 12u));
    this->infoText.setString("Flags: 0/100\nTime: 00:05:29");
}

void Game::loadMusic() {
    if (!clickSoundBuffer.loadFromFile("Sounds/Pressed.mp3")) {
        std::cerr << "Error loading sound: Sounds/Pressed.mp3\n";
    }
    clickSound.setBuffer(this->clickSoundBuffer);

    if (!mineSoundBuffer.loadFromFile("Sounds/Bomb.wav")) {
        std::cerr << "Error loading sound: Sounds/Bomb.wav\n";
    }
    mineSound.setBuffer(this->mineSoundBuffer);

    if (!flagSoundBuffer.loadFromFile("Sounds/Flag.wav")) {
        std::cerr << "Error loading sound: Sounds/Flag.wav\n";
    }
    flagSound.setBuffer(this->flagSoundBuffer);

    if (!hoverSoundBuffer.loadFromFile("Sounds/Hovered.ogg")) {
        std::cerr << "Error loading sound: Sounds/Hovered.ogg\n";
    }
    hoverSound.setBuffer(this->hoverSoundBuffer);
    hoverSound.setVolume(25.f);

    this->restartButton.setHoverSound("Sounds/Hovered.ogg");
    this->restartButton.setClickSound("Sounds/Pressed.mp3");

    this->homeButton.setHoverSound("Sounds/Hovered.ogg");
    this->homeButton.setClickSound("Sounds/Pressed.mp3");
}

void Game::loadSprites() {
    std::vector<std::string> filePaths = {
        "One.png",
        "Two.png",
        "Three.png",
        "Four.png",
        "Five.png",
        "Six.png",
        "Seven.png",
        "Eight.png",
    };

    mineNumberTexture.reserve(filePaths.size());

    for (const auto& path : filePaths) {
        auto texture = sf::Texture("Sprites/" + path);
        if (texture.loadFromFile("Sprites/" + path)) {
            mineNumberTexture.push_back(texture);
        }
        else {
            std::cerr << "Error loading sprite: Sprite/" + path + "\n";
        }
    }

    if (!mineTexture.loadFromFile("Sprites/Cell with Mine.png")) {
        std::cerr << "Error loading sprite: Sprites/Mine.png\n";
    }
    if (!cellTexture.loadFromFile("Sprites/Cell.png")) {
        std::cerr << "Error loading sprite: Sprites/Cell.png\n";
    }

    if (!emptyTexture.loadFromFile("Sprites/Empty.png")) {
        std::cerr << "Error loading sprite: Sprites/Empty.png\n";
    }

    if (!flagTexture.loadFromFile("Sprites/Cell with flag.png")) {
        std::cerr << "Error loading sprite: Sprites/Cell with flag.png\n";
    }

    if (!highlightTexture.loadFromFile("Sprites/Pointed Cell.png")) {
        std::cerr << "Error loading sprite: Sprites/Pointed Cell.png\n";
    }

    if (!highlightFlagTexture.loadFromFile("Sprites/Choose Flagged Cell.png")) {
        std::cerr << "Error loading sprite: Sprites/Choose Flagged Cell.png\n";
    }
}

void Game::unitilizeGrathics() {
    this->fieldSprites.resize(this->fieldSize.x);
    for (auto& line : this->fieldSprites) {
        line.resize(this->fieldSize.y, sf::Sprite(this->cellTexture));
    }
}

void Game::setFieldSize(sf::Vector2u size) {
    if (size.x == 0 || size.y == 0)
        return;

    this->fieldSize = size;

    this->field.clear();
    this->field.resize(this->fieldSize.x);

    for (auto& line : this->field)
        line.resize(this->fieldSize.y);

    this->unitilizeGrathics();
    this->updateWindowSize();
}

void Game::setMineAmount(unsigned int amount) {
    if (fieldSize.x == 0 || fieldSize.y == 0)
        return;

    unsigned int cellCount = fieldSize.x * fieldSize.y;
    this->mineAmount = std::min(amount, cellCount - 1);

    this->field.assign(
        fieldSize.x,
        std::vector<Cell>(fieldSize.y)
    );
}

int Game::getRandomNumber(const int min, const int max) {
    // static гарантирует, что генератор инициализируется только один раз
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Равномерное распределение на отрезке [k, n]
    std::uniform_int_distribution<int> dist(min, max);

    return dist(gen);
}

void Game::unitilizeField(sf::Vector2i illegalCellPos) {
    if (this->mineAmount >= this->fieldSize.x * this->fieldSize.y)
        this->mineAmount = this->fieldSize.x * this->fieldSize.y - 1;

    unsigned int createdMineAmount = 0;

    while (createdMineAmount < this->mineAmount) {
        int x = this->getRandomNumber(0, this->fieldSize.x - 1);
        int y = this->getRandomNumber(0, this->fieldSize.y - 1);

        if (this->field[x][y].getMineStatus() || sf::Vector2i(x, y) == illegalCellPos) continue;

        this->field[x][y].makeMine();
        createdMineAmount++;

        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                if (i < 0 || i > this->fieldSize.x - 1 ||
                    j < 0 || j > this->fieldSize.y - 1 ||
                    field[i][j].getMineStatus()) continue;

                field[i][j].addCount();
            }
        }
    }
}

void Game::setEndTime() {
    auto current_time = std::chrono::steady_clock::now();
    this->endTime = std::chrono::duration_cast<std::chrono::seconds>(current_time - startTime).count();
}

void Game::updateInfoText() {
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - startTime).count();

    if (this->isGameOver || this->isWin)
        elapsed = this->endTime;

    long long hours = elapsed / 3600;
    long long minutes = (elapsed % 3600) / 60;
    long long seconds = elapsed % 60;

    std::string text = "Flags: " + std::to_string(this->flagsAmount) + "/" +
        std::to_string(this->mineAmount);

    text += std::format("\nTime: {:02}:{:02}:{:02}", hours, minutes, seconds);

    this->infoText.setString(text);

    const float menuHeight = static_cast<float>(fieldWindowPosition.y);

    // Сколько места оставляем тексту.
    // Справа резервируем место под кнопку Restart.
    const float padding = 10.f;
    const float restartArea = menuHeight;

    const float maxWidth =
        static_cast<float>(window.getSize().x) - restartArea - padding * 2.f;

    const float maxHeight = menuHeight - padding * 2.f;

    // Начинаем с большого размера и уменьшаем,
    // пока текст полностью не поместится.
    unsigned int fontSize =
        static_cast<unsigned int>(menuHeight * 0.35f);

    fontSize = std::max(fontSize, 1u);

    infoText.setCharacterSize(fontSize);

    while (fontSize > 1) {
        const auto bounds = infoText.getLocalBounds();

        if (bounds.size.x <= maxWidth &&
            bounds.size.y <= maxHeight)
        {
            break;
        }

        --fontSize;
        infoText.setCharacterSize(fontSize);
    }

    // Центрируем текст по вертикали,
    // но оставляем его прижатым к левому краю.
    const auto bounds = infoText.getLocalBounds();

    infoText.setOrigin({
        bounds.position.x,
        bounds.position.y + bounds.size.y / 2.f
    });

    infoText.setPosition({
        padding,
        menuHeight / 2.f
    });
}

void Game::resize()
{
    updateWindowSize();

    endGameText.setPosition({
        window.getSize().x / 2.f,
        window.getSize().y / 2.f
    });
}

void Game::draw() {
    const float menuHeight = static_cast<float>(fieldWindowPosition.y);

    sf::RectangleShape upMenu({
        static_cast<float>(window.getSize().x),
        menuHeight
    });

    upMenu.setFillColor(sf::Color(140, 140, 140));
    upMenu.setOutlineThickness(2.f);
    upMenu.setOutlineColor(sf::Color::Black);

    this->window.draw(upMenu);

    for (auto& line : this->fieldSprites) {
        for (auto& cell : line) {
            this->window.draw(cell);
        }
    }

    this->window.draw(upMenu);

    this->window.draw(this->infoText);

    if  (isGameOver || isWin) {
        sf::RectangleShape fade;
        fade.setSize(sf::Vector2f(this->window.getSize()));
        fade.setFillColor(sf::Color(128, 128, 128, 150));

        this->window.draw(fade);
        this->window.draw(this->endGameText);
    }

    this->updateInfoText();

    this->restartButton.draw(this->window);
    this->homeButton.draw(this->window);
}

sf::Vector2i Game::getClickedPosition(sf::Vector2i mousePos) {
    return sf::Vector2i((mousePos.x - this->fieldWindowPosition.x) / this->targetCellSize.x,
        (mousePos.y - this->fieldWindowPosition.y) / this->targetCellSize.y);
}

bool Game::isMouseInField(sf::Vector2i mousePos) {
    return mousePos.x >= this->fieldWindowPosition.x &&
        mousePos.x < this->fieldWindowPosition.x + this->fieldWindowSize.x &&
            mousePos.y >= this->fieldWindowPosition.y &&
                mousePos.y < this->fieldWindowPosition.y + this->fieldWindowSize.y;
}

void Game::open(sf::Vector2i position) {
    field[position.x][position.y].Open();
    const sf::Texture* targetTex = nullptr;

    if (field[position.x][position.y].getMineStatus()) {
        targetTex = &this->mineTexture;
    }
    else {
        unsigned short mineAmount = field[position.x][position.y].getMineAmount();
        if (mineAmount <= 0) {
            targetTex = &this->emptyTexture;
        }
        else {
            targetTex = &this->mineNumberTexture[mineAmount - 1];
        }
    }

    // Удаляем предыдущую анимацию переворота для этой ячейки (если есть)
    activeAnimations.erase(std::remove_if(activeAnimations.begin(), activeAnimations.end(),
        [&](const FlipAnimation& a) { return a.gridPos == position; }), activeAnimations.end());

    // Открываем ячейку моментально без задержки (delay = 0.0f)
    this->activeAnimations.push_back({position, 0.0f, targetTex, false, 0.0f});
}

void Game::updateAnimations(float dt) {
    const float animSpeed = 4.0f;

    for (auto it = activeAnimations.begin(); it != activeAnimations.end(); ) {
        // Отрабатываем задержку (для эффекта волны)
        if (it->delay > 0.0f) {
            it->delay -= dt;
            ++it;
            continue;
        }

        it->progress += dt * animSpeed;

        sf::Sprite& sprite = fieldSprites[it->gridPos.x][it->gridPos.y];

        float baseScaleX = targetCellSize.x / sprite.getLocalBounds().size.x;
        float basePosX = fieldWindowPosition.x + (it->gridPos.x * targetCellSize.x);

        if (it->progress >= 0.5f && !it->textureSwapped) {
            sprite.setTexture(*it->targetTexture);
            it->textureSwapped = true;
        }

        if (it->progress >= 1.0f) {
            sprite.setScale({baseScaleX, sprite.getScale().y});
            sprite.setPosition({basePosX, sprite.getPosition().y});
            it = activeAnimations.erase(it);
        } else {
            float scaleMultiplier = std::abs(std::cos(it->progress * 3.14159f));
            sprite.setScale({baseScaleX * scaleMultiplier, sprite.getScale().y});

            float currentWidth = sprite.getLocalBounds().size.x * baseScaleX * scaleMultiplier;
            float offset = (targetCellSize.x - currentWidth) / 2.0f;
            sprite.setPosition({basePosX + offset, sprite.getPosition().y});

            ++it;
        }
    }
}


void Game::openOpenedCell(sf::Vector2i position) {
    int startX = std::max(0, position.x - 1);
    int startY = std::max(0, position.y - 1);

    int endX = std::min(static_cast<int>(fieldSize.x) - 1, position.x + 1);
    int endY = std::min(static_cast<int>(fieldSize.y) - 1, position.y + 1);

    int flagsAround = 0;

    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            if (field[x][y].getFlaggedStatus())
                flagsAround++;
        }
    }

    if (flagsAround != field[position.x][position.y].getMineAmount())
        return;

    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            if (field[x][y].getFlaggedStatus() ||
                field[x][y].getOpenedStatus())
                continue;

            sf::Vector2i mousePos(
                static_cast<int>(fieldWindowPosition.x + (x + 0.5f) * targetCellSize.x),
                static_cast<int>(fieldWindowPosition.y + (y + 0.5f) * targetCellSize.y)
            );

            this->OpenCell(mousePos);

            if (isGameOver || isWin)
                return;
        }
    }
}


void Game::openEmpty(sf::Vector2i position) {
    this->open(position);

    for (int i = position.x - 1; i <= position.x + 1; i++) {
        for (int j = position.y - 1; j <= position.y + 1; j++) {
            if (i < 0 || i >= this->fieldSize.x ||
                j < 0 || j >= this->fieldSize.y) continue;

            if (field[i][j].getMineStatus() || field[i][j].getOpenedStatus()) {
                continue;
            }

            if (field[i][j].isEmpty()) {
                this->openEmpty(sf::Vector2i(i, j));
            } else {
                this->open(sf::Vector2i(i, j));
            }
        }
    }
}

void Game::OpenCell(sf::Vector2i mousePos) {
    if (!this->isMouseInField(mousePos)) return;

    sf::Vector2i clickedCellPos = this->getClickedPosition(mousePos);

    if (!this->wasFirstMove) {
        this->wasFirstMove = true;
        this->unitilizeField(clickedCellPos);
    }

    if (field[clickedCellPos.x][clickedCellPos.y].getFlaggedStatus())
        return;

    
    if (field[clickedCellPos.x][clickedCellPos.y].isEmpty()) {
        this->clickSound.play();
        this->openEmpty(clickedCellPos);
        this->checkWin();
    }
    else if (field[clickedCellPos.x][clickedCellPos.y].getMineStatus()) {
        this->mineSound.play();
        this->gameOver();
    }
    else if (field[clickedCellPos.x][clickedCellPos.y].getOpenedStatus()) {
        this->openOpenedCell(clickedCellPos);
    }
    else {
        this->clickSound.play();
        this->open(clickedCellPos);
        this->checkWin();
    }
}

void Game::FlagCell(sf::Vector2i mousePos) {
    if (!this->isMouseInField(mousePos)) return;

    sf::Vector2i clickedCellPos = this->getClickedPosition(mousePos);

    if (field[clickedCellPos.x][clickedCellPos.y].getOpenedStatus()) return;

    this->flagSound.play();

    if (field[clickedCellPos.x][clickedCellPos.y].getFlaggedStatus()) {
        this->field[clickedCellPos.x][clickedCellPos.y].unFlag();
        this->fieldSprites[clickedCellPos.x][clickedCellPos.y].setTexture(this->cellTexture);
        this->flagsAmount--;
    }
    else {
        this->field[clickedCellPos.x][clickedCellPos.y].Flag();
        this->fieldSprites[clickedCellPos.x][clickedCellPos.y].setTexture(this->flagTexture);
        this->flagsAmount++;
    }

    this->updateInfoText();
    this->HighlightCell(mousePos);
}

void Game::openAll() {
    for (int i = 0; i < this->fieldSize.x; i++) {
        for (int j = 0; j < this->fieldSize.y; j++) {
            this->open(sf::Vector2i(i, j));
        }
    }
}

void Game::updateTextOrigin() {
    auto bounds = endGameText.getLocalBounds();

    endGameText.setOrigin({
        std::round(bounds.position.x + bounds.size.x / 2.f),
        std::round(bounds.position.y + bounds.size.y / 2.f)
    });
}

void Game::gameOver() {
    this->isGameOver = true;

    this->setEndTime();

    this->updateInfoText();

    this->endGameText.setString(U"Проиграл");
    this->updateTextOrigin();

    this->openAll();
}

void Game::checkWin() {
    unsigned int flaggedMineAmount = 0;
    unsigned int openedCellsAmount = 0;

    for (const auto& line : this->field) {
        for (const auto& cell : line) {
            openedCellsAmount += cell.getOpenedStatus();
            flaggedMineAmount += (cell.getFlaggedStatus() && cell.getMineStatus());
        }
    }

    if (openedCellsAmount == fieldSize.x * fieldSize.y - mineAmount ||
        flaggedMineAmount == mineAmount) {
            this->winGame();
    }
}

void Game::winGame() {
    this->isWin = true;

    this->setEndTime();

    this->updateInfoText();

    this->endGameText.setString(U"Выиграл");
    this->updateTextOrigin();

    this->openAll();
}

void Game::setFieldWindowPosition(const sf::Vector2u position) {
    if (position.x < 0 || position.x >= this->window.getSize().x ||
        position.y < 0 || position.y >= this->window.getSize().y)
        return;

    this->fieldWindowPosition = position;
    this->updateWindowSize();
}

void Game::setFieldWindowSize(sf::Vector2u size) {
    if (size.x + this->fieldWindowPosition.x > this->window.getSize().x ||
        size.y + this->fieldWindowPosition.y > this->window.getSize().y)
    {
        return;
    }

    this->fieldWindowSize = size;
    this->updateWindowSize();
}

void Game::updateWindowSize()
{
    if (fieldSize.x == 0 || fieldSize.y == 0)
        return;

    const auto windowSize = window.getSize();

    if (windowSize.x == 0 || windowSize.y == 0)
        return;

    window.setView(sf::View(sf::FloatRect(
        {0.f, 0.f},
        {
            static_cast<float>(windowSize.x),
            static_cast<float>(windowSize.y)
        }
    )));

    const float minMenuHeight = 60.f;
    const float maxMenuHeight = 130.f;

    float menuHeight = std::clamp(
        static_cast<float>(windowSize.y) * 0.12f,
        minMenuHeight,
        maxMenuHeight
    );

    float availableWidth = static_cast<float>(windowSize.x);
    float availableHeight =
        static_cast<float>(windowSize.y) - menuHeight;

    float cellSize = std::min(
        availableWidth / static_cast<float>(fieldSize.x),
        availableHeight / static_cast<float>(fieldSize.y)
    );

    if (cellSize <= 0.f)
        return;

    float fieldWidth =
        cellSize * static_cast<float>(fieldSize.x);

    float fieldHeight =
        cellSize * static_cast<float>(fieldSize.y);

    float startX =
        (static_cast<float>(windowSize.x) - fieldWidth) / 2.f;

    float startY =
        menuHeight +
        (availableHeight - fieldHeight) / 2.f;

    targetCellSize = {cellSize, cellSize};

    fieldWindowPosition = {
        static_cast<unsigned int>(startX),
        static_cast<unsigned int>(startY)
    };

    fieldWindowSize = {
        static_cast<unsigned int>(fieldWidth),
        static_cast<unsigned int>(fieldHeight)
    };

    for (unsigned int x = 0; x < fieldSize.x; ++x) {
        for (unsigned int y = 0; y < fieldSize.y; ++y) {
            auto& sprite = fieldSprites[x][y];
            const auto bounds = sprite.getLocalBounds();

            sprite.setScale({
                cellSize / bounds.size.x,
                cellSize / bounds.size.y
            });

            sprite.setPosition({
                startX + static_cast<float>(x) * cellSize,
                startY + static_cast<float>(y) * cellSize
            });
        }
    }

    restartButton.setAction([this]() {
        restart();
    });

    restartButton.setTextures(
        "Sprites/Restart Button/Restart Button.png",
        "Sprites/Restart Button/Aimed Restart Button.png",
        "Sprites/Restart Button/Pressed Restart Button.png"
    );

    homeButton.setTextures(
        "Sprites/Home Button/Home.png",
        "Sprites/Home Button/Aimed Home.png",
        "Sprites/Home Button/Pressed Home.png"
    );

    const float buttonSize = menuHeight - 10.f;

    restartButton.setSize({
        buttonSize,
        buttonSize
    });

    restartButton.setPosition({
        static_cast<float>(windowSize.x) - buttonSize - 5.f,
        5.f
    });

    homeButton.setSize({
        buttonSize,
        buttonSize
    });

    homeButton.centerOriginX();

    homeButton.setPosition({
        static_cast<float>(windowSize.x) / 2.f,
        5.f
    });

    updateInfoText();

    endGameText.setPosition({
        static_cast<float>(windowSize.x) / 2.f,
        static_cast<float>(windowSize.y) / 2.f
    });

    updateTextOrigin();
}

void Game::HighlightCell(sf::Vector2i mousePos) {
    if (!this->isMouseInField(mousePos)) {
        if (highlightedCellPosition.x >= 0 && highlightedCellPosition.y >= 0) {
            if (!field[highlightedCellPosition.x][highlightedCellPosition.y].getOpenedStatus()) {
                if (field[highlightedCellPosition.x][highlightedCellPosition.y].getFlaggedStatus())
                    fieldSprites[highlightedCellPosition.x][highlightedCellPosition.y].setTexture(this->flagTexture);
                else
                    fieldSprites[highlightedCellPosition.x][highlightedCellPosition.y].setTexture(this->cellTexture);
            }
            highlightedCellPosition = sf::Vector2i(-1, -1);
        }
        return;
    }

    sf::Vector2i currentCellPos = this->getClickedPosition(mousePos);

    if (currentCellPos == highlightedCellPosition) {
        return;
    }

    if (highlightedCellPosition.x >= 0 && highlightedCellPosition.y >= 0) {
        if (!field[highlightedCellPosition.x][highlightedCellPosition.y].getOpenedStatus()) {
            if (field[highlightedCellPosition.x][highlightedCellPosition.y].getFlaggedStatus())
                fieldSprites[highlightedCellPosition.x][highlightedCellPosition.y].setTexture(this->flagTexture);
            else
                fieldSprites[highlightedCellPosition.x][highlightedCellPosition.y].setTexture(this->cellTexture);
        }
    }

    if (!field[currentCellPos.x][currentCellPos.y].getOpenedStatus()) {
        if (field[currentCellPos.x][currentCellPos.y].getFlaggedStatus())
            fieldSprites[currentCellPos.x][currentCellPos.y].setTexture(this->highlightFlagTexture);
        else
            fieldSprites[currentCellPos.x][currentCellPos.y].setTexture(this->highlightTexture);

        this->hoverSound.play();
    }

    highlightedCellPosition = currentCellPos;
}


void Game::UpdateUI(sf::Vector2i mousePos) {
    this->restartButton.update(this->window, mousePos);
    this->homeButton.update(this->window, mousePos);
}

void Game::restart() {
    // 1. Очищаем старые анимации (если игрок нажал рестарт прямо во время открытия)
    this->activeAnimations.clear();

    // 2. Сброс счетчика флагов
    this->flagsAmount = 0;
    this->wasFirstMove = false;

    // 3. Запускаем красивую волну закрытия для уже открытых или помеченных ячеек
    for (int x = 0; x < this->fieldSize.x; x++) {
        for (int y = 0; y < this->fieldSize.y; y++) {
            // Если текстура ячейки отличается от стандартной закрытой - переворачиваем её
            if (&this->fieldSprites[x][y].getTexture() != &this->cellTexture) {
                // Формируем задержку на основе координат (создаст волну по диагонали сверху-вниз)
                float waveDelay = (x + y) * 0.02f;
                this->activeAnimations.push_back({sf::Vector2i(x, y), 0.0f, &this->cellTexture, false, waveDelay});
            }
        }
    }

    // 4. Мгновенно обнуляем внутреннюю логику поля
    this->field.clear();
    this->field.resize(this->fieldSize.x);
    for (auto& line : this->field) {
        line.resize(this->fieldSize.y, Cell());
    }

    this->isGameOver = false;
    this->isWin = false;

    // 5. Обновляем таймер и текстовую информацию
    this->startTime = std::chrono::steady_clock::now();
    this->updateInfoText();
}

void Game::setTargetCellSize(unsigned int size)
{
    if (size == 0)
        return;

    desiredCellSize = size;
    updateWindowSize();
}

void Game::setGamePrefences(sf::Vector2u fieldSize, unsigned int mineAmount) {
    this->setFieldSize(fieldSize);
    this->setMineAmount(mineAmount);

    this->unitilizeField({-1, -1});
}