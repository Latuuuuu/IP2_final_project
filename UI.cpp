#include "UI.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include "Player.h"
#include "Level.h"

// fixed settings
constexpr char love_img_path[] = "./assets/image/love.png";
constexpr int love_img_padding = 5;
constexpr int tower_img_left_padding = 30;
constexpr int tower_img_top_padding = 30;

void
UI_game::init() {
	// DataCenter *DC = DataCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	love = IC->get(love_img_path);
	// int tl_x = DC->game_field_length + tower_img_left_padding;
	// int tl_y = tower_img_top_padding;
	// int max_height = 0;
	// arrange tower shop
	debug_log("<UI_game> state: change to HALT\n");
	state = STATE::HALT;
	on_item = -1;
}

void
UI_game::update() {
	DataCenter *DC = DataCenter::get_instance();
	const Point &mouse = DC->mouse;

	switch(state) {
		case STATE::HALT: {
			for(size_t i = 0; i < tower_items.size(); ++i) {
				auto &[bitmap, p, price] = tower_items[i];
				int w = al_get_bitmap_width(bitmap);
				int h = al_get_bitmap_height(bitmap);
				// hover on a shop tower item
				if(mouse.overlap(Rectangle{p.x, p.y, p.x+w, p.y+h})) {
					on_item = i;
					debug_log("<UI_game> state: change to HOVER\n");
					state = STATE::HOVER;
					break;
				}
			}
			break;
		} case STATE::HOVER: {
			auto &[bitmap, p, price] = tower_items[on_item];
			int w = al_get_bitmap_width(bitmap);
			int h = al_get_bitmap_height(bitmap);
			if(!mouse.overlap(Rectangle{p.x, p.y, p.x+w, p.y+h})) {
				on_item = -1;
				debug_log("<UI_game> state: change to HALT\n");
				state = STATE::HALT;
				break;
			}
			// click mouse left button
			if(DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
				// no money
				if(price > DC->player->coin) {
					debug_log("<UI_game> Not enough money to buy tower %d.\n", on_item);
					break;
				}
				debug_log("<UI_game> state: change to SELECT\n");
				state = STATE::SELECT;
			}
			break;
		} case STATE::SELECT: {
			// click mouse left button: place
			if(DC->mouse_state[1] && !DC->prev_mouse_state[1]) {
				debug_log("<UI_game> state: change to PLACE\n");
				state = STATE::PLACE;
			}
			// click mouse right button: cancel
			if(DC->mouse_state[2] && !DC->prev_mouse_state[2]) {
				on_item = -1;
				debug_log("<UI_game> state: change to HALT\n");
				state = STATE::HALT;
			}
			break;
		} case STATE::PLACE: {
			// check placement legality
		}
	}
}

void
UI_game::draw() {
	DataCenter *DC = DataCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();
	// const Point &mouse = DC->mouse;
	// draw HP
	const int &game_field_length = DC->game_field_length;
	const int &player_HP = DC->player->HP;
	int love_width = al_get_bitmap_width(love);
	for(int i = 1; i <= player_HP; ++i) {
		al_draw_bitmap(love, game_field_length - (love_width + love_img_padding) * i, love_img_padding, 0);
	}
	// draw coin
	// const int &player_coin = DC->player->coin;
	// al_draw_textf(
	// 	FC->courier_new[FontSize::MEDIUM], al_map_rgb(0, 0, 0),
	// 	game_field_length+love_img_padding, love_img_padding,
	// 	ALLEGRO_ALIGN_LEFT, "coin: %5d", player_coin);
	// draw tower shop items
	for(auto &[bitmap, p, price] : tower_items) {
		int w = al_get_bitmap_width(bitmap);
		int h = al_get_bitmap_height(bitmap);
		al_draw_bitmap(bitmap, p.x, p.y, 0);
		al_draw_rectangle(
			p.x - 1, p.y - 1,
			p.x + w + 1, p.y + h + 1,
			al_map_rgb(0, 0, 0), 1);
		al_draw_textf(
			FC->courier_new[FontSize::MEDIUM], al_map_rgb(0, 0, 0),
			p.x + w / 2, p.y + h,
			ALLEGRO_ALIGN_CENTRE, "%d", price);
	}

	switch(state) {
		case STATE::HALT: {
			// No tower should be selected for HALT state.
			break;
		} case STATE::HOVER: {
			auto &[bitmap, p, price] = tower_items[on_item];
			int w = al_get_bitmap_width(bitmap);
			int h = al_get_bitmap_height(bitmap);
			// Create a semitransparent mask covered on the hovered tower.
			al_draw_filled_rectangle(p.x, p.y, p.x + w, p.y + h, al_map_rgba(50, 50, 50, 64));
			break;
		}
		case STATE::SELECT: {
			// If a tower is selected, we new a corresponding tower for previewing purpose.
		}
		case STATE::PLACE: {
			// If we select a tower from menu, we need to preview where the tower will be built and its attack range.
		}
	}
}
