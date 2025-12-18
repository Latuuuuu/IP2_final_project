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
	static constexpr char monster_imgs_root_path[static_cast<int>(MonsterType::MONSTERTYPE_MAX)][50] = {
		"./assets/gif/monster/Monster1/monster_1",
		"./assets/gif/monster/Monster2/monster_2",
		"./assets/gif/monster/Monster3/monster_3",
		"./assets/image/monster/Professer/professer"
	};
	static constexpr char dir_path_prefix[][10] = {
		"UP", "DOWN", "LEFT", "RIGHT" //"UP", "DOWN", "LEFT", "RIGHT"
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
	int HP, last_HP;
	int e;
	BulletState bullet_state;
	const BulletState get_bullet_state() { return bullet_state; }
	Point get_size() const { return Point(graph_w, graph_h); }
	void set_adjust_speed(double dx, double dy) { adjust_speed_x += dx; adjust_speed_y += dy; }
protected:
	Point dir_to_vector(const Dir dir);
	int v;
	int attack_cd;
	int graph_h;
	int graph_w;
	double adjust_speed_x = 0.0;
    double adjust_speed_y = 0.0;
	std::vector<std::vector<int>> bitmap_img_ids;
	int bitmap_switch_counter;
	int bitmap_switch_freq;
	int bitmap_img_id;
	Action action;
	Dir dir;
	MonsterType type;
	Point size;
	Circle force_shape;
private:
	Dir convert_dir(const Point &v);
	void virtual attack();
	Point hero_tracker();
	Point position;
	clock_t timer;
	clock_t cd_timer;
	int action_timer[4];
	bool need_effect;
	bool can_attack;
	bool is_collid;
	double dist_to_hero;
};

#endif
