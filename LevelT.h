#ifndef LEVELT_H_INCLUDED
#define LEVELT_H_INCLUDED

#include <vector>
#include <utility>
#include <tuple>
#include "./shapes/Point.h"
#include "./shapes/Rectangle.h"
#include <allegro5/allegro_primitives.h>
#include <array>
#include <string>

// fixed settings
namespace LevelSetting {
	constexpr char level_path_format[] = "./assets/level/LEVEL%d.txt";
	//! @brief Grid size for each level.
	constexpr int grid_size[4] = {
		40, 40, 40, 40
	};
	constexpr int monster_spawn_rate = 90;
	constexpr int hero_spawn_x[4] = {
		640, 3840, 7040, 10240
	};
	constexpr int puzzle_bound_x[4] = {
		1280, 4480, 7680, 10800
	};
	constexpr int monster_spawn_x[4] = {
		1920, 5120, 8320, 11520
	};
	constexpr int lvl_bound_x[5] = {
		0, 3200, 6400, 9600, 12800
	};
	const std::array<std::string, 4> lvl_background_path = {
		"./assets/image/lvl_background.png",
		"./assets/image/lvl_background.png",
		"./assets/image/lvl_background.png",
		"./assets/image/lvl_background.png"
	};
	const std::string tmp_background_path = "./assets/image/tmp_background.png";
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
private:
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
};

#endif
