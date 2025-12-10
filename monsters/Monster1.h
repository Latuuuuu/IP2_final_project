#ifndef MONSTER1_H_INCLUDED
#define MONSTER1_H_INCLUDED

#include "MonsterT.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <random>

class Monster1 : public MonsterT
{
public:
	Monster1(Point borned_place) : MonsterT(MonsterType::MONSTER1, borned_place) {
		// DataCenter *DC = DataCenter::get_instance();
		ImageCenter *IC = ImageCenter::get_instance();
		HP = 100;
		v = 60;
		money = 10;
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3})); // UP
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3})); // DOWN
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3})); // LEFT
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3})); // RIGHT
		bitmap_switch_freq = 20;
		char buffer[50];
		sprintf(
			buffer, "%s/%s_%d_%s.png",
			MonsterSetting::monster_imgs_root_path[static_cast<int>(type)],
			MonsterSetting::dir_path_prefix[static_cast<int>(dir)],
			bitmap_img_ids[static_cast<int>(dir)][bitmap_img_id],
			MonsterSetting::bullet_prefix[static_cast<int>(bullet_state)]);
		ALLEGRO_BITMAP *bitmap = IC->get(buffer);
		graph_h = al_get_bitmap_width(bitmap) * 0.8;
		graph_w = al_get_bitmap_height(bitmap) * 0.8;
		shape.reset(new Rectangle{
			(shape->center_x() - graph_w / 2.), (shape->center_y() - graph_h / 2.),
			(shape->center_x() - graph_w / 2. + graph_w), (shape->center_y() - graph_h / 2. + graph_h)
		});
	}
private:
	void attack() override;
	BulletState bullet_state = BulletState::SOLID;
	int graph_h;
	int graph_w;
	std::mt19937 generator;
	std::uniform_int_distribution<int> distribution{1, 3};
	int state_timer = 0;
	int rand_state = 0;
};

#endif
