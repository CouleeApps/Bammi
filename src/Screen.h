//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_SCREEN_H
#define BAMMI_SCREEN_H

#include "Board.h"

class Screen {
public:
	virtual ~Screen() {}
	virtual void print(const Board &board) const = 0;
	virtual void printAt(const std::string &text, const Point &position) const = 0;
	virtual bool getMove(Point &move) const = 0;
	virtual bool getBoardSize(Point &size) const = 0;
	virtual void delay(const uint64_t &milliseconds) const = 0;
	virtual void clear() const = 0;

	static Screen *getScreen();
};

#endif //BAMMI_SCREEN_H
