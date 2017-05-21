//
// Created by Glenn Smith on 5/7/17.
//

#include <iostream>
#include "Board.h"
#include "AI.h"
#include "Screen.h"

enum {
	PLAYER_HUMAN = 0,
	PLAYER_AI = 1
};

int main() {
	//Get something we can display on
	Screen *screen = Screen::getScreen();
	Point boardSize;
	screen->getBoardSize(boardSize);

	//Initial state
	int winner;
	Board::Layout layout(boardSize);
	Board board(layout);
	screen->print(board);

	//Load AI
	AI player2(board, PLAYER_AI);

	//Main play loop
	while (true) {
		//Human player
		{
			Point move;
			if (!screen->getMove(move)) {
				break;
			}
			if (!board.move(move, PLAYER_HUMAN)) {
				continue;
			}
			if (board.getWinner(winner)) {
				break;
			}
			screen->clear();
			screen->print(board);
		}
		//AI player
		{
			int move;
			if (!player2.getMove(move)) {
				std::cout << "RIP AI" << std::endl;
				break;
			}
			screen->delay(300);
			if (!board.move(move, PLAYER_AI)) {
				std::cout << "No, AI" << std::endl;
				break;
			}
			if (board.getWinner(winner)) {
				break;
			}
			screen->print(board);
		}
	}

	//Ended the game, print final state
	screen->print(board);
	if (winner == PLAYER_HUMAN) {
		screen->printAt("You win!", {0, boardSize.y * 2 + 2});
	} else {
		screen->printAt("You lose!", {0, boardSize.y * 2 + 2});
	}
	screen->delay(3000);

	//Clean up
	delete screen;
}