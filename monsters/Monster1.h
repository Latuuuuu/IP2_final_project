#ifndef MONSTER1_H_INCLUDED
#define MONSTER1_H_INCLUDED

#include "MonsterT.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include <random>
#include <map>

class Monster1 : public MonsterT
{
public:
	Monster1(Point borned_place);
	void draw() override;
	// void update() override;
private:
	void attack() override;
	BulletState bullet_state = BulletState::SOLID;
	std::map<Dir, std::string> gifPath;
	std::mt19937 generator;
	std::uniform_int_distribution<int> distribution{1, 3};
	std::vector<Point> atk_dir = {
		Point(0, -1), Point(0.5, -0.866), Point(0.866, -0.5), Point(1, 0), Point(0.5, 0.866), Point(0.866, 0.5), Point(0, 1), Point(-0.5, 0.866), Point(-0.866, 0.5), Point(-1, 0), Point(-0.5, -0.866), Point(-0.866, -0.5)
	};
	int state_timer = 0;
	int rand_state = 0;
	int max_hp = 5000;
};

#endif
