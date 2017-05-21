#include <iostream>
#include "Board.h"
#include "AI.h"
#include "Screen.h"

int main() {
	Board b(Point(5, 5));

	//Get something we can display on
	Screen *screen = Screen::getScreen();
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
		int aiMove;
		if (!ai.getMove(aiMove)) {
			std::cout << "RIP AI" << std::endl;
			break;
		}
		screen->delay(100);
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
	screen->delay(3000);

	delete screen;
}