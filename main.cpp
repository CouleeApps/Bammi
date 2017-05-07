#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <set>
#include <deque>
#include <chrono>
#include <thread>

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
	int fill;

	Region(const std::vector<Point> &points, int index) : points(points), index(index), fill(0) {}
};

class Board {
	std::vector<std::vector<int>> indices;
	std::set<std::pair<Point, Point>> edges;
	std::vector<Region> regions;
	std::deque<int> explodeRegions;
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

	void assignRegions() {
		//Make new regions until we run out of points
		std::vector<Point> regionPoints;
		for (int index = 0; newRegion(regionPoints, index); index ++) {
			regions.push_back(Region(regionPoints, index));
			//So we have an empty vector for the next one
			regionPoints.clear();
		}
	}

	void findNeighbors() {
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

	void findEdges() {
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
		for (int y = 0; y < extent.y + 1; y ++) {
			enum Pass : int {
				TopBorder,
				Center,
				MaxPasses
			};
			for (int pass = TopBorder; pass < MaxPasses; pass ++) {
				for (int x = 0; x < extent.x + 1; x++) {
					bool topEdge     = edges.find({Point(x, y - 1), Point(x, y)}) != edges.end();
					bool leftEdge    = edges.find({Point(x - 1, y), Point(x, y)}) != edges.end();
					bool leftTopEdge = edges.find({Point(x - 1, y - 1), Point(x - 1, y)}) != edges.end();
					bool topLeftEdge = edges.find({Point(x - 1, y - 1), Point(x, y - 1)}) != edges.end();
					if (pass == TopBorder) {
						if (topEdge) {
							if (leftEdge || topLeftEdge) {
								std::cout << "+";
							} else {
								std::cout << "-";
							}
							std::cout << "---";
						} else {
							if (leftTopEdge) {
								std::cout << "+";
							} else if (leftEdge) {
								std::cout << "|";
							} else {
								std::cout << " ";
							}
							std::cout << "   ";
						}
					} else if (pass == Center) {
						if (leftEdge) {
							std::cout << "|";
						} else {
							std::cout << " ";
						}
						if (x < extent.x && y < extent.y) {
							if (std::find(explodeRegions.begin(), explodeRegions.end(), indices[x][y]) != explodeRegions.end()) {
								std::cout << "*";
							} else {
								std::cout << " ";
							}
							std::cout << std::setw(2) << std::left << regions[indices[x][y]].fill;
						} else {
							std::cout << "  ";
						}
					}
				}
				std::cout << std::endl;
			}
		}
	}

	bool fillSlice(const Point &point) {
		if (point.x < 0) return false;
		if (point.y < 0) return false;
		if (point.x >= extent.x) return false;
		if (point.y >= extent.y) return false;
		int index = indices[point.x][point.y];
		if (!fillSlice(index)) {
			return false;
		}
		//Explode them in the order they are added so we don't recurse
		for (auto it = explodeRegions.begin(); it != explodeRegions.end(); ) {
			print();
			std::this_thread::sleep_for(std::chrono::seconds(1));

			//Remove from exploded region
			Region &region = regions[*it];
			region.fill -= region.neighbors.size();
			//And fill each neighbor with one (optionally exploding those too)
			for (auto &neighborIndex : region.neighbors) {
				if (!fillSlice(neighborIndex)) {
					return false;
				}
			}
			//Advance
			it = explodeRegions.erase(it);

			//Keep going!
			if (region.fill > region.neighbors.size()) {
				//Split!
				explodeRegions.push_back(region.index);
			}
		}
		return true;
	}
	bool fillSlice(int index) {
		Region &region = regions[index];
		region.fill ++;
		if (region.fill > region.neighbors.size() &&
				std::find(explodeRegions.begin(), explodeRegions.end(), region.index) == explodeRegions.end()) {
			//Split!
			explodeRegions.push_back(region.index);
		}
		return true;
	}
};

int main() {
	Board b(Point(10, 10));
	b.print();

	while (true) {
		Point p;
		std::cin >> p.x >> p.y;
		if (!b.fillSlice(p)) {
			std::cout << "No" << std::endl;
			break;
		}
		b.print();
	}
}