#ifndef MONSTER4_H_INCLUDED
#define MONSTER4_H_INCLUDED

#include "MonsterT.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <random>

class Monster4 : public MonsterT
{
public:
	Monster4(Point borned_place);
	void draw() override;
private:
	void attack() override;
	std::mt19937 generator;
	std::uniform_int_distribution<int> distribution{0, 7};
	int state_timer = 0;
	int rand_state = 0;
	int max_hp = 10000;
};

#endif
