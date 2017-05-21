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
	class Layout {
		friend class Board;

		std::vector<std::vector<int>> mIndices;
		std::vector<std::vector<int>> mNeighbors;
		std::map<Point, std::vector<Point>> mEdges;
		Point mExtent;
		int mRegionCount;

		bool randomEmptyCell(Point &point);
		bool newRegion(std::vector<Point> &regionPoints, int index);
		void assignRegions();
		void findNeighbors();
		void findEdges();
		void getRegionPoints(int index, std::vector<Point> &points) const;
		const std::vector<int> &getRegionNeighbors(int index) const;
		int getRegionMax(int index) const;
		bool isEdge(const Point &a, const Point &b) const;

	public:
		Layout(const Point &extent);
	};

private:
	Layout &mLayout;
	std::vector<Region> mRegions;
	std::vector<int> mLastMove;
	std::deque<int> mExplodeRegions;

	bool fillSlice(int index, int player);
public:
	Board(Layout &layout);

	const int getIndex(const Point &p) const { return mLayout.mIndices[p.x][p.y]; }
	const Region &getRegion(int index) const { return mRegions[index]; }
	const Region &getRegion(const Point &p) const { return mRegions[getIndex(p)]; }
	const std::vector<Region> &getRegions() const { return mRegions; }

	Point getExtent() const { return mLayout.mExtent; }
	void getRegionPoints(int index, std::vector<Point> &points) const { mLayout.getRegionPoints(index, points); }
	const std::vector<int> &getRegionNeighbors(int index) const { return mLayout.getRegionNeighbors(index); }
	int getRegionMax(int index) const { return mLayout.getRegionMax(index); }

	bool isEdge(const Point &a, const Point &b) const { return mLayout.isEdge(a, b); }
	bool isLastMove(const Point &a) const;

	bool move(const Point &point, int player);
	bool move(int index, int player);

	bool getWinner(int &winner) const;
};

#endif //BAMMI_BOARD_H
