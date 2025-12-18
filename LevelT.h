#ifndef LEVELT_H_INCLUDED
#define LEVELT_H_INCLUDED

#include <vector>
#include <utility>
#include <tuple>
#include "./shapes/Point.h"
#include "./shapes/Rectangle.h"
#include "./towers/Bullet.h"
#include <allegro5/allegro_primitives.h>
#include <array>
#include <string>
#include <random>
#include <deque>

// fixed settings
namespace LevelSetting {
	constexpr char level_path_format[] = "./assets/level/LEVEL%d.txt";
	//! @brief Grid size for each level.
	constexpr int grid_size[4] = {
		40, 40, 40, 40
	};
	constexpr int monster_spawn_rate = 90;
	constexpr int hero_spawn_x[4] = {
		-320, 4705, 7828, 10837
	};
	constexpr int puzzle_bound_x[4] = {
		2816, 5895, 8858, 10777 //1280./
	};
	constexpr int monster_spawn_x[4] = {
		3476, 6495, 9458, 11377
	};
	constexpr int lvl_bound_x[5] = {
		0, 4645, 7768, 10777, 12917
	};
	const std::array<std::string, 4> lvl_background_path = {
		"./assets/image/background/lv1.png", // "./assets/image/background/lv1.png"
		"./assets/image/background/lv2.png", 
		"./assets/image/background/lv3.png",
		"./assets/image/background/lv4.png"
	};
	const std::array<std::string, 5> buffer_background_path = {
		"./assets/image/background/lv1_buffer.png",
		"./assets/image/background/lv2_buffer.png", 
		"./assets/image/background/lv3_buffer.png",
		"./assets/image/background/lv4_buffer.png",
		"./assets/image/background/lv4_buffer1.png"
	};
	const std::array<std::string, 4> instruction_path = {
		"./assets/image/instruction/lv1_instruction.png",
		"./assets/image/instruction/lv1_instruction.png", 
		"./assets/image/instruction/lv1_instruction.png",
		"./assets/image/instruction/lv1_instruction.png"
	};
};

/**
 * @brief The class manages data of each level.
 * @details The class could load level with designated input file and record. The level itself will decide when to create next monster.
 * @see DataCenter::level
 */
class LevelT{
public:
	LevelT() {}
	void init();
	void load_level(int lvl);
	void update1();
	void update2();
	void update3();
	void update4();
	void draw();
	// const std::vector<Point> &get_road_path() const
	// { return road_path; }
	// int remain_monsters() const {
	// 	int res = 0;
	// 	for(const int &i : num_of_monsters) res += i;
	// 	return res;
	// }
	bool get_puzzle_solved() { return is_puzzle_solved; }
	bool get_monster_dead() { return is_monster_dead; }
	bool get_monster_spawn() { return is_monster_spawn; }
	bool get_level_start() { return is_level_start; }
	int get_level() { return level; }
private:
	void shooter(Point shooter_pt, Point shoot_vec, BulletState bullet_type);
	/**
	 * @brief The index of current level.
	 */
	int level;
	/**
	 * @brief Number of grid in x-direction.
	 */
	int grid_w;
	/**
	 * @brief Number of grid in y-direction.
	 */
	int grid_h;
	Point spawn_position;
	ALLEGRO_BITMAP *background;
	bool is_puzzle_solved;
	bool is_monster_dead;
	bool is_monster_spawn;
	bool is_level_start;
	int block_timer = 0;
	clock_t cd_timer;
	int shoot_cd;
	std::mt19937 generator;
	std::uniform_int_distribution<int> distribution{1, 30};
	int block_x[16] = {176, 396, 616, 836, 1056, 1144, 1364, 1452, 1672, 1760, 1980, 2068, 2156, 2376, 2464, 2552} ;//68
	std::deque<int> recent_x_history;
	Point shooter_pt;
	Point receive_pt;
};

#endif
