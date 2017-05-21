//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_COUTSCREEN_H
#define BAMMI_COUTSCREEN_H

#include "Screen.h"

class CoutScreen : public Screen {
public:
	~CoutScreen() {};
	CoutScreen();
	void print(const Board &board) const override;
	void printAt(const std::string &text, const Point &position) const override;
	bool getMove(Point &move) const override;
	bool getBoardSize(Point &size) const override;
	void delay(const uint64_t &milliseconds) const override;
	void clear() const override;
};

#endif //BAMMI_COUTSCREEN_H
