//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_AI_H
#define BAMMI_AI_H

#include <vector>
#include "Board.h"

class AI {
	Board *board;
	std::vector<int> myRegions;

	bool basicMove(int maxSize, int &move);

public:
	AI(Board *board) : board(board) {}
	bool getMove(int &move);
};


#endif //BAMMI_AI_H
