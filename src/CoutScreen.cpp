//
// Created by Glenn Smith on 5/7/17.
//

#include "CoutScreen.h"
#include <iostream>
#include <iomanip>

CoutScreen::CoutScreen() {
}

void CoutScreen::print(const Board &board) const {
	for (int y = 0; y < board.getExtent().y + 1; y++) {
		enum Pass : int {
			TopBorder,
			Center,
			MaxPasses
		};
		for (int pass = TopBorder; pass < MaxPasses; pass++) {
			for (int x = 0; x < board.getExtent().x + 1; x++) {
				bool topEdge     = board.isEdge({x, y - 1},     {x, y});
				bool leftEdge    = board.isEdge({x - 1, y},     {x, y});
				bool leftTopEdge = board.isEdge({x - 1, y - 1}, {x - 1, y});
				bool topLeftEdge = board.isEdge({x - 1, y - 1}, {x, y - 1});
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
					if (x < board.getExtent().x && y < board.getExtent().y) {
						const Region &r = board.getRegion({x, y});
						switch (r.owner) {
							case Region::UNOWNED:
								std::cout << " ";
								break;
							default:
								std::cout << (char)('A' + r.owner);
								break;
						}
						bool moved = board.isLastMove({x, y});
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

void CoutScreen::printAt(const std::string &text, const Point &) const {
	std::cout << text << std::endl;
}

void CoutScreen::delay(const uint64_t &) const {
	//Don't
}

bool CoutScreen::getBoardSize(Point &size) const {
	std::cout << "Board Size: " << std::endl;
	return (bool)(std::cin >> size.x >> size.y);
}

void CoutScreen::clear() const {
	//No
}
