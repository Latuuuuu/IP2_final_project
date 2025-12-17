#ifndef MONSTER2_H_INCLUDED
#define MONSTER2_H_INCLUDED

#include "MonsterT.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <random>
#include <iostream>

class Monster2 : public MonsterT
{
public:
	Monster2(Point borned_place);
	void draw() override;
private:
	void attack() override;
	std::map<Dir, std::string> gifPath;
	std::mt19937 generator;
	std::uniform_int_distribution<int> distribution{6, 7};
	int state_timer = 0;
	int rand_state = 0;
	int max_hp = 5000;
};

#endif
