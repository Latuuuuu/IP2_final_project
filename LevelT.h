#ifndef LEVELT_H_INCLUDED
#define LEVELT_H_INCLUDED

#include <vector>
#include <utility>
#include <tuple>
#include "./shapes/Point.h"
#include "./shapes/Rectangle.h"

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
	int arena_entry;
	Point spawn_position;
	bool is_puzzle_solved;
	bool is_monster_dead;
	bool is_monster_spawn;
};

#endif
