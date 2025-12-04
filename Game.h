#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <allegro5/allegro.h>
#include "UI.h"
#include "UI_menu.h"
#include "UI_levels.h"
#include "UI_setting.h"
#include "UI_intro.h"
#include "UI_pause.h"
#include <map>

/**
 * @brief Main class that runs the whole game.
 * @details All game procedures must be processed through this class.
 */
class Game
{
public:
	void execute();
public:
	Game(bool testMode = false);
	~Game();
	void game_init();
	bool game_update();
	void game_draw();
private:
	/**
	 * @brief States of the game process in game_update.
	 * @see Game::game_update()
	 */
	enum class STATE {
		START, // -> MENU
		MENU, // -> INTRO, SETTING, LEVELS,
		INTRO, // -> MENU
		SETTING, // -> MENU
		LEVELS, // -> MENU, LEVEL1, LEVEL2, LEVEL3, LEVEL4
		LEVEL1, // -> PAUSE, END, MENU, LEVEL2
		LEVEL2, // -> PAUSE, END, MENU, LEVEL3
		LEVEL3, // -> PAUSE, END, MENU, LEVEL4
		LEVEL4, // -> PAUSE, END, MENU
		PAUSE, // -> LEVEL1, LEVEL2, LEVEL3, LEVEL4, MENU
		END
	};
	std::map<STATE, int> level_map = {
		{STATE::LEVEL1, 1},
		{STATE::LEVEL2, 2},
		{STATE::LEVEL3, 3},
		{STATE::LEVEL4, 4}
	};
	STATE state, last_state;
	ALLEGRO_EVENT event;
	ALLEGRO_BITMAP *game_icon;
	ALLEGRO_BITMAP *background;
private:
	ALLEGRO_DISPLAY *display;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *event_queue;
	UI_game *ui_game;
	UI_menu *ui_menu;
	UI_intro *ui_intro;
	UI_setting *ui_setting;
	UI_levels *ui_levels;
	UI_pause *ui_pause;
};

#endif
