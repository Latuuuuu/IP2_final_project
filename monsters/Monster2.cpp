#include "Monster2.h"
#include "../Hero.h"
#include "../Camera.h"
#include "../towers/Bullet.h"
#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>

using namespace std;

void Monster2::attack() {
	DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();
	Bullet *atk;
	if (!state_timer) {
		rand_state = distribution(generator);
		state_timer = 200;
		bullet_state = static_cast<BulletState>(rand_state);
	} else {
		state_timer--;
	}
	if (action == Action::CHASE) {
		// cout << "chase and atk" << endl;
		const Point &t = Point(DC->hero->shape->center_x() - shape->center_x(), DC->hero->shape->center_y() - shape->center_y());
		double d = Point::dist(t);
		const Point &p = Point(t.x / d * max(graph_h, graph_w) + shape->center_x(), 
							   t.y / d * max(graph_h, graph_w) + shape->center_y());
		atk = new Bullet(p, t, BulletSetting::bullet_paths[rand_state], 480, BulletSetting::bullet_dmgs[rand_state], 500, bullet_state);
	} else {
		const Point &t = dir_to_vector(dir);
		const Point &p = Point(t.x * max(graph_h, graph_w) + shape->center_x(), 
							   t.y * max(graph_h, graph_w) + shape->center_y());
		atk = new Bullet(p, t, BulletSetting::bullet_paths[rand_state], 480, BulletSetting::bullet_dmgs[rand_state], 500, bullet_state);
	}
	DC->waveBullets.emplace_back(atk);
}

void Monster2::draw() {
	DataCenter *DC = DataCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	// draw bitmap
	// TODO: change to gif
	char buffer[50];
	sprintf(
		buffer, "%s/%s_%d.png",
		MonsterSetting::monster_imgs_root_path[static_cast<int>(type)],
		MonsterSetting::dir_path_prefix[static_cast<int>(dir)],
		bitmap_img_ids[static_cast<int>(dir)][bitmap_img_id]);
	ALLEGRO_BITMAP *bitmap = IC->get(buffer);
	Point offset = DC->camera->transform_object(*shape);
	al_draw_bitmap(
		bitmap,
		offset.center_x() - al_get_bitmap_width(bitmap) / 2,
		offset.center_y() - al_get_bitmap_height(bitmap) / 2, 0);

	// draw UI, bar will turn red if it is attacked
	ALLEGRO_COLOR bar_color;
	if (last_HP != HP) {
		bar_color = al_map_rgb(220, 20, 60); // red
	} else {
		bar_color = al_map_rgb(100, 100, 100); // gray
	}
	float bar_w = 120;
    float bar_h = 8;
    float bar_x = offset.center_x() - (bar_w / 2);
    float bar_y = offset.center_y() - (graph_h / 2) - 25;
	float hp_ratio = (float)this->HP / (float)max_hp;
    if(hp_ratio < 0) hp_ratio = 0;
	al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, al_map_rgb(50, 50, 50));
    al_draw_filled_rectangle(bar_x, bar_y, bar_x + (bar_w * hp_ratio), bar_y + bar_h, bar_color);
    al_draw_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, al_map_rgb(108, 190, 255), 2);
}