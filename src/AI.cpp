//
// Created by Glenn Smith on 5/7/17.
//

#include <random>
#include <unordered_map>
#include <sstream>
#include "AI.h"
#include "Screen.h"

void AI::updateMyRegions() {
	mRegions.clear();
	for (const auto &region : mBoard.getRegions()) {
		if (region.owner == mPlayer) {
			mRegions.push_back(region.index);
		}
	}
}

bool AI::getMove(int &move) {
	updateMyRegions();

	//Get a list of all the moves we can make and weight them
	float weightTotal = 0.0f;
	std::vector<std::pair<int, float>> moveList;
	for (const auto &region : mBoard.getRegions()) {
		//Can't move here
		if (region.owner != mPlayer && region.owner != Region::UNOWNED) {
			continue;
		}

		float weight = getMoveWeight(region.index);
		moveList.push_back({region.index, weight});
		weightTotal += weight;

		//Debug printing output
		std::vector<Point> points;
		mBoard.getRegionPoints(region.index, points);

		std::stringstream ss;
		ss << '(' << points[0].x << ", " << points[0].y << ')';
		ss << ' ' << weight;
		Screen::getScreen()->printAt(ss.str(), {mBoard.getExtent().x * 4 + 2, region.index});
	}
	if (weightTotal == 0.0f) {
		//Oh shit
		Screen::getScreen()->printAt("Shit", {mBoard.getExtent().x * 4 + 2, 0});
	}
	//Sort highest to lowest
	std::sort(moveList.begin(), moveList.end(), [](const auto &a, const auto &b)->bool{
		return a.second > b.second;
	});
	//Use the highest
	move = moveList[0].first;
	return true;
}

bool AI::isWinningMove(int move) {
	//Try all of our regions
	Board copy(mBoard);
	copy.move(move, mPlayer);

	int winner;
	return copy.getWinner(winner) && winner == mPlayer;
}

bool AI::isLosingMove(int move) {
	//If this move loses us the game on the next turn
	Board oneAdvance(mBoard);
	oneAdvance.move(move, mPlayer);

	//See all the moves the other player could make and if any would lose us the game
	for (const auto &region : mBoard.getRegions()) {
		//Ignore us as that would mean we move twice
		if (region.owner == mPlayer) {
			continue;
		}
		Board twoAdvance(oneAdvance);
		twoAdvance.move(region.index, region.owner);

		int winner;
		if (twoAdvance.getWinner(winner)) {
			//Don't move here at all if possible
			return true;
		}
	}
	return false;
}

//Get next move's weight. Chance of the move is weight / sum(all move weights)
float AI::getMoveWeight(int move) {
	if (isWinningMove(move)) {
		//Basically as big as possible
		return 1000.f;
	}
	if (isLosingMove(move)) {
		//As small as possible
		return 0.0f;
	}

	//Some simple factors for move weight:
	// - Bringing an owned tile closer to exploding
	// - If this tile is full:
	//    - Less valuable, don't just cause lots of explosions
	//    - Value increases by how many of the other player's tiles it can take
	//    - If any surrounding other player tiles are also full then very valuable
	// - If the other player can claim this tile on the next move, it's less valuable

	float weight;
	const Region &region = mBoard.getRegion(move);
	int max = mBoard.getRegionMax(region.index);
	if (region.fill == max) {
		//How many we could claim if we moved here
		int movedClaimed = getPlayerClaimedCount(mBoard, move, mPlayer);
		int currentClaimed = (int)mRegions.size();

		weight = (float)(movedClaimed - currentClaimed) / (float)max;
		//Also take into account the number of surrounding full regions they have
		for (const auto &neighbor : mBoard.getRegionNeighbors(region.index)) {
			const auto &other = mBoard.getRegion(neighbor);
			if (other.owner == mPlayer) {
				continue;
			}
			//We could explode an opponent's tile if we pick this; it's a very good choice
			if (other.fill == mBoard.getRegionMax(other.index)) {
				weight += 0.5f;
			}
		}
	} else if (region.owner == Region::UNOWNED) {
		//Unclaimed tile, it's worth more if it has fewer neighbors.
		// It's also worth more if its neighbors are not ours (uncertain how helpful this
		// strategy is but it makes nicer patterns)

		//Implicit weight to getting new tiles, this also helps the neighbor count weight
		// be sorted better.
		weight = 2;

		//How many neighbors are controlled by our opponent
		int opposing = 0;
		for (const auto &neighbor : mBoard.getRegionNeighbors(region.index)) {
			if (mBoard.getRegion(neighbor).owner != mPlayer && mBoard.getRegion(neighbor).owner != Region::UNOWNED) {
				opposing ++;
			}
		}
		//Has some effect on weight, but not that much
		weight += ((float)opposing / max) * 0.5f;
		//Biggest factor: the number of neighboring regions
		weight /= max;
	} else {
		//Our tile but nothing special about it. Try to fill it up if it's close to full
		weight = (float)region.fill / (float)max;
		//TODO: Less important if we're not next to any of the opponent's tiles
		//TODO: Much less important if we're next to a full tile of ours
		//TODO: Better prediction?
	}

	//TODO: This is entirely stateless. Theoretically we could call this again as the
	// other player and see what their best move would be. Then minimizing the weight of
	// their best move would be our best move. Uncertain how successful that would be.

	//If moving here would let the other player claim it on the next turn, less value
	{
		int opponentClaimed = 0;
		int movedMaxOpponentClaimed = 0;

		Board oneAdvance(mBoard);
		oneAdvance.move(move, mPlayer);
		//See if they would be able to claim it
		for (const auto &testRegion : oneAdvance.getRegions()) {
			//Can't move twice in a row
			if (testRegion.owner == mPlayer) {
				continue;
			}
			opponentClaimed ++;

			Board twoAdvance(oneAdvance);
			twoAdvance.move(testRegion.index, testRegion.owner);
			if (twoAdvance.getRegion(move).owner != mPlayer) {
				//Opponent can reclaim this, less valuable
				weight *= 0.25f;
			}

			//See how many our opponent could own after this move
			int movedOpponentClaimed = 0;
			for (const auto &twoTestRegion : twoAdvance.getRegions()) {
				if (twoTestRegion.owner != mPlayer) {
					movedOpponentClaimed ++;
				}
			}
			//If this is their best move then update that
			if (movedOpponentClaimed > movedMaxOpponentClaimed) {
				movedMaxOpponentClaimed = movedOpponentClaimed;
			}
		}

		//And weight down the move if our opponent can gain more points from it
		weight /= 1.0f + ((float)(movedMaxOpponentClaimed - opponentClaimed) / (float)mBoard.getRegions().size());
	}

	return weight;
}

int AI::getPlayerClaimedCount(const Board &input, int move, int player) {
	//How many can it claim if we move here
	int movedClaimed = 0;

	//Try the move and see what we get
	Board testBoard(input);
	testBoard.move(move, player);
	for (const auto &testRegion : testBoard.getRegions()) {
		if (testRegion.owner == player) {
			movedClaimed ++;
		}
	}
	return movedClaimed;
}
