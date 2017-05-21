//
// Created by Glenn Smith on 5/7/17.
//

#ifndef BAMMI_BOARD_H
#define BAMMI_BOARD_H

#include <vector>
#include <set>
#include <deque>
#include <unordered_map>
#include <map>

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
public:
	/**
	 * Contains all the tile index/neighbor/edge structure for the game board.
	 * The Board has a constant reference to this, since this is only generated
	 * once (in the constructor) and never changed afterwards.
	 */
	class Layout {
		std::vector<std::vector<int>> mIndices;
		std::vector<std::vector<int>> mNeighbors;
		std::map<Point, std::vector<Point>> mEdges;
		Point mExtent;
		int mRegionCount;

		bool randomEmptyCell(Point &point);
		bool newRegion(int index);
		void assignRegions();
		void findNeighbors();
		void findEdges();

	public:
		int getIndex(const Point &point) const;
		void getRegionPoints(int index, std::vector<Point> &points) const;
		const std::vector<int> &getRegionNeighbors(int index) const;
		int getRegionMax(int index) const;
		bool isEdge(const Point &a, const Point &b) const;
		const Point &getExtent() const;
		int getRegionCount() const;

		Layout(const Point &extent);
	};

private:
	const Layout &mLayout;
	std::vector<Region> mRegions;
	std::vector<int> mLastMove;
	std::deque<int> mExplodeRegions;

	bool fillSlice(int index, int player);
public:
	Board(const Layout &layout);
	Point getExtent() const;

	const int getIndex(const Point &p) const;
	const Region &getRegion(int index) const;
	const Region &getRegion(const Point &p) const;
	const std::vector<Region> &getRegions() const;
	int getRegionMax(int index) const;
	void getRegionPoints(int index, std::vector<Point> &points) const;
	const std::vector<int> &getRegionNeighbors(int index) const;

	bool isEdge(const Point &a, const Point &b) const;
	bool isLastMove(const Point &a) const;

	bool move(const Point &point, int player);
	bool move(int index, int player);

	bool getWinner(int &winner) const;
};

#endif //BAMMI_BOARD_H
