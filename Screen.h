//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_SCREEN_H
#define BAMMI_SCREEN_H

#include "Board.h"

class Screen {
public:
	Screen();
	void print(const Board &board) const;
	bool getMove(Point &move) const;
};


#endif //BAMMI_SCREEN_H
