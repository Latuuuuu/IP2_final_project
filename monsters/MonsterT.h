#ifndef MONSTERT_H_INCLUDED
#define MONSTERT_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "../shapes/Point.h"
#include "../towers/Bullet.h"
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
namespace MonsterSetting {
	static constexpr char monster_imgs_root_path[static_cast<int>(MonsterType::MONSTERTYPE_MAX)][40] = {
		"./assets/gif/monster/Monster1/monster_1",
		"./assets/image/monster/Monster2",
		"./assets/image/monster/Monster3",
		"./assets/image/monster/Monster4"
	};
	static constexpr char dir_path_prefix[][10] = {
		"up", "down", "left", "right" //"UP", "DOWN", "LEFT", "RIGHT"
	};
	static constexpr char bullet_prefix[][10] = {
		"", "ice", "water", "vapor"
	};
}

class OperationCenter;

class MonsterT : public Object
{
public:
	static MonsterT *create_monster(MonsterType type, Point borned_place);
	friend class OperationCenter;
public:
	MonsterT(MonsterType type, Point borned_place);
	void virtual update();
	void virtual draw();
	const int &get_money() const { return money; }
	int HP;
	BulletState bullet_state;
	const BulletState get_bullet_state() { return bullet_state; }
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
	MonsterType type;
private:
	Dir convert_dir(const Point &v);
	void virtual attack();
	Point hero_tracker();
	Point position;
	clock_t timer;
	clock_t cd_timer;
	int attack_cd;
	int action_timer[4];
	bool need_effect;
	bool can_attack;
	bool is_collid;
	double dist_to_hero;
};

#endif
