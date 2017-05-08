//
// Created by Glenn Smith on 5/7/17.
//

#include "AI.h"

bool AI::basicMove(int maxSize, int &move) {
	//Find the first region with the fewest neighbors
	std::vector<int> sortedRegions;
	for (int i = 0; i < board->regions.size(); i ++) {
		if (board->regions[i].owner == Region::Nobody && board->regions[i].size() <= maxSize) {
			sortedRegions.push_back(i);
		}
	}
	std::sort(sortedRegions.begin(), sortedRegions.end(), [this](const int &a, const int &b)->bool{
		return board->regions[a].size() < board->regions[b].size();
	});

	//Nothing free, do something more complicated
	if (sortedRegions.size() == 0) {
		return false;
	}

	move = sortedRegions[0];
	myRegions.push_back(move);

	return true;
}

bool AI::getMove(int &move) {
	if (basicMove(5, move)) {
		return true;
	}

	return true;
}
