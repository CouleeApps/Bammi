//
// Created by Glenn Smith on 5/7/17.
//

#include "AI.h"

void AI::updateMyRegions() {
	myRegions.clear();
	for (int i = 0; i < board->regions.size(); i ++) {
		if (board->regions[i].owner == player) {
			myRegions.push_back(i);
		}
	}
	std::sort(myRegions.begin(), myRegions.end(), [this](const int &a, const int &b)->bool{
		if (board->regions[a].size() != board->regions[b].size()) {
			return board->regions[a].size() < board->regions[b].size();
		}
		return board->regions[a].points.size() > board->regions[b].points.size();
	});
}

bool AI::basicMove(int maxSize, int &move) {
	//Find the first region with the fewest neighbors
	std::vector<int> sortedRegions;
	for (int i = 0; i < board->regions.size(); i ++) {
		if (board->regions[i].owner == -1 && board->regions[i].size() <= maxSize) {
			sortedRegions.push_back(i);
		}
	}
	std::sort(sortedRegions.begin(), sortedRegions.end(), [this](const int &a, const int &b)->bool{
		if (board->regions[a].size() != board->regions[b].size()) {
			return board->regions[a].size() < board->regions[b].size();
		}
		return board->regions[a].points.size() > board->regions[b].points.size();
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
	updateMyRegions();

	//If our next move could win us the game, do that of course
	if (winningMove(move)) {
		return true;
	}
	//If we can chain explosions, hey why not
	if (obviousExplode(move)) {
		return true;
	}
	//Try to fill up empty spaces so we have more
	if (basicMove(5, move)) {
		return true;
	}

	//Fill up smaller regions


	return true;
}

bool AI::obviousExplode(int &move) {
	//Check if any regions bounding one of our full regions is also full
	// If so, explode our region for super chain reaction party
	for (auto region : myRegions) {
		Region &myRegion = board->regions[region];
		//Not full
		if (myRegion.fill != myRegion.size())
			continue;

		for (auto neighbor : board->regions[region].neighbors) {
			Region &neighborRegion = board->regions[neighbor];
			if (neighborRegion.fill == neighborRegion.size()) {
				//Yep it is, let's goooo
				move = region;
				return true;
			}
		}
	}

	return false;
}

bool AI::winningMove(int &move) {
	//Try all of our regions
	for (auto region : myRegions) {
		Board copy(*board);
		copy.move(region, player);

		int winner;
		if (copy.getWinner(winner) && winner == player) {
			move = region;
			return true;
		}
	}

	return false;
}
