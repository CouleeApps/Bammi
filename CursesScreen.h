//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_CURSESSCREEN_H
#define BAMMI_CURSESSCREEN_H

#include "Screen.h"

class CursesScreen : public Screen {
public:
	~CursesScreen();
	CursesScreen();
	void print(const Board &board) const override;
	void printAt(const std::string &text, const Point &position) const override;
	bool getMove(Point &move) const override;
	bool getBoardSize(Point &size) const override;
	void delay(const uint64_t &milliseconds) const override;
	void clear() const override;
};

#endif //BAMMI_CURSESSCREEN_H
