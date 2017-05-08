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
	int player;

	void updateMyRegions();
	bool basicMove(int maxSize, int &move);
	bool obviousExplode(int &move);
	bool winningMove(int &move);

public:
	AI(Board *board, int player) : board(board), player(player) {}
	bool getMove(int &move);
};


#endif //BAMMI_AI_H
