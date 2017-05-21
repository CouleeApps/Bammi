//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_AI_H
#define BAMMI_AI_H

#include <vector>
#include "Board.h"

class AI {
	Board *mBoard;
	std::vector<int> mRegions;
	int mPlayer;

	void updateMyRegions();
	bool isWinningMove(int move);
	bool isLosingMove(int move);
	float getMoveWeight(int move);
	int getPlayerClaimedCount(const Board &input, int move, int player);

public:
	AI(Board *board, int player) : mBoard(board), mPlayer(player) {}
	bool getMove(int &move);
};


#endif //BAMMI_AI_H
