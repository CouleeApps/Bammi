#include <iostream>
#include <thread>
#include "Board.h"
#include "AI.h"
#include "Screen.h"

int main() {
	Board b(Point(7, 7));

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
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
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
	std::this_thread::sleep_for(std::chrono::seconds(3));

	delete screen;
}