//
// Created by Glenn Smith on 5/7/17.
//

#include <iostream>
#include "Board.h"
#include "AI.h"
#include "Screen.h"

int main() {
	//Get something we can display on
	Screen *screen = Screen::getScreen();
	Point boardSize;
	screen->getBoardSize(boardSize);

	//Initial state
	int winner = 0;
	Board b(boardSize);
	screen->print(b);

	//Load AI
	AI ai(&b, 1);

	//Main play loop
	while (true) {
		//Human player
		{
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
			screen->clear();
			screen->print(b);
		}
		//AI player
		{
			int aiMove;
			if (!ai.getMove(aiMove)) {
				std::cout << "RIP AI" << std::endl;
				break;
			}
			screen->delay(300);
			if (!b.move(aiMove, 1)) {
				std::cout << "No, AI" << std::endl;
				break;
			}
			if (b.getWinner(winner)) {
				break;
			}
			screen->print(b);
		}
	}

	screen->clear();

	//Ended the game, print final state
	screen->print(b);
	if (winner == 0) {
		screen->printAt("You win!", {0, boardSize.y * 2 + 2});
	} else {
		screen->printAt("You lose!", {0, boardSize.y * 2 + 2});
	}
	screen->delay(3000);

	//Clean up
	delete screen;
}