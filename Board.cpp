//
// Created by Glenn Smith on 5/7/17.
//

#include "Board.h"
#include <random>

Board::Board(const Point &extent) : extent(extent) {
	indices = std::vector<std::vector<int>>(static_cast<size_t>(extent.x), std::vector<int>(static_cast<size_t>(extent.y), -1));
	assignRegions();
	findNeighbors();
	findEdges();
}

bool Board::randomEmptyCell(Point &point) {
	//Get a list of all the empties
	std::vector<Point> emptyPoints;
	for (int x = 0; x < indices.size(); x ++) {
		auto &col = indices[x];
		for (int y = 0; y < col.size(); y ++) {
			auto &cell = col[y];
			if (cell == -1) {
				emptyPoints.push_back(Point(x, y));
			}
		}
	}

	//Out of empty cells-- guess we're done generating
	if (emptyPoints.size() == 0) {
		return false;
	}

	//Choose a random point
	std::random_device rd;
	std::uniform_int_distribution<int> dist(0, static_cast<int>(emptyPoints.size() - 1));
	point = emptyPoints[dist(rd)];
	return true;
}

bool Board::newRegion(std::vector<Point> &regionPoints, int index) {
	Point start;
	//Make sure we have somewhere to start
	if (!randomEmptyCell(start)) {
		return false;
	}
	//Mark this point as taken by this region
	indices[start.x][start.y] = index;
	regionPoints.push_back(start);

	//Spread a bit
	std::random_device rd;
	std::bernoulli_distribution bdist(0.7);
	Point next = start;
	while (bdist(rd)) {
		std::vector<Point> emptyPoints;
		//See which points we can spread to
		if (next.x > 0              && indices[next.x - 1][next.y] == -1) emptyPoints.push_back(Point(next.x - 1, next.y));
		if (next.x < (extent.x - 1) && indices[next.x + 1][next.y] == -1) emptyPoints.push_back(Point(next.x + 1, next.y));
		if (next.y > 0              && indices[next.x][next.y - 1] == -1) emptyPoints.push_back(Point(next.x, next.y - 1));
		if (next.y < (extent.y - 1) && indices[next.x][next.y + 1] == -1) emptyPoints.push_back(Point(next.x, next.y + 1));
		//Nothing? How sad.
		if (emptyPoints.size() == 0) break;

		//Add a random one
		std::uniform_int_distribution<int> dist(0, static_cast<int>(emptyPoints.size() - 1));
		next = emptyPoints[dist(rd)];

		//Mark this point as taken by this region
		indices[next.x][next.y] = index;
		regionPoints.push_back(next);
	}

	return true;
}

void Board::assignRegions() {
	//Make new regions until we run out of points
	std::vector<Point> regionPoints;
	for (int index = 0; newRegion(regionPoints, index); index ++) {
		regions.push_back(Region(regionPoints, index));
		//So we have an empty vector for the next one
		regionPoints.clear();
	}
}

void Board::findNeighbors() {
	for (auto &region : regions) {
		for (const auto &point : region.points) {
			//Up to four, check if they're a different region
			if (point.x > 0              && indices[point.x - 1][point.y] != region.index) region.neighbors.insert(indices[point.x - 1][point.y]);
			if (point.x < (extent.x - 1) && indices[point.x + 1][point.y] != region.index) region.neighbors.insert(indices[point.x + 1][point.y]);
			if (point.y > 0              && indices[point.x][point.y - 1] != region.index) region.neighbors.insert(indices[point.x][point.y - 1]);
			if (point.y < (extent.y - 1) && indices[point.x][point.y + 1] != region.index) region.neighbors.insert(indices[point.x][point.y + 1]);
		}
	}
}

void Board::findEdges() {
	for (int x = 0; x < extent.x; x ++) {
		for (int y = 0; y < extent.y; y ++) {
			int at = indices[x][y];
			//Edge of the board
			if (x == 0) edges.insert({Point(-1, y), Point(x, y)});
				//Or the tile to the left is a different region
			else if (indices[x - 1][y] != at) edges.insert({Point(x - 1, y), Point(x, y)});

			//Same thing x4
			if (y == 0) edges.insert({Point(x, -1), Point(x, y)});
			else if (indices[x][y - 1] != at) edges.insert({Point(x, y - 1), Point(x, y)});
			if (x == extent.x - 1) edges.insert({Point(x, y), Point(extent.x, y)});
			else if (indices[x + 1][y] != at) edges.insert({Point(x + 1, y), Point(x, y)});
			if (y == extent.y - 1) edges.insert({Point(x, y), Point(x, extent.y)});
			else if (indices[x][y + 1] != at) edges.insert({Point(x, y + 1), Point(x, y)});
		}
	}
}

bool Board::move(const Point &point, int player) {
	if (point.x < 0) return false;
	if (point.y < 0) return false;
	if (point.x >= extent.x) return false;
	if (point.y >= extent.y) return false;
	int index = indices[point.x][point.y];
	return move(index, player);
}

bool Board::move(int index, int player) {
	lastMove.clear();

	if (!fillSlice(index, player)) {
		return false;
	}
	regions[index].owner = player;

	//Explode them in the order they are added so we don't recurse
	for (auto it = explodeRegions.begin(); it != explodeRegions.end(); ) {
		//Remove from exploded region
		Region &region = regions[*it];
		region.fill -= region.max();
		//And fill each neighbor with one (optionally exploding those too)
		for (auto &neighborIndex : region.neighbors) {
			//We control this now
			regions[neighborIndex].owner = player;
			if (!fillSlice(neighborIndex, player)) {
				return false;
			}
		}
		//Advance
		it = explodeRegions.erase(it);

		//Has someone won?
		int winner;
		if (getWinner(winner)) {
			return true;
		}

		//Keep going!
		if (region.fill > region.max()) {
			//Split!
			explodeRegions.push_back(region.index);
		}
	}
	return true;
}

bool Board::fillSlice(int index, int player) {
	Region &region = regions[index];
	if (region.owner != player && region.owner != -1) {
		return false;
	}
	region.fill ++;
	if (region.fill > region.max() &&
	    std::find(explodeRegions.begin(), explodeRegions.end(), region.index) == explodeRegions.end()) {
		//Split!
		explodeRegions.push_back(region.index);
	}
	lastMove.push_back(index);
	return true;
}

bool Board::getWinner(int &winner) const {
	//See if this wins us the game
	winner = -1;
	bool winning = false;
	for (const auto &copyRegion : regions) {
		//Not everything is covered
		if (copyRegion.owner == -1) {
			return false;
		}
		if (winner == -1) {
			winner = copyRegion.owner;
			winning = true;
		}
		//Nope, so stop looking
		if (copyRegion.owner != winner) {
			winning = false;
			break;
		}
	}

	return winning;
}
