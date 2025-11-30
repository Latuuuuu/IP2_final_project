#ifndef MONSTER3_H_INCLUDED
#define MONSTER3_H_INCLUDED

#include "MonsterT.h"

class Monster3 : public MonsterT
{
public:
	Monster3(Point borned_place, Point size) : MonsterT(MonsterType::MONSTER3, borned_place, size) {
		HP = 10;
		v = 60;
		money = 10;
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3})); // UP
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3})); // DOWN
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3, 4})); // LEFT
		bitmap_img_ids.emplace_back(std::vector<int>({0, 1, 2, 3, 4})); // RIGHT
		bitmap_switch_freq = 20;
	}
private:
	void attack() override;
};

#endif
