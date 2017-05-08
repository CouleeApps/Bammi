#include <iostream>
#include <thread>
#include "Board.h"
#include "AI.h"
#include "Screen.h"

int main() {
	Board b(Point(10, 10));
	Screen screen;

	AI ai(&b);

	while (true) {
		screen.print(b);
		Point p;
		if (!screen.getMove(p)) {
			break;
		}
		if (!b.move(p, Region::Human)) {
			continue;
		}
//		std::this_thread::sleep_for(std::chrono::seconds(1));
//		int aiMove;
//		if (!ai.getMove(aiMove)) {
//			std::cout << "RIP AI" << std::endl;
//			break;
//		}
//		if (!b.move(aiMove, Region::AI)) {
//			std::cout << "No, AI" << std::endl;
//			break;
//		}
//		b.print();
	}
}