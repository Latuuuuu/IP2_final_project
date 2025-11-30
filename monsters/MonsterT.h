#ifndef MONSTERT_H_INCLUDED
#define MONSTERT_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include <vector>
#include <queue>
#include <ctime>

// fixed settings
enum class Dir {
	UP, DOWN, LEFT, RIGHT
};
enum class MonsterType {
	MONSTER1, MONSTER2, MONSTER3, MONSTER4, MONSTERTYPE_MAX
};
enum class Action {
	IDLE, STAND, CHASE, TELEPORT
};

class MonsterT : public Object
{
public:
	static MonsterT *create_monster(MonsterType type, Point borned_place, Point size);
public:
	MonsterT(MonsterType type, Point borned_place, Point size);
	void update();
	void draw();
	const int &get_money() const { return money; }
	int HP;
protected:
	Point dir_to_vector(const Dir dir);
	int v;
	int money;
	std::vector<std::vector<int>> bitmap_img_ids;
	int bitmap_switch_counter;
	int bitmap_switch_freq;
	int bitmap_img_id;
	Action action;
	Dir dir;
private:
	Dir convert_dir(const Point &v);
	void virtual attack();
	Point hero_tracker();
	MonsterType type;
	Point position;
	clock_t timer;
	clock_t cd_timer;
	int attack_cd;
	int action_timer[4];
	bool need_effect;
	bool can_attack;
	double dist_to_hero;
};

#endif
