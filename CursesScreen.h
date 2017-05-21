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
	bool getMove(Point &move) const override;
};

#endif //BAMMI_CURSESSCREEN_H
