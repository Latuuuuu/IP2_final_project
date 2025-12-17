#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Camera.h"
#include "LevelT.h"
#include "Hero.h"
#include "shapes/Point.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <vector>
#include <cstring>
#include <iostream>
// fixed settings
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_img_path[] = "./assets/image/StartBackground.png";
constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";

/**
 * @brief Game entry.
 * @details The function processes all allegro events and update the event state to a generic data storage (i.e. DataCenter).
 * For timer event, the game_update and game_draw function will be called if and only if the current is timer.
 */
void
Game::execute() {
	DataCenter *DC = DataCenter::get_instance();
	// main game loop
	bool run = true;
	while(run) {
		// process all events here
		al_wait_for_event(event_queue, &event);
		switch(event.type) {
			case ALLEGRO_EVENT_TIMER: {
				run &= game_update();
				game_draw();
				break;
			} case ALLEGRO_EVENT_DISPLAY_CLOSE: { // stop game
				run = false;
				break;
			} case ALLEGRO_EVENT_KEY_DOWN: {
				DC->key_state[event.keyboard.keycode] = true;
				break;
			} case ALLEGRO_EVENT_KEY_UP: {
				DC->key_state[event.keyboard.keycode] = false;
				break;
			} case ALLEGRO_EVENT_MOUSE_AXES: {
				DC->mouse.x = event.mouse.x;
				DC->mouse.y = event.mouse.y;
				break;
			} case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
				DC->mouse_state[event.mouse.button] = true;
				break;
			} case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
				DC->mouse_state[event.mouse.button] = false;
				break;
			} default: break;
		}
	}
}

/**
 * @brief Initialize all allegro addons and the game body.
 * @details Only one timer is created since a game and all its data should be processed synchronously.
 */
Game::Game(bool testMode) {
	DataCenter *DC = DataCenter::get_instance();
	GAME_ASSERT(al_init(), "failed to initialize allegro.");

	// initialize allegro addons
	bool addon_init = true;
	addon_init &= al_init_primitives_addon();
	addon_init &= al_init_font_addon();
	addon_init &= al_init_ttf_addon();
	addon_init &= al_init_image_addon();
	addon_init &= al_init_acodec_addon();
	GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

	if (testMode) {
		timer = nullptr;
		event_queue = nullptr;
		display = nullptr;
		debug_log("Game initialized in test mode.\n");
		return;
	}

	// initialize events
	bool event_init = true;
	event_init &= al_install_keyboard();
	event_init &= al_install_mouse();
	event_init &= al_install_audio();
	GAME_ASSERT(event_init, "failed to initialize allegro events.");

	// initialize game body
	GAME_ASSERT(
		timer = al_create_timer(1.0 / DC->FPS),
		"failed to create timer.");
	GAME_ASSERT(
		event_queue = al_create_event_queue(),
		"failed to create event queue.");
	GAME_ASSERT(
		display = al_create_display(DC->window_width, DC->window_height),
		"failed to create display.");

	debug_log("Game initialized.\n");
	game_init();
}

/**
 * @brief Initialize all auxiliary resources.
 */
void Game::game_init() {
	DataCenter *DC = DataCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();
	// set window icon
	game_icon = IC->get(game_icon_img_path);
	al_set_display_icon(display, game_icon);

	// register events to event_queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// init sound setting
	SC->init();

	// init font setting
	FC->init();

	ui_game = new UI_game();
	ui_game->init();
	ui_menu = new UI_menu();
	ui_menu->init();
	ui_intro = new UI_intro();
	ui_intro->init();
	ui_setting = new UI_setting();
	ui_setting->init();
	ui_levels = new UI_levels();
	ui_levels->init();
	ui_pause = new UI_pause();
	ui_pause->init();

	DC->level->init();

	// game start
	background = IC->get(background_img_path);
	debug_log("Game state: change to START\n");
	state = STATE::START;
	al_start_timer(timer);
}

/**
 * @brief The function processes all data update.
 * @details The behavior of the whole game body is determined by its state.
 * @return Whether the game should keep running (true) or reaches the termination criteria (false).
 * @see Game::STATE
 */
bool Game::game_update() {
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	static ALLEGRO_SAMPLE_INSTANCE *background = nullptr;

	switch(state) {
		case STATE::START: {
			static bool is_played = false;
			static ALLEGRO_SAMPLE_INSTANCE *instance = nullptr;
			// display start music
			if(!is_played) {
				instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
				is_played = true;
			}
			if(!SC->is_playing(instance)) {
				debug_log("<Game> state: change to MENU\n");
				state = STATE::MENU;
			}
			break;
		} case STATE::MENU: {
			int button = -1;
			button = ui_menu->update();
			if (button == 0) {
				state = STATE::INTRO;
			} else if (button == 1) {
				state = STATE::SETTING;
			} else if (button == 2) {
				state = STATE::LEVELS;
			}
			break;
		} case STATE::INTRO: {
			int button = -1;
			button = ui_intro->update();
			if (button == 0) {
				state = STATE::MENU;
			}
			break;
		} case STATE::SETTING: {
			int button = -1;
			button = ui_setting->update();
			if (button == 0) {
				// modify difficulty
			} else if (button == 1) {
				if (SC->gain <= 0.9)
					SC->gain += 0.1;
			} else if (button == 2) {
				if (SC->gain >= 0.1)
					SC->gain -= 0.1;
			} else if (button == 3) {
				state = STATE::MENU;
			}
			break;
		} case STATE::LEVELS: {
			int button = -1;
			button = ui_levels->update();
			// std::cout << button << std::endl;
			if (button == 0) {
				DC->hero->init(1);
				DC->level->load_level(1);
				state = STATE::LEVEL1;
			} else if (button == 1) {
				DC->hero->init(2);
				DC->level->load_level(2);
				state = STATE::LEVEL2;
			} else if (button == 2) {
				DC->hero->init(3);
				DC->level->load_level(3);
				state = STATE::LEVEL3;
			} else if (button == 3) {
				DC->hero->init(4);
				DC->level->load_level(4);
				state = STATE::LEVEL4;
			} else if (button == 4) {
				state = STATE::MENU;
			}
			break;
		} case STATE::LEVEL1: {
			static bool BGM_played = false;
			if (!BGM_played) {
				background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
				BGM_played = true;
			}
			if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
				SC->toggle_playing(background);
				debug_log("<Game> state: change to PAUSE\n");
				last_state = STATE::LEVEL1;
				state = STATE::PAUSE;
			}
			if (DC->level->get_monster_dead()) {
				debug_log("<Game> state: change to LEVEL2\n");
				DC->reset_bullet();
				DC->hero->init(0);
				DC->level->load_level(2);
				state = STATE::LEVEL2;
			} else if (DC->hero->HP == 0) {
				debug_log("<Game> state: change to MENU\n");
				DC->reset_bullet();
				state = STATE::MENU;
			}
			break;
		} case STATE::LEVEL2: {
			static bool BGM_played = false;
			if (!BGM_played) {
				background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
				BGM_played = true;
			}
			if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
				SC->toggle_playing(background);
				debug_log("<Game> state: change to PAUSE\n");
				last_state = STATE::LEVEL2;
				state = STATE::PAUSE;
			}
			if (DC->level->get_monster_dead()) {
				debug_log("<Game> state: change to LEVLE3\n");
				DC->reset_bullet();
				DC->hero->init(0);
				DC->level->load_level(3);
				state = STATE::LEVEL3;
			} else if (DC->hero->HP == 0) {
				debug_log("<Game> state: change to MENU\n");
				DC->reset_bullet();
				state = STATE::MENU;
			}
			break;
		} case STATE::LEVEL3: {
			static bool BGM_played = false;
			if (!BGM_played) {
				background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
				BGM_played = true;
			}
			if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
				SC->toggle_playing(background);
				debug_log("<Game> state: change to PAUSE\n");
				last_state = STATE::LEVEL3;
				state = STATE::PAUSE;
			}
			if (DC->level->get_monster_dead()) {
				debug_log("<Game> state: change to LEVEL4\n");
				DC->reset_bullet();
				DC->hero->init(0);
				DC->level->load_level(4);
				state = STATE::LEVEL4;
			} else if (DC->hero->HP == 0) {
				debug_log("<Game> state: change to MENU\n");
				DC->reset_bullet();
				state = STATE::MENU;
			}
			break;
		} case STATE::LEVEL4: {
			static bool BGM_played = false;
			if (!BGM_played) {
				background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
				BGM_played = true;
			}
			if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
				SC->toggle_playing(background);
				debug_log("<Game> state: change to PAUSE\n");
				last_state = STATE::LEVEL4;
				state = STATE::PAUSE;
			}
			if (DC->level->get_monster_dead()) {
				debug_log("<Game> state: change to MENU\n");
				DC->reset_bullet();
				state = STATE::MENU;
			} else if (DC->hero->HP == 0) {
				debug_log("<Game> state: change to MENU\n");
				DC->reset_bullet();
				state = STATE::MENU;
			}
			break;
		} case STATE::PAUSE: {
			int button = ui_pause->update();
			if ((DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) || button == 0) { // resume
				SC->toggle_playing(background);
				debug_log("<Game> state: change to last state\n");
				state = last_state;
			} else if (button == 1) { // restart
				DC->reset_bullet();
				DC->hero->init(level_map[last_state]);
				DC->level->load_level(level_map[last_state]);
				state = last_state;
			} else if (button == 2) { // to menu
				DC->reset_bullet();
				state = STATE::MENU;
			}
			break;
		} case STATE::END: {
			return false;
		}
	}
	// If the game is not paused, we should progress update.
	if(state != STATE::PAUSE) {
		// DC->player->update();
		SC->update();
		if (state == STATE::LEVEL1) {
			DC->hero->update();
			DC->level->update1();
			OC->update();
		} else if (state == STATE::LEVEL2) {
			DC->hero->update();
			DC->level->update2();
			OC->update();
		} else if (state == STATE::LEVEL3) {
			DC->hero->update();
			DC->level->update3();
			OC->update();
		} else if (state == STATE::LEVEL4) {
			DC->hero->update();
			DC->level->update4();
			OC->update();
		}
	}
	// game_update is finished. The states of current frame will be previous states of the next frame.
	memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
	memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));
	return true;
}

/**
 * @brief Draw the whole game and objects.
 */
void
Game::game_draw() {
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();

	// Flush the screen first.
	al_clear_to_color(al_map_rgb(100, 100, 100));
	if(state != STATE::END) {
		// background
		// al_draw_bitmap(background, 0, 0, 0);
		// if(DC->game_field_length < DC->window_width)
		// 	al_draw_filled_rectangle(
		// 		DC->game_field_length, 0,
		// 		DC->window_width, DC->window_height,
		// 		al_map_rgb(100, 100, 100));
		// if(DC->game_field_length < DC->window_height)
		// 	al_draw_filled_rectangle(
		// 		0, DC->game_field_length,
		// 		DC->window_width, DC->window_height,
		// 		al_map_rgb(100, 100, 100));
		// user interface
		if (state == STATE::START) {
			// draw start vedio
		} else if (state == STATE::MENU) {
			al_draw_bitmap(background, 0, 0, 0);
			ui_menu->draw();
		} else if (state == STATE::INTRO) {
			al_draw_bitmap(background, 0, 0, 0);
			ui_intro->draw();
		} else if (state == STATE::SETTING) {
			al_draw_bitmap(background, 0, 0, 0);
			ui_setting->draw();
		} else if (state == STATE::LEVELS) {
			al_draw_bitmap(background, 0, 0, 0);
			ui_levels->draw();
		} else if(state == STATE::LEVEL1 || state == STATE::LEVEL2 || state == STATE::LEVEL3 || state == STATE::LEVEL4) {
			// al_draw_bitmap(background,
			// 				DC->camera->transform_bitmap(0, 0).center_x(),
			// 				DC->camera->transform_bitmap(0, 0).center_y(), 0);
			DC->camera->update_camera(Point(DC->hero->shape->center_x(),
											DC->hero->shape->center_y()), 
									Point(0, 0));
			DC->level->draw();
			DC->hero->draw();
			ui_game->draw();
			OC->draw();
		} else if (state == STATE::PAUSE) {
			al_draw_bitmap(background,
							DC->camera->transform_bitmap(0, 0).center_x(),
							DC->camera->transform_bitmap(0, 0).center_y(), 0);
			DC->level->draw();
			DC->hero->draw();
			OC->draw();
			// game layout cover
			al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(30, 30, 30, 80));
			ui_pause->draw();
			al_draw_text(
				FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
				DC->window_width/2., DC->window_height/2.,
				ALLEGRO_ALIGN_CENTRE, "GAME PAUSED");
		}
	}
	// switch(state) {
	// 	case STATE::START: {
	// 	} case STATE::LEVEL: {
	// 		break;
	// 	} case STATE::PAUSE: {
	// 		// game layout cover
	// 		al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(50, 50, 50, 64));
	// 		al_draw_text(
	// 			FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
	// 			DC->window_width/2., DC->window_height/2.,
	// 			ALLEGRO_ALIGN_CENTRE, "GAME PAUSED");
	// 		break;
	// 	} case STATE::END: {
	// 	}
	// }
	al_flip_display();
}

Game::~Game() {
	if(display) al_destroy_display(display);
	if(timer) al_destroy_timer(timer);
	if(event_queue) al_destroy_event_queue(event_queue);
}
