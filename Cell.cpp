#include "Cell.h"

void Cell::Open() {
    this->isOpened = true;
}

bool Cell::getMineStatus() const {
    return this->isMine;
}

bool Cell::getOpenedStatus() const {
    return this->isOpened;
}

void Cell::makeMine() {
    this->isMine = true;
}

unsigned short Cell::getMineAmount() const {
    return (this->isMine) ? -1 : this->minesNearBy;
}

void Cell::addCount() {
    this->minesNearBy++;
}

bool Cell::isEmpty() const {
    return !this->isMine && this->minesNearBy == 0;
}

void Cell::Flag() {
    this->isFlagged = true;
}

void Cell::unFlag() {
    this->isFlagged = false;
}

unsigned short Cell::getFlaggedStatus() const {
    return this->isFlagged;
}
