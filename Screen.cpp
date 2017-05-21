//
// Created by Glenn Smith on 5/7/17.
//

#include "Screen.h"

#include "CursesScreen.h"
#include "CoutScreen.h"

Screen *Screen::getScreen() {
	Screen *screen;
	try {
		screen = new CursesScreen();
	} catch (int) {
		screen = new CoutScreen();
	}
	return screen;
}
