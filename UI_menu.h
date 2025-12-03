#ifndef UIMENU_H_INCLUDED
#define UIMENU_H_INCLUDED

#include "Utils.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "shapes/Rectangle.h"

#include <allegro5/bitmap.h>
#include <vector>
#include <tuple>
#include "UI_T.h"
#include "./shapes/Point.h"

class UI_menu : public UI {
public:
	UI_menu() {}
	void init() override {
		DataCenter *DC = DataCenter::get_instance();
		ImageCenter *IC = ImageCenter::get_instance();
		int tl_x = DC->game_field_length + button_img_left_padding;
		int tl_y = button_img_top_padding;
		int max_height = 0;
		// arrange tower shop
		for(size_t i = 0; i < (size_t)(MenuSetting::ButtonType::BUTTON_MAX); ++i) {
			ALLEGRO_BITMAP *bitmap = IC->get(MenuSetting::button_img_path[i]);
			int w = al_get_bitmap_width(bitmap);
			int h = al_get_bitmap_height(bitmap);
			if(tl_x + w > DC->window_width) {
				tl_x = DC->game_field_length + button_img_left_padding;
				tl_y += max_height + button_img_top_padding;
				max_height = 0;
			}
			button_items.emplace_back(bitmap, Point{tl_x, tl_y});
			tl_x += w + button_img_left_padding;
			max_height = std::max(max_height, h);
		}
		debug_log("<UI menu> state: change to HALT\n");
		state = STATE::HALT;
		on_item = -1;
	}
private:
};

#endif