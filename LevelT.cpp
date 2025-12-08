#include "LevelT.h"
#include "Utils.h"
#include "Hero.h"
#include "monsters/MonsterT.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "Camera.h"
#include "shapes/Point.h"
#include "shapes/Rectangle.h"
#include "towers/Block.h"
#include <iostream>

using namespace std;

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
	is_puzzle_solved = false;
	is_monster_spawn = false;
	is_monster_dead = false;
	spawn_position = Point(LevelSetting::monster_spawn_x[lvl-1], DC->window_height / 2);
	// setup puzzle
}

/**
 * @brief Updates monster_spawn_counter and create monster if needed.
*/
void LevelT::update1() {
	DataCenter *DC = DataCenter::get_instance();
	DC->hero->change_skill_state(SkillState::SLG);
	if (!is_puzzle_solved) {
		if(!block_timer){
			bool repeat = true;
			int rand_state = distribution(generator) % 3;
			int rand_x = distribution(generator) % 9;
			while(repeat){
				repeat = false;
				for(int x : recent_x_history){
					if(rand_x == x){
						repeat = true;
						rand_x = distribution(generator) % 9;
						break;
					}
				}
			}
			recent_x_history.push_back(rand_x);
			if (recent_x_history.size() > 5) {
				recent_x_history.pop_front();
			}
			int rand_y = rand_x % 2 ? 0 : 1280;
			Block *block = new Block(Point(block_x[rand_x], rand_y), (rand_y ? -1 : 1) * (160), static_cast<BlockState>(rand_state));//+50*rand_x
			DC->blocks.push_back(block);
			std::cout << "create block" << std::endl;
			block_timer = 4;
		}else{
			block_timer--;
		}
		// is_puzzle_solved = true;
	}
	if (DC->get_instance()->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (DC->hero->shape->center_x() > LevelSetting::puzzle_bound_x[0] && !is_monster_spawn) {
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
	}
	if (DC->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (DC->hero->shape->center_x() > LevelSetting::puzzle_bound_x[1] && !is_monster_spawn) {
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
	if (!is_puzzle_solved) { // 解謎遊戲還沒完成
		// update puzzle
		is_puzzle_solved = true; // 會決定角色是否獲得技能
	} 
	if (DC->monster != nullptr || !is_monster_spawn) { // 怪獸還沒死(已經生成) 或 怪獸還沒生成(還沒離開解謎區)
		// update arena
		if (DC->hero->shape->center_x() > LevelSetting::puzzle_bound_x[2] && !is_monster_spawn) {
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
	}
	if (DC->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (DC->hero->shape->center_x() > LevelSetting::puzzle_bound_x[3] && !is_monster_spawn) {
			// init monster for this level
			DC->monster = MonsterT::create_monster(MonsterType::MONSTER4, this->spawn_position);
			is_monster_spawn = true;
		}
	} else {
		is_monster_dead = true;
	}
}

void LevelT::draw() {
	DataCenter *DC = DataCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	Point p;
	if(level == -1) return;

	for (size_t i = 0; i < 4; ++i) {
		if (DC->hero->shape->center_x() > LevelSetting::lvl_bound_x[i] && DC->hero->shape->center_x() < LevelSetting::puzzle_bound_x[i]) {
			background = IC->get(LevelSetting::lvl_background_path[i]);
			al_draw_bitmap(IC->get(LevelSetting::tmp_background_path),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_y(), 0);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_y(), 0);
		} else if (DC->hero->shape->center_x() >= LevelSetting::puzzle_bound_x[i] && DC->hero->shape->center_x() < LevelSetting::lvl_bound_x[i+1] - 640) {
			background = IC->get(LevelSetting::lvl_background_path[i]);
			al_draw_bitmap(IC->get(LevelSetting::tmp_background_path),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1]-640, 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1]-640, 0).center_y(), 0);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_y(), 0);
		} else if (DC->hero->shape->center_x() >= LevelSetting::lvl_bound_x[i+1] - 640 && DC->hero->shape->center_x() < LevelSetting::lvl_bound_x[i+1]) {
			background = IC->get(LevelSetting::lvl_background_path[i]);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_y(), 0);
			al_draw_bitmap(IC->get(LevelSetting::tmp_background_path),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1]-640, 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1]-640, 0).center_y(), 0);
			background = IC->get(LevelSetting::lvl_background_path[i+1]);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1], 0).center_y(), 0);
		}
	}

	switch (level) {
	case 1:
		// draw puzzle items
		// draw arena items
		break;
	case 2:
		// draw puzzle items
		// draw arena items
		break;
	case 3:
		// draw puzzle items
		// draw arena items
		break;
	case 4:
		// draw puzzle items
		// draw arena items
		break;
	default:
		break;
	}
}