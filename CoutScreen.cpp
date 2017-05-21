//
// Created by Glenn Smith on 5/7/17.
//

#include "CoutScreen.h"
#include <iostream>
#include <iomanip>
#include <curses.h>

CoutScreen::CoutScreen() {
}

void CoutScreen::print(const Board &board) const {
	for (int y = 0; y < board.extent.y + 1; y++) {
		enum Pass : int {
			TopBorder,
			Center,
			MaxPasses
		};
		for (int pass = TopBorder; pass < MaxPasses; pass++) {
			for (int x = 0; x < board.extent.x + 1; x++) {
				bool topEdge = board.edges.find({Point(x, y - 1), Point(x, y)}) != board.edges.end();
				bool leftEdge = board.edges.find({Point(x - 1, y), Point(x, y)}) != board.edges.end();
				bool leftTopEdge =
						board.edges.find({Point(x - 1, y - 1), Point(x - 1, y)}) != board.edges.end();
				bool topLeftEdge =
						board.edges.find({Point(x - 1, y - 1), Point(x, y - 1)}) != board.edges.end();
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
						} else if (topLeftEdge && topEdge) {
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
					if (x < board.extent.x && y < board.extent.y) {
						const Region &r = board.regions[board.indices[x][y]];
						switch (r.owner) {
							case -1:
								std::cout << " ";
								break;
							default:
								std::cout << (char)('A' + r.owner);
								break;
						}
						std::cout << std::setw(2) << std::left << r.fill;
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
	std::cin >> move.x >> move.y;
	return true;
}

void CoutScreen::printAt(const Point &position, const std::string &text) const {
	std::cout << text << std::endl;
}

void CoutScreen::delay(const uint64_t &milliseconds) const {
	//Don't
}
