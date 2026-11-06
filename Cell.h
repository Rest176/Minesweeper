#pragma once

#include <SFML/Graphics.hpp>

class Cell {
private:
    unsigned short minesNearBy = 0;

    bool isFlagged = false;
    bool isMine = false;
    bool isOpened = false;

public:
    void Open();
    void Flag();
    void unFlag();

    void makeMine();
    void addCount();

    [[nodiscard]] bool getMineStatus() const;
    [[nodiscard]] bool getOpenedStatus() const;
    [[nodiscard]] unsigned short getMineAmount() const;
    [[nodiscard]] unsigned short getFlaggedStatus() const;
    [[nodiscard]] bool isEmpty() const;
};