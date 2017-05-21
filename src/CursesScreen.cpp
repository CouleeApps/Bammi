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
	for (int y = 0; y < board.getExtent().y + 1; y++) {
		enum Pass : int {
			TopBorder,
			Center,
			MaxPasses
		};
		for (int pass = TopBorder; pass < MaxPasses; pass++) {
			for (int x = 0; x < board.getExtent().x + 1; x++) {
				bool topEdge = std::find(board.mLayout.mEdges.begin(), board.mLayout.mEdges.end(), std::pair<Point, Point>{Point(x, y - 1), Point(x, y)}) != board.mLayout.mEdges.end();
				bool leftEdge = std::find(board.mLayout.mEdges.begin(), board.mLayout.mEdges.end(), std::pair<Point, Point>{Point(x - 1, y), Point(x, y)}) != board.mLayout.mEdges.end();
				bool leftTopEdge = std::find(board.mLayout.mEdges.begin(), board.mLayout.mEdges.end(), std::pair<Point, Point>{Point(x - 1, y - 1), Point(x - 1, y)}) != board.mLayout.mEdges.end();
				bool topLeftEdge = std::find(board.mLayout.mEdges.begin(), board.mLayout.mEdges.end(), std::pair<Point, Point>{Point(x - 1, y - 1), Point(x, y - 1)}) != board.mLayout.mEdges.end();
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
					if (x < board.getExtent().x && y < board.getExtent().y) {
						if (std::find(board.mExplodeRegions.begin(), board.mExplodeRegions.end(),
						              board[{x, y}]) != board.mExplodeRegions.end()) {
							mvaddch(y * 2 + 1, x * 4 + 1, '*');
						}
						auto color = COLOR_PAIR(0);
						switch (board.mRegions[board[{x, y}]].owner) {
							case 0:
								color = COLOR_PAIR(1);
								break;
							case 1:
								color = COLOR_PAIR(2);
								break;
							default:
								break;
						}
						bool moved = std::find(board.mLastMove.begin(), board.mLastMove.end(), board[{x, y}]) != board.mLastMove.end();

						if (moved) {
							attrset(color);
						} else {
							attrset(COLOR_PAIR(0));
						}
						mvaddch(y * 2 + 1, x * 4 + 1, ' ');
						attrset(color);
						char num[3];
						snprintf(num, 3, "%d", board.mRegions[board[{x, y}]].fill);
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

void CursesScreen::printAt(const std::string &text, const Point &position) const {
	Point current(position);
	for (const auto &ch : text) {
		mvaddch(current.y, current.x, ch);
		current.x ++;
	}
	refresh();
}

void CursesScreen::delay(const uint64_t &milliseconds) const {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

bool CursesScreen::getBoardSize(Point &size) const {
	printAt("Board Size: ", {0, 0});

	char input[128];
	if (getnstr(input, 128) == ERR) {
		return false;
	}

	sscanf(input, "%d %d", &size.x, &size.y);

	return true;
}

void CursesScreen::clear() const {
	erase();
}
