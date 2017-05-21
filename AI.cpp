//
// Created by Glenn Smith on 5/7/17.
//

#include <random>
#include <unordered_map>
#include <sstream>
#include "AI.h"
#include "Screen.h"

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

bool AI::getMove(int &move) {
	updateMyRegions();

	//If our next move could win us the game, do that of course
	if (winningMove(move)) {
		return true;
	}
	//Try to fill up empty spaces so we have more
	if (basicMove(5, move)) {
		return true;
	}
//	//If we can chain explosions, hey why not
//	if (obviousExplodeMove(move)) {
//		return true;
//	}

	//Fill up smaller regions

	//Can't find anything to do??
	return highestWeightedMove(move);
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

bool AI::obviousExplodeMove(int &move) {
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

bool AI::highestWeightedMove(int &move) {
	if (myRegions.size() == 0) {
		return false;
	}

	//Get a list of all the moves we can make and weight them
	float weightTotal = 0.0f;
	std::vector<std::pair<int, float>> moveList;
	for (const auto &region : myRegions) {
		float weight = getMoveWeight(region);

		moveList.push_back({region, weight});
		weightTotal += weight;

		std::stringstream ss;
		ss << '(' << board->regions[region].points[0].x << ", " << board->regions[region].points[0].y << ')';
		ss << ' ' << weight;
		Screen::getScreen()->printAt(ss.str(), {board->extent.x * 4 + 2, region});
	}
	if (weightTotal == 0.0f) {
		//Oh shit
		Screen::getScreen()->printAt("Shit", {board->extent.x * 4 + 2, 0});
	}
	//Sort highest to lowest
	std::sort(moveList.begin(), moveList.end(), [](const auto &a, const auto &b)->bool{
		return a.second > b.second;
	});
	//Use the highest
	move = moveList[0].first;
	return true;

	//Pick a random place to go
	std::random_device rd;
	std::uniform_real_distribution<float> dist(0, weightTotal);

	float weightedMove = dist(rd);
	for (const auto &pair : moveList) {
		if (weightedMove > pair.second) {
			weightedMove -= pair.second;
			continue;
		}
		move = pair.first;
		return true;
	}

	//Couldn't pick anything?
	return false;
}

//Get next move's weight. Chance of the move is weight / sum(all move weights)
float AI::getMoveWeight(int move) {
	//Some quick things that make moves less desirable:
	{
		//If this move loses us the game on the next turn
		Board oneAdvance(*board);
		oneAdvance.move(move, player);

		//See all the moves the other player could make and if any would lose us the game
		for (const auto &region : board->regions) {
			//Ignore us as that would mean we move twice
			if (region.owner == player) {
				continue;
			}
			Board twoAdvance(oneAdvance);
			twoAdvance.move(region.index, region.owner);

			int winner;
			if (twoAdvance.getWinner(winner)) {
				//Don't move here at all if possible
				return 0.0f;
			}
		}
	}

	//Some simple factors for move weight:
	// - Bringing an owned tile closer to exploding
	// - If this tile is full:
	//    - Less valuable, don't just cause lots of explosions
	//    - Value increases by how many of the other player's tiles it can take
	//    - If any surrounding other player tiles are also full then very valuable
	// - If the other player can claim this tile on the next move, it's less valuable

	float weight;
	const Region &region = board->regions[move];
	if (region.fill == region.size()) {
		int movedClaimed = 0;
		int currentClaimed = (int)myRegions.size();
		//How many can it claim if we move here
		{
			Board testBoard(*board);
			testBoard.move(move, player);
			for (const auto &testRegion : testBoard.regions) {
				if (testRegion.owner == player) {
					movedClaimed ++;
				}
			}
		}

		weight = (float)(movedClaimed - currentClaimed) / (float)region.size();
	} else {
		weight = (float)region.fill / (float)region.size();
	}

	//If moving here would let the other player claim it on the next turn, less value
	{
		int opponentClaimed = 0;
		int movedMaxOpponentClaimed = 0;

		Board oneAdvance(*board);
		oneAdvance.move(move, player);
		//See if they would be able to claim it
		for (const auto &testRegion : oneAdvance.regions) {
			//Can't move twice in a row
			if (testRegion.owner == player) {
				continue;
			}
			opponentClaimed ++;

			Board twoAdvance(oneAdvance);
			twoAdvance.move(testRegion.index, testRegion.owner);
			if (twoAdvance.regions[move].owner != player) {
				//Opponent can reclaim this, less valuable
				weight *= 0.25f;
			}

			//See how many our opponent could own after this move
			int movedOpponentClaimed = 0;
			for (const auto &twoTestRegion : twoAdvance.regions) {
				if (twoTestRegion.owner != player) {
					movedOpponentClaimed ++;
				}
			}
			//If this is their best move then update that
			if (movedOpponentClaimed > movedMaxOpponentClaimed) {
				movedMaxOpponentClaimed = movedOpponentClaimed;
			}
		}

		//And weight down the move if our opponent can gain more points from it
		weight /= 1.0f + ((float)(movedMaxOpponentClaimed - opponentClaimed) / (float)board->regions.size());
	}

	return weight;
}
