#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <set>

struct Point {
	int x;
	int y;
	Point() : x(0), y(0) {}
	Point(int x, int y) : x(x), y(y) {}
};

struct Region {
	std::vector<Point> points;
	//Use a set so we don't have to worry about duplicates
	std::set<int> neighbors;
	int index;

	Region(const std::vector<Point> &points, int index) : points(points), index(index) {}
};

class Board {
	std::vector<std::vector<int>> indices;
	std::vector<Region> regions;
	Point extent;

	bool randomEmptyCell(Point &point) {
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

		if (emptyPoints.size() == 0) {
			return false;
		}

		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, emptyPoints.size() - 1);
		point = emptyPoints[dist(rd)];
		return true;
	}

	bool newRegion(std::vector<Point> &regionPoints, int index) {
		Point start;
		if (!randomEmptyCell(start)) {
			return false;
		}
		indices[start.x][start.y] = index;
		regionPoints.push_back(start);

		//Spread a bit
		std::random_device rd;
		std::bernoulli_distribution bdist(0.7);
		while (bdist(rd)) {
			std::vector<Point> emptyPoints;
			if (start.x > 0              && indices[start.x - 1][start.y] == -1) emptyPoints.push_back(Point(start.x - 1, start.y));
			if (start.x < (extent.x - 1) && indices[start.x + 1][start.y] == -1) emptyPoints.push_back(Point(start.x + 1, start.y));
			if (start.y > 0              && indices[start.x][start.y - 1] == -1) emptyPoints.push_back(Point(start.x, start.y - 1));
			if (start.y < (extent.y - 1) && indices[start.x][start.y + 1] == -1) emptyPoints.push_back(Point(start.x, start.y + 1));
			if (emptyPoints.size() == 0) break;

			//Add a random one
			std::uniform_int_distribution<int> dist(0, emptyPoints.size() - 1);
			Point next = emptyPoints[dist(rd)];

			indices[next.x][next.y] = index;
			regionPoints.push_back(next);
		}

		return true;
	}

	void assignRegions() {
		std::vector<Point> regionPoints;
		while (newRegion(regionPoints, regions.size())) {
			regions.push_back(Region(regionPoints, regions.size()));
			regionPoints.clear();
		}
	}

	void findNeighbors() {
		for (auto &region : regions) {
			for (const auto &point : region.points) {
				//Up to four
				if (point.x > 0              && indices[point.x - 1][point.y] != region.index) region.neighbors.insert(indices[point.x - 1][point.y]);
				if (point.x < (extent.x - 1) && indices[point.x + 1][point.y] != region.index) region.neighbors.insert(indices[point.x + 1][point.y]);
				if (point.y > 0              && indices[point.x][point.y - 1] != region.index) region.neighbors.insert(indices[point.x][point.y - 1]);
				if (point.y < (extent.y - 1) && indices[point.x][point.y + 1] != region.index) region.neighbors.insert(indices[point.x][point.y + 1]);
			}
		}
	}

public:
	Board(const Point &extent) : extent(extent) {
		indices = std::vector<std::vector<int>>(extent.x, std::vector<int>(extent.y, -1));
		assignRegions();
		findNeighbors();
	}

	int &operator[](const Point &p) { return indices[p.x][p.y]; }
	const int &operator[](const Point &p) const { return indices[p.x][p.y]; }

	void print() const {
		for (int y = 0; y < extent.y; y ++) {
			for (int x = 0; x < extent.x; x ++) {
				std::cout << std::setw(2) << indices[x][y] << " ";
			}
			std::cout << std::endl;
		}
		for (auto &region : regions) {
			std::cout << "Region " << region.index << ": " << std::endl;
			std::cout << "   Points: ";
			for (auto &point : region.points) {
				std::cout << "(" << point.x << ", " << point.y << ") ";
			}
			std::cout << std::endl;
			std::cout << "   Neighbors: ";
			for (auto &neighbor : region.neighbors) {
				std::cout << neighbor << " ";
			}
			std::cout << std::endl;
		}
	}
};

int main() {
	Board b(Point(5, 5));

	b.print();
}