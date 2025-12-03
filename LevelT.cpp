#include "LevelT.h"
#include <string>
#include "Utils.h"
#include "Hero.h"
#include "monsters/MonsterT.h"
#include "data/DataCenter.h"
#include "Camera.h"
#include <allegro5/allegro_primitives.h>
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include <array>
#include <iostream>

using namespace std;

// fixed settings
namespace LevelSetting {
	constexpr char level_path_format[] = "./assets/level/LEVEL%d.txt";
	//! @brief Grid size for each level.
	constexpr array<int, 4> grid_size = {
		40, 40, 40, 40
	};
	constexpr int monster_spawn_rate = 90;
};

void
LevelT::init() {
	level = -1;
	grid_w = -1;
	grid_h = -1;
	// monster_spawn_counter = 0;
}

/**
 * @brief Loads level data from input file. The input file is required to follow the format.
 * @param lvl level index. The path format is a fixed setting in code.
 * @details The content of the input file should be formatted as follows:
 *          * Total number of monsters.
 *          * Number of each different number of monsters. The order and number follows the definition of MonsterType.
 *          * Indefinite number of Point (x, y), represented in grid format.
 * @see level_path_format
 * @see MonsterType
 */
void LevelT::load_level(int lvl) {
	DataCenter *DC = DataCenter::get_instance();
	char buffer[50];
	sprintf(buffer, LevelSetting::level_path_format, lvl);
	FILE *f = fopen(buffer, "r");
	GAME_ASSERT(f != nullptr, "cannot find level.");
	level = lvl;
	// grid_w = DC->game_field_length / LevelSetting::grid_size[lvl];
	// grid_h = DC->game_field_length / LevelSetting::grid_size[lvl];
	is_puzzle_solved = false;
	is_monster_spawn = false;
	is_monster_dead = false;
	arena_entry = DC->game_field_length / 2.0;
	spawn_position = Point(DC->game_field_length, DC->game_field_length);
	// setup puzzle
}

/**
 * @brief Updates monster_spawn_counter and create monster if needed.
*/
void LevelT::update1() {
	DataCenter *DC = DataCenter::get_instance();
	if (!is_puzzle_solved) {
		// update puzzle
		is_puzzle_solved = true;
	} else if (DC->get_instance()->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (DC->hero->shape->center_x() > this->arena_entry && !is_monster_spawn) {
			// init monster for this level
			// cout << "create monster" << endl;
			DC->monster = MonsterT::create_monster(MonsterType::MONSTER1, this->spawn_position);
			is_monster_spawn = true;
		}
	} else {
		// cout << "dead" << endl;
		is_monster_dead = true;
	}
}

void LevelT::update2() {
	DataCenter *DC = DataCenter::get_instance();
	if (!is_puzzle_solved) {
		// update puzzle
		is_puzzle_solved = true;
	} else if (DC->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (DC->hero->shape->center_x() > this->arena_entry && !is_monster_spawn) {
			// init monster for this level
			DC->monster = MonsterT::create_monster(MonsterType::MONSTER2, this->spawn_position);
			is_monster_spawn = true;
		}
	} else {
		is_monster_dead = true;
	}
}

void LevelT::update3() {
	DataCenter *DC = DataCenter::get_instance();
	if (!is_puzzle_solved) {
		// update puzzle
		is_puzzle_solved = true;
	} else if (DC->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (DC->hero->shape->center_x() > this->arena_entry && !is_monster_spawn) {
			// init monster for this level
			DC->monster = MonsterT::create_monster(MonsterType::MONSTER3, this->spawn_position);
			is_monster_spawn = true;
		}
	} else {
		is_monster_dead = true;
	}
}

void LevelT::update4() {
	DataCenter *DC = DataCenter::get_instance();
	if (!is_puzzle_solved) {
		// update puzzle
		is_puzzle_solved = true;
	} else if (DC->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (DC->hero->shape->center_x() > this->arena_entry && !is_monster_spawn) {
			// init monster for this level
			DC->monster = MonsterT::create_monster(MonsterType::MONSTER4, this->spawn_position);
			is_monster_spawn = true;
		}
	} else {
		is_monster_dead = true;
	}
}

void LevelT::draw() {
	// DataCenter *DC = DataCenter::get_instance();
	Point p;
	if(level == -1) return;
	switch (level) {
	case 1:
		// draw puzzle items
		// draw background
		// draw arena items
		break;
	case 2:
		// draw puzzle items
		// draw background
		// draw arena items
		break;
	case 3:
		// draw puzzle items
		// draw background
		// draw arena items
		break;
	case 4:
		// draw puzzle items
		// draw background
		// draw arena items
		break;
	default:
		break;
	}
}