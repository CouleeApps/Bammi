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
};

struct Region {
	std::vector<Point> points;
	//Use a set so we don't have to worry about duplicates
	std::set<int> neighbors;
	int size() const { return static_cast<int>(neighbors.size()); }
	int index;
	int fill;
	enum Player {
		Nobody,
		Human,
		AI,
		MaxPlayers
	} owner;

	Region(const std::vector<Point> &points, int index) : points(points), index(index), fill(0), owner(Nobody) {}
};

class Board {
	friend class AI;
	friend class Screen;

	std::vector<std::vector<int>> indices;
	std::set<std::pair<Point, Point>> edges;
	std::vector<Region> regions;
	std::deque<int> explodeRegions;
	Point extent;

	bool randomEmptyCell(Point &point);
	bool newRegion(std::vector<Point> &regionPoints, int index);
	void assignRegions();
	void findNeighbors();
	void findEdges();

	bool fillSlice(int index, Region::Player player);
public:
	Board(const Point &extent);

	int &operator[](const Point &p) { return indices[p.x][p.y]; }
	const int &operator[](const Point &p) const { return indices[p.x][p.y]; }

	Region &getRegion(int index) { return regions[index]; }
	const Region &getRegion(int index) const { return regions[index]; }

	bool move(const Point &point, Region::Player player);
	bool move(int index, Region::Player player);
};


#endif //BAMMI_BOARD_H
