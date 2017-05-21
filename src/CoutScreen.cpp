//
// Created by Glenn Smith on 5/7/17.
//

#include "CoutScreen.h"
#include <iostream>
#include <iomanip>

CoutScreen::CoutScreen() {
}

void CoutScreen::print(const Board &board) const {
	for (int y = 0; y < board.mExtent.y + 1; y++) {
		enum Pass : int {
			TopBorder,
			Center,
			MaxPasses
		};
		for (int pass = TopBorder; pass < MaxPasses; pass++) {
			for (int x = 0; x < board.mExtent.x + 1; x++) {
				bool topEdge = std::find(board.mEdges.begin(), board.mEdges.end(), std::pair<Point, Point>{Point(x, y - 1), Point(x, y)}) != board.mEdges.end();
				bool leftEdge = std::find(board.mEdges.begin(), board.mEdges.end(), std::pair<Point, Point>{Point(x - 1, y), Point(x, y)}) != board.mEdges.end();
				bool leftTopEdge = std::find(board.mEdges.begin(), board.mEdges.end(), std::pair<Point, Point>{Point(x - 1, y - 1), Point(x - 1, y)}) != board.mEdges.end();
				bool topLeftEdge = std::find(board.mEdges.begin(), board.mEdges.end(), std::pair<Point, Point>{Point(x - 1, y - 1), Point(x, y - 1)}) != board.mEdges.end();
				if (pass == TopBorder) {
					if (topEdge) {
						if (topLeftEdge && leftEdge && leftTopEdge) {
							std::cout << "╋";
						} else if (leftEdge && topLeftEdge) {
							std::cout << "┣";
						} else if (leftEdge && leftTopEdge) {
							std::cout << "┳";
						} else if (topLeftEdge && leftTopEdge) {
							std::cout << "┻";
						} else if (topLeftEdge) {
							std::cout << "┗";
						} else if (leftEdge) {
							std::cout << "┏";
						} else {
							std::cout << "━";
						}
						std::cout << "━━━";
					} else {
						if (leftTopEdge && topLeftEdge && leftEdge) {
							std::cout << "┫";
						} else if (leftEdge && leftTopEdge) {
							std::cout << "┓";
						} else if (topLeftEdge && leftTopEdge) {
							std::cout << "┛";
						} else if (leftEdge) {
							std::cout << "┃";
						} else {
							std::cout << " ";
						}
						std::cout << "   ";
					}
				} else if (pass == Center) {
					if (leftEdge) {
						std::cout << "┃";
					} else {
						std::cout << " ";
					}
					if (x < board.mExtent.x && y < board.mExtent.y) {
						const Region &r = board.mRegions[board.mIndices[x][y]];
						switch (r.owner) {
							case -1:
								std::cout << " ";
								break;
							default:
								std::cout << (char)('A' + r.owner);
								break;
						}
						bool moved = std::find(board.mLastMove.begin(), board.mLastMove.end(), board.mIndices[x][y]) != board.mLastMove.end();
						if (moved) {
							std::cout << '*' << (r.fill % 10);
						} else {
							std::cout << std::setw(2) << std::right << r.fill;
						}
					} else {
						std::cout << "  ";
					}
				}
			}
			std::cout << std::endl;
		}
	}
}

bool CoutScreen::getMove(Point &move) const {
	std::cout << "Your Move: " << std::endl;
	return (bool)(std::cin >> move.x >> move.y);
}

void CoutScreen::printAt(const std::string &text, const Point &position) const {
	std::cout << text << std::endl;
}

void CoutScreen::delay(const uint64_t &milliseconds) const {
	//Don't
}

bool CoutScreen::getBoardSize(Point &size) const {
	std::cout << "Board Size: " << std::endl;
	return (bool)(std::cin >> size.x >> size.y);
}

void CoutScreen::clear() const {
	//No
}
