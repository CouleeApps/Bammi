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

	bool operator<(const Point &other) const {
		return x < other.x || (x == other.x && y < other.y);
	}
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
	std::set<std::pair<Point, Point>> edges;
	std::vector<Region> regions;
	Point extent;

	bool randomEmptyCell(Point &point) {
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

	bool newRegion(std::vector<Point> &regionPoints, int index) {
		Point start;
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

	void assignRegions() {
		std::vector<Point> regionPoints;
		for (int index = 0; newRegion(regionPoints, index); index ++) {
			regions.push_back(Region(regionPoints, index));
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

	void findEdges() {
		for (int x = 0; x < extent.x; x ++) {
			for (int y = 0; y < extent.y; y ++) {
				int at = indices[x][y];
				if (x == 0) edges.insert({Point(-1, y), Point(x, y)});
				else if (indices[x - 1][y] != at) edges.insert({Point(x - 1, y), Point(x, y)});
				if (y == 0) edges.insert({Point(x, -1), Point(x, y)});
				else if (indices[x][y - 1] != at) edges.insert({Point(x, y - 1), Point(x, y)});
				if (x == extent.x - 1) edges.insert({Point(x, y), Point(extent.x, y)});
				else if (indices[x + 1][y] != at) edges.insert({Point(x + 1, y), Point(x, y)});
				if (y == extent.y - 1) edges.insert({Point(x, y), Point(x, extent.y)});
				else if (indices[x][y + 1] != at) edges.insert({Point(x, y + 1), Point(x, y)});
			}
		}
	}

public:
	Board(const Point &extent) : extent(extent) {
		indices = std::vector<std::vector<int>>(static_cast<size_t>(extent.x), std::vector<int>(static_cast<size_t>(extent.y), -1));
		assignRegions();
		findNeighbors();
		findEdges();
	}

	int &operator[](const Point &p) { return indices[p.x][p.y]; }
	const int &operator[](const Point &p) const { return indices[p.x][p.y]; }

	Region &getRegion(int index) { return regions[index]; }
	const Region &getRegion(int index) const { return regions[index]; }

	void print() const {
		for (int y = 0; y < extent.y; y ++) {
			enum Pass : int {
				TopBorder,
				Center,
				BottomBorder,
				MaxPasses
			};
			for (int pass = TopBorder; pass < MaxPasses; pass ++) {
				for (int x = 0; x < extent.x; x++) {
					bool topEdge = edges.find({Point(x, y - 1), Point(x, y)}) != edges.end();
					bool leftEdge = edges.find({Point(x - 1, y), Point(x, y)}) != edges.end();
					bool rightEdge = edges.find({Point(x, y), Point(x + 1, y)}) != edges.end();
					bool bottomEdge = edges.find({Point(x, y), Point(x, y + 1)}) != edges.end();
					switch (pass) {
						case TopBorder:
							if (topEdge) {
								if (leftEdge) {
									std::cout << "+";
								} else {
									std::cout << "-";
								}
								std::cout << "--";
								if (rightEdge) {
									std::cout << "+";
								} else {
									std::cout << "-";
								}
							} else {
								if (leftEdge) {
									std::cout << "|";
								} else {
									std::cout << " ";
								}
								std::cout << "  ";
								if (rightEdge) {
									std::cout << "|";
								} else {
									std::cout << " ";
								}
							}
							break;
						case Center:
							if (leftEdge) {
								std::cout << "|";
							} else {
								std::cout << " ";
							}
							std::cout << std::setw(2) << regions[indices[x][y]].neighbors.size();
							if (rightEdge) {
								std::cout << "|";
							} else {
								std::cout << " ";
							}
							break;
						case BottomBorder:
							if (bottomEdge) {
								if (leftEdge) {
									std::cout << "+";
								} else {
									std::cout << "-";
								}
								std::cout << "--";
								if (rightEdge) {
									std::cout << "+";
								} else {
									std::cout << "-";
								}
							} else {
								if (leftEdge) {
									std::cout << "|";
								} else {
									std::cout << " ";
								}
								std::cout << "  ";
								if (rightEdge) {
									std::cout << "|";
								} else {
									std::cout << " ";
								}
							}
							break;
						default:
							break;
					}

				}
				std::cout << std::endl;
			}
		}
	}
};

int main() {
	Board b(Point(10, 10));

	b.print();
}