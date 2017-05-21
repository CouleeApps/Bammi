//
// Created by Glenn Smith on 5/7/17.
//

#include "CursesScreen.h"
#include <iostream>
#include <iomanip>
#include <curses.h>
#include <thread>

void finish(int) {
	endwin();
	exit(0);
}

CursesScreen::CursesScreen() {
	if (getenv("TERM") == NULL) {
		throw 0;
	}
	signal(SIGINT, finish);
	initscr();
	keypad(stdscr, TRUE);
	nonl();
	cbreak();
	echo();

	mousemask(BUTTON1_CLICKED, NULL);

	if (has_colors()) {
		start_color();

		init_pair(0, COLOR_BLACK, COLOR_BLACK);
		init_pair(1, COLOR_BLACK, COLOR_BLUE);
		init_pair(2, COLOR_BLACK, COLOR_RED);
	}
}

CursesScreen::~CursesScreen() {
	endwin();
}

void CursesScreen::print(const Board &board) const {
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
					//Top left corner
					move(y * 2, x * 4);
					if (topEdge) { //Has at least right side
						if (leftEdge) { //Has at least bottom side
							if (topLeftEdge && leftTopEdge) {
								addch(ACS_PLUS); // ╋
							} else if (topLeftEdge) {
								addch(ACS_LTEE); // ┣
							} else if (leftTopEdge) {
								addch(ACS_TTEE); // ┳
							} else {
								addch(ACS_ULCORNER); // ┏
							}
						} else {
							if (topLeftEdge && leftTopEdge) {
								addch(ACS_BTEE); // ┻
							} else if (topLeftEdge) {
								addch(ACS_LLCORNER); // ┗
							} else {
								addch(ACS_HLINE); // ━
							}
						}
					} else {//No right side
						if (leftEdge && leftTopEdge && topLeftEdge) {
							addch(ACS_RTEE); // ┫
						} else if (leftEdge && leftTopEdge) {
							addch(ACS_URCORNER); // ┓
						} else if (leftEdge) {
							addch(ACS_VLINE); // ┃
						} else if (topLeftEdge && leftTopEdge) {
							addch(ACS_LRCORNER); // ┛
						}
					}

					if (topEdge) {
						mvaddch(y * 2, x * 4 + 1, ACS_HLINE);
						mvaddch(y * 2, x * 4 + 2, ACS_HLINE);
						mvaddch(y * 2, x * 4 + 3, ACS_HLINE);
					}

				} else if (pass == Center) {
					//Left side
					if (leftEdge) {
						mvaddch(y * 2 + 1, x * 4, ACS_VLINE); // ┃
					}
					//Center number
					if (x < board.extent.x && y < board.extent.y) {
						if (std::find(board.explodeRegions.begin(), board.explodeRegions.end(),
						              board.indices[x][y]) != board.explodeRegions.end()) {
							mvaddch(y * 2 + 1, x * 4 + 1, '*');
						}
						auto color = COLOR_PAIR(0);
						switch (board.regions[board.indices[x][y]].owner) {
							case 0:
								color = COLOR_PAIR(1);
								break;
							case 1:
								color = COLOR_PAIR(2);
								break;
							default:
								color = COLOR_PAIR(0);
								break;
						}
						bool moved = std::find(board.lastMove.begin(), board.lastMove.end(), board.indices[x][y]) != board.lastMove.end();

						if (moved) {
							attrset(color);
						} else {
							attrset(COLOR_PAIR(0));
						}
						mvaddch(y * 2 + 1, x * 4 + 1, ' ');
						attrset(color);
						char num[3];
						snprintf(num, 3, "%d", board.regions[board.indices[x][y]].fill);
						mvaddch(y * 2 + 1, x * 4 + 2, num[0]);
						if (moved) {
							attrset(color);
						} else {
							attrset(COLOR_PAIR(0));
						}
						if (num[1]) {
							mvaddch(y * 2 + 1, x * 4 + 3, num[1]);
						} else {
							mvaddch(y * 2 + 1, x * 4 + 3, ' ');
						}
						attrset(COLOR_PAIR(0));
					}
				}
			}
		}
	}
	refresh();
}

bool CursesScreen::getMove(Point &move) const {
	MEVENT mouse;

	int ch = 0;
	while (ch != KEY_MOUSE) {
		ch = getch();
	}
	if (getmouse(&mouse) != OK) {
		return false;
	}

	move.x = mouse.x / 4;
	move.y = mouse.y / 2;

	return true;
}

void CursesScreen::printAt(const Point &position, const std::string &text) const {
	Point current(position);
	for (const auto &ch : text) {
		mvaddch(current.y, current.x, ch);
		current.x ++;
	}
}

void CursesScreen::delay(const uint64_t &milliseconds) const {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
