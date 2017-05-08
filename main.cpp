#include <iostream>
#include <thread>
#include "Board.h"
#include "AI.h"
#include "Screen.h"
#include "CursesScreen.h"
#include "CoutScreen.h"

int main() {
	Board b(Point(6, 6));

	//Get something we can display on
	Screen *screen;
	try {
		screen = new CursesScreen();
	} catch (int) {
		screen = new CoutScreen();
	}
	screen->print(b);

	AI ai(&b, 1);

	int winner = 0;

	while (true) {
		Point p;
		if (!screen->getMove(p)) {
			break;
		}
		if (!b.move(p, 0)) {
			continue;
		}
		if (b.getWinner(winner)) {
			break;
		}
		screen->print(b);
//		std::this_thread::sleep_for(std::chrono::seconds(1));
		int aiMove;
		if (!ai.getMove(aiMove)) {
			std::cout << "RIP AI" << std::endl;
			break;
		}
		if (!b.move(aiMove, 1)) {
			std::cout << "No, AI" << std::endl;
			break;
		}
		if (b.getWinner(winner)) {
			break;
		}
		screen->print(b);
	}

	screen->print(b);
	std::cout << "Winner is " << winner << std::endl;

	delete screen;
}