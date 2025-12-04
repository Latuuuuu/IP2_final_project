#include "UI_T.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"

int UI::update() {
	DataCenter *DC = DataCenter::get_instance();
	const Point &mouse = DC->mouse;

	switch (state) {
		case STATE::HALT: {
			for (size_t i = 0; i < button_items.size(); ++i) {
				auto &[bitmap, p] = button_items[i];
				int w = al_get_bitmap_width(bitmap);
				int h = al_get_bitmap_height(bitmap);
				// hover on a shop tower item
				if (DC->mouse_state[1] && !DC->prev_mouse_state[1] && mouse.overlap(Rectangle{p.x, p.y, p.x+w, p.y+h})) {
					on_item = i;
					// debug_log("<UI> state: change to SELECT\n");
					state = STATE::SELECT;
					break;
				} else {
					on_item = -1;
				}
			}
			break;
		} case STATE::SELECT: {
			auto &[bitmap, p] = button_items[on_item];
			int w = al_get_bitmap_width(bitmap);
			int h = al_get_bitmap_height(bitmap);
			if (!mouse.overlap(Rectangle{p.x, p.y, p.x+w, p.y+h})) {
				on_item = -1;
				// debug_log("<UI> state: change to HALT\n");
				state = STATE::HALT;
				break;
			} else if (!DC->mouse_state[1] && DC->prev_mouse_state[1]) {
				// debug_log("<UI> state: change to HALT\n");
				state = STATE::HALT;
				return on_item;
			}
			break;
		} default : {
			on_item = -1;
			break;
		}
	}
	return -1;
}

void UI::draw() {
	// DataCenter *DC = DataCenter::get_instance();
	// FontCenter *FC = FontCenter::get_instance();
	// const Point &mouse = DC->mouse;
	// draw button
	for(auto &[bitmap, p] : button_items) {
		int w = al_get_bitmap_width(bitmap);
		int h = al_get_bitmap_height(bitmap);
		al_draw_bitmap(bitmap, p.x, p.y, 0);
		al_draw_rectangle(
			p.x - 1, p.y - 1,
			p.x + w + 1, p.y + h + 1,
			al_map_rgb(0, 0, 0), 1);
	}

	switch(state) {
		case STATE::HALT: {
			break;
		} case STATE::SELECT: {
			auto &[bitmap, p] = button_items[on_item];
			int w = al_get_bitmap_width(bitmap);
			int h = al_get_bitmap_height(bitmap);
			// Create a semitransparent mask covered on the hovered tower.
			al_draw_filled_rectangle(p.x, p.y, p.x + w, p.y + h, al_map_rgba(50, 50, 50, 64));
			break;
		} default : {
			break;
		}
	}
}
