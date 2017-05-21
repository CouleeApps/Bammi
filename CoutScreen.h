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
	void printAt(const Point &position, const std::string &text) const override;
	bool getMove(Point &move) const override;
	void delay(const uint64_t &milliseconds) const override;
};

#endif //BAMMI_COUTSCREEN_H
