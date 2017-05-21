//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_BOARD_H
#define BAMMI_BOARD_H

#include <vector>
#include <set>
#include <deque>

struct Point {
	int x;
	int y;
	Point() : x(0), y(0) {}
	Point(int x, int y) : x(x), y(y) {}

	bool operator<(const Point &other) const {
		return x < other.x || (x == other.x && y < other.y);
	}
	bool operator==(const Point &other) const {
		return x == other.x && y == other.y;
	}
};

struct Region {
	int index;
	int fill;
	int owner;

	Region(int index) : index(index), fill(0), owner(-1) {}
};

class Board {
	friend class AI;
	friend class CursesScreen;
	friend class CoutScreen;

	std::vector<std::vector<int>> mIndices;
	std::vector<std::vector<int>> mNeighbors;
	std::vector<std::pair<Point, Point>> mEdges;
	std::vector<Region> mRegions;
	std::deque<int> mExplodeRegions;
	Point mExtent;

	std::vector<int> mLastMove;

	bool randomEmptyCell(Point &point);
	bool newRegion(std::vector<Point> &regionPoints, int index);
	void assignRegions();
	void findNeighbors();
	void findEdges();

	bool fillSlice(int index, int player);
public:
	Board(const Point &extent);

	int &operator[](const Point &p) { return mIndices[p.x][p.y]; }
	const int &operator[](const Point &p) const { return mIndices[p.x][p.y]; }

	void getRegionPoints(int index, std::vector<Point> &points) const;
	const std::vector<int> &getRegionNeighbors(int index) const;
	int getRegionMax(int index) const;

	bool move(const Point &point, int player);
	bool move(int index, int player);

	bool getWinner(int &winner) const;
};


#endif //BAMMI_BOARD_H
