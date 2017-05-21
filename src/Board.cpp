//
// Created by Glenn Smith on 5/7/17.
//

#include "Board.h"
#include <random>
#include <algorithm> //std::find

Board::Board(const Layout &layout) : mLayout(layout) {
	for (int i = 0; i < mLayout.getRegionCount(); i ++) {
		mRegions.push_back(Region(i));
	}
}

bool Board::move(const Point &point, int player) {
	if (point.x < 0) return false;
	if (point.y < 0) return false;
	if (point.x >= getExtent().x) return false;
	if (point.y >= getExtent().y) return false;
	int index = getIndex(point);
	return move(index, player);
}

bool Board::move(int index, int player) {
	mLastMove.clear();

	if (!fillSlice(index, player)) {
		return false;
	}
	mRegions[index].owner = player;

	//Explode them in the order they are added so we don't recurse
	for (auto it = mExplodeRegions.begin(); it != mExplodeRegions.end(); ) {
		//Remove from exploded region
		Region &region = mRegions[*it];
		region.fill -= getRegionMax(region.index);
		//And fill each neighbor with one (optionally exploding those too)
		const auto &neighbors = getRegionNeighbors(region.index);
		for (auto &neighborIndex : neighbors) {
			//We control this now
			mRegions[neighborIndex].owner = player;
			if (!fillSlice(neighborIndex, player)) {
				return false;
			}
		}
		//Advance
		it = mExplodeRegions.erase(it);

		//Has someone won?
		int winner;
		if (getWinner(winner)) {
			return true;
		}

		//Keep going!
		if (region.fill > getRegionMax(region.index)) {
			//Split!
			mExplodeRegions.push_back(region.index);
		}
	}
	return true;
}

bool Board::fillSlice(int index, int player) {
	Region &region = mRegions[index];
	if (region.owner != player && region.owner != Region::UNOWNED) {
		return false;
	}
	region.fill ++;
	if (region.fill > getRegionMax(region.index) &&
	    std::find(mExplodeRegions.begin(), mExplodeRegions.end(), region.index) == mExplodeRegions.end()) {
		//Split!
		mExplodeRegions.push_back(region.index);
	}
	mLastMove.push_back(index);
	return true;
}

bool Board::getWinner(int &winner) const {
	//See if this wins us the game
	winner = -1;
	bool winning = false;
	for (const auto &copyRegion : mRegions) {
		//Not everything is covered
		if (copyRegion.owner == Region::UNOWNED) {
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

bool Board::isLastMove(const Point &a) const {
	return std::find(mLastMove.begin(), mLastMove.end(), mLayout.getIndex(a)) != mLastMove.end();
}

int Board::getIndex(const Point &p) const {
	return mLayout.getIndex(p);
}

const Region &Board::getRegion(int index) const {
	return mRegions[index];
}

const Region &Board::getRegion(const Point &p) const {
	return mRegions[getIndex(p)];
}

const std::vector<Region> &Board::getRegions() const {
	return mRegions;
}

Point Board::getExtent() const {
	return mLayout.getExtent();
}

void Board::getRegionPoints(int index, std::vector<Point> &points) const {
	mLayout.getRegionPoints(index, points);
}

const std::vector<int> &Board::getRegionNeighbors(int index) const {
	return mLayout.getRegionNeighbors(index);
}

int Board::getRegionMax(int index) const {
	return mLayout.getRegionMax(index);
}

bool Board::isEdge(const Point &a, const Point &b) const {
	return mLayout.isEdge(a, b);
}


//---------------------------------------------------------------------------------------


Board::Layout::Layout(const Point &extent) : mExtent(extent) {
	mIndices = std::vector<std::vector<int>>(static_cast<size_t>(extent.x), std::vector<int>(static_cast<size_t>(extent.y), UNASSIGNED));
	assignRegions();
	findNeighbors();
	findEdges();
}

bool Board::Layout::randomEmptyCell(Point &point) {
	//Get a list of all the empties
	std::vector<Point> emptyPoints;
	for (size_t x = 0; x < mIndices.size(); x ++) {
		auto &col = mIndices[x];
		for (size_t y = 0; y < col.size(); y ++) {
			auto &cell = col[y];
			if (cell == UNASSIGNED) {
				emptyPoints.push_back(Point(static_cast<int>(x), static_cast<int>(y)));
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

bool Board::Layout::newRegion(int index) {
	Point start;
	//Make sure we have somewhere to start
	if (!randomEmptyCell(start)) {
		return false;
	}

	std::vector<Point> regionPoints;

	//Mark this point as taken by this region
	mIndices[start.x][start.y] = index;
	regionPoints.push_back(start);

	//Spread a bit
	std::random_device rd;
	std::bernoulli_distribution bdist(0.7);
	Point next = start;
	while (bdist(rd)) {
		std::vector<Point> emptyPoints;
		//See which points we can spread to
		if (next.x > 0               && mIndices[next.x - 1][next.y] == UNASSIGNED) emptyPoints.push_back(Point(next.x - 1, next.y));
		if (next.x < (mExtent.x - 1) && mIndices[next.x + 1][next.y] == UNASSIGNED) emptyPoints.push_back(Point(next.x + 1, next.y));
		if (next.y > 0               && mIndices[next.x][next.y - 1] == UNASSIGNED) emptyPoints.push_back(Point(next.x, next.y - 1));
		if (next.y < (mExtent.y - 1) && mIndices[next.x][next.y + 1] == UNASSIGNED) emptyPoints.push_back(Point(next.x, next.y + 1));
		//Nothing? How sad.
		if (emptyPoints.size() == 0) break;

		//Add a random one
		std::uniform_int_distribution<int> dist(0, static_cast<int>(emptyPoints.size() - 1));
		next = emptyPoints[dist(rd)];

		//Mark this point as taken by this region
		mIndices[next.x][next.y] = index;
		regionPoints.push_back(next);
	}

	return true;
}

void Board::Layout::assignRegions() {
	//Make new regions until we run out of points
	for (int index = 0; newRegion(index); index ++) {
		mRegionCount ++;
	}
}

void Board::Layout::findNeighbors() {
	for (int i = 0; i < mRegionCount; i ++) {
		//So we don't have duplicates
		std::set<int> neighborSet;

		std::vector<Point> points;
		getRegionPoints(i, points);

		for (const auto &point : points) {
			//Up to four, check if they're a different region
			if (point.x > 0              && mIndices[point.x - 1][point.y] != i) neighborSet.insert(mIndices[point.x - 1][point.y]);
			if (point.x < (mExtent.x - 1) && mIndices[point.x + 1][point.y] != i) neighborSet.insert(mIndices[point.x + 1][point.y]);
			if (point.y > 0              && mIndices[point.x][point.y - 1] != i) neighborSet.insert(mIndices[point.x][point.y - 1]);
			if (point.y < (mExtent.y - 1) && mIndices[point.x][point.y + 1] != i) neighborSet.insert(mIndices[point.x][point.y + 1]);
		}

		std::vector<int> neighbors;
		for (const auto &neighbor : neighborSet) {
			neighbors.push_back(neighbor);
		}
		mNeighbors.push_back(neighbors);
	}
}

void Board::Layout::findEdges() {
	std::set<std::pair<Point, Point>> foundEdges;

	for (int x = 0; x < mExtent.x; x ++) {
		for (int y = 0; y < mExtent.y; y ++) {
			int at = mIndices[x][y];
			//Edge of the board
			if (x == 0) foundEdges.insert({Point(-1, y), Point(x, y)});
				//Or the tile to the left is a different region
			else if (mIndices[x - 1][y] != at) foundEdges.insert({Point(x - 1, y), Point(x, y)});

			//Same thing x4
			if (y == 0) foundEdges.insert({Point(x, -1), Point(x, y)});
			else if (mIndices[x][y - 1] != at) foundEdges.insert({Point(x, y - 1), Point(x, y)});
			if (x == mExtent.x - 1) foundEdges.insert({Point(x, y), Point(mExtent.x, y)});
			else if (mIndices[x + 1][y] != at) foundEdges.insert({Point(x + 1, y), Point(x, y)});
			if (y == mExtent.y - 1) foundEdges.insert({Point(x, y), Point(x, mExtent.y)});
			else if (mIndices[x][y + 1] != at) foundEdges.insert({Point(x, y + 1), Point(x, y)});
		}
	}

	for (const auto &pair : foundEdges) {
		mEdges[pair.first].push_back(pair.second);
		mEdges[pair.second].push_back(pair.first);
	}
}

void Board::Layout::getRegionPoints(int index, std::vector<Point> &points) const {
	for (int x = 0; x < mExtent.x; x ++) {
		for (int y = 0; y < mExtent.y; y ++) {
			if (mIndices[x][y] == index) {
				points.push_back({x, y});
			}
		}
	}
}

const std::vector<int> &Board::Layout::getRegionNeighbors(int index) const {
	return mNeighbors[index];
}

int Board::Layout::getRegionMax(int index) const {
	return (int)mNeighbors[index].size();
}

bool Board::Layout::isEdge(const Point &a, const Point &b) const {
	auto it = mEdges.find(a);
	if (it == mEdges.end()) {
		return false;
	}
	return std::find(it->second.cbegin(), it->second.cend(), b) != it->second.cend();
}

int Board::Layout::getIndex(const Point &point) const {
	return mIndices[point.x][point.y];
}

const Point &Board::Layout::getExtent() const {
	return mExtent;
}

int Board::Layout::getRegionCount() const {
	return mRegionCount;
}
