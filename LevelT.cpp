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
	is_level_start = false;
	spawn_position = Point(LevelSetting::monster_spawn_x[lvl-1], DC->window_height / 2);
	// setup puzzle
	if (level == 2)
		shoot_cd = 1;
	else
		shoot_cd = 1000;
}

void LevelT::shooter(Point shooter_pt, Point shoot_vec, BulletState bullet_type) {
	DataCenter *DC = DataCenter::get_instance();
	Bullet *b;
	b = new Bullet(shooter_pt, shoot_vec, BulletSetting::bullet_paths[static_cast<int>(bullet_type)], 480, 0, 10000, bullet_type);
	if (bullet_type == BulletState::LASER)
		DC->waveBullets.emplace_back(b);
	else if (bullet_type == BulletState::NEGATIVE || bullet_type == BulletState::POSITIVE)
		DC->electrodeBullets.emplace_back(b);
	else
		DC->bullets.emplace_back(b);
}
/**
 * @brief Updates monster_spawn_counter and create monster if needed.
*/
void LevelT::update1() {
	DataCenter *DC = DataCenter::get_instance();

	if (DC->hero->shape->center_x() > LevelSetting::lvl_bound_x[this->level-1] + DC->hero->get_size().center_x() / 2.0 && !is_monster_dead && !is_level_start) {
		is_level_start = true;
		DC->hero->change_skill_state(SkillState::SLG);
	} else if (!is_level_start) {
		DC->hero->change_skill_state(SkillState::NORMAL);
	}
        
	if (!is_puzzle_solved) {
		if(!block_timer){
			bool repeat = true;
			int rand_state = distribution(generator) % 3;
			int rand_x = distribution(generator) % 16;
			while(repeat){
				repeat = false;
				for(int x : recent_x_history){
					if(rand_x == x){
						repeat = true;
						rand_x = distribution(generator) % 16;
						break;
					}
				}
			}
			recent_x_history.push_back(rand_x);
			if (recent_x_history.size() > 11) {
				recent_x_history.pop_front();
			}
			int rand_y = rand_x % 2 ? 0 : 800;
			Block *block = new Block(Point(block_x[rand_x], rand_y), (rand_y ? -1 : 1) * (80), static_cast<BlockState>(rand_state));//+50*rand_x
			DC->blocks.push_back(block);
			// std::cout << "create block" << std::endl;
			block_timer = 4;
		}else{
			block_timer--;
		}
		// is_puzzle_solved = true;
	}
	if (DC->key_state[ALLEGRO_KEY_L] && !DC->prev_key_state[ALLEGRO_KEY_L] && !is_puzzle_solved) {
		is_puzzle_solved = true;
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
		is_level_start = false;
	}
}

void LevelT::update2() {
	DataCenter *DC = DataCenter::get_instance();
	shooter_pt.update_center_x(LevelSetting::lvl_bound_x[this->level-1]+300);
	shooter_pt.update_center_y(340);
	receive_pt.update_center_x(LevelSetting::lvl_bound_x[this->level-1]+1200);
	receive_pt.update_center_y(int(DC->window_height * 0.7));
	Point shoot_vec(1, 0);
	if (DC->hero->shape->center_x() > LevelSetting::lvl_bound_x[this->level-1] + DC->hero->get_size().center_x() / 2.0 && !is_monster_dead && !is_level_start) {
		is_level_start = true;
		DC->hero->change_skill_state(SkillState::WAVE);
	} else if (!is_level_start) {
		DC->hero->change_skill_state(SkillState::NORMAL);
	}
	// std::cout << "a";
	if (!is_puzzle_solved) {
		// update puzzle
		// 發射雷射
		if (int(clock() - this->cd_timer) >= shoot_cd) {
			// cout << "can attack" << endl;
			shooter(shooter_pt, shoot_vec, BulletState::LASER);
			this->cd_timer = clock();
			// std::cout << "b";
		}
		// 判斷雷射有沒有打到接收器
		std::vector<Bullet*> &wave_bullets = DC->waveBullets;
		for (size_t i = 0; i < wave_bullets.size(); ++i) {
			Point bullet_pt(wave_bullets[i]->shape->center_x(), wave_bullets[i]->shape->center_y());
			if (Point::dist(bullet_pt, receive_pt) < 40) {
				is_puzzle_solved = true;
				DC->hero->change_skill_state(SkillState::WAVE);
			}
		}
	}
	if (DC->key_state[ALLEGRO_KEY_L] && !DC->prev_key_state[ALLEGRO_KEY_L] && !is_puzzle_solved) {
		is_puzzle_solved = true;
		DC->hero->change_skill_state(SkillState::WAVE);
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
		is_level_start = false;
	}
}

void LevelT::update3() {
	DataCenter *DC = DataCenter::get_instance();
	shooter_pt.update_center_x(LevelSetting::lvl_bound_x[this->level-1]+150);
	shooter_pt.update_center_y(347);
	receive_pt.update_center_x(LevelSetting::lvl_bound_x[this->level-1]+1000);
	receive_pt.update_center_y(int(DC->window_height * 0.7));
	Point shoot_vec(1, 0);
	if (DC->hero->shape->center_x() > LevelSetting::lvl_bound_x[this->level-1] + DC->hero->get_size().center_x() / 2.0 && !is_monster_dead && !is_level_start) {
		is_level_start = true;
		DC->hero->change_skill_state(SkillState::ELECTRIC);
	} else if (!is_level_start) {
		DC->hero->change_skill_state(SkillState::NORMAL);
	}
	if (!is_puzzle_solved) { // 解謎遊戲還沒完成
		// update puzzle
		if (int(clock() - this->cd_timer) >= shoot_cd) {
			// cout << "can attack" << endl;
			shooter(shooter_pt, shoot_vec, BulletState::NEGATIVE);
			this->cd_timer = clock();
		}
		// 判斷電荷有沒有打到接收器
		std::vector<Bullet*> &electrode_bullets = DC->electrodeBullets;
		for (size_t i = 0; i < electrode_bullets.size(); ++i) {
			Point bullet_pt(electrode_bullets[i]->shape->center_x(), electrode_bullets[i]->shape->center_y());
			if (Point::dist(bullet_pt, receive_pt) < 40) {
				is_puzzle_solved = true;
				DC->hero->change_skill_state(SkillState::ELECTRIC);
			}
		}
	}
	if (DC->key_state[ALLEGRO_KEY_L] && !DC->prev_key_state[ALLEGRO_KEY_L] && !is_puzzle_solved) {
		is_puzzle_solved = true;
		DC->hero->change_skill_state(SkillState::ELECTRIC);
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
		is_level_start = false;
	}
}

void LevelT::update4() {
	DataCenter *DC = DataCenter::get_instance();
	if (DC->hero->shape->center_x() > LevelSetting::lvl_bound_x[this->level-1] + DC->hero->get_size().center_x() / 2.0 && !is_monster_dead && !is_level_start) {
		DC->hero->all_skill = true;
		is_level_start = true;
	} else if (!is_level_start) {
		DC->hero->change_skill_state(SkillState::NORMAL);
	}
	if (DC->monster != nullptr || !is_monster_spawn) {
		// update arena
		if (is_level_start && !is_monster_spawn) {
			// init monster for this level
			DC->monster = MonsterT::create_monster(MonsterType::MONSTER4, this->spawn_position);
			is_monster_spawn = true;
		}
	} else {
		is_monster_dead = true;
		is_level_start = false;
	}
}

void LevelT::draw() {
	DataCenter *DC = DataCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	Point p;
	if(level == -1) return;

	for (size_t i = 0; i < 4; ++i) {
		if(DC->hero->shape->center_x() < LevelSetting::lvl_bound_x[i]){
			al_draw_bitmap(IC->get(LevelSetting::buffer_background_path[i]),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_y(), 0);
			background = IC->get(LevelSetting::lvl_background_path[i]);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_y(), 0);

			ALLEGRO_COLOR tint_color = al_map_rgba_f(0.5,0.5,0.5,0.5);
			ALLEGRO_BITMAP *instruction = IC->get(LevelSetting::instruction_path[0]);
			al_draw_tinted_bitmap(instruction, tint_color,
				DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-960, 0).center_x(),
				DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-960, 0).center_y(), 0);
		}
		else if (DC->hero->shape->center_x() >= LevelSetting::lvl_bound_x[i] && DC->hero->shape->center_x() < LevelSetting::puzzle_bound_x[i]) {
			background = IC->get(LevelSetting::lvl_background_path[i]);
			al_draw_bitmap(IC->get(LevelSetting::buffer_background_path[i]),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_y(), 0);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_y(), 0);
		} else if (DC->hero->shape->center_x() >= LevelSetting::puzzle_bound_x[i] && DC->hero->shape->center_x() < LevelSetting::lvl_bound_x[i+1] - 640) {
			background = IC->get(LevelSetting::lvl_background_path[i]);
			al_draw_bitmap(IC->get(LevelSetting::buffer_background_path[i+1]),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1]-640, 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i+1]-640, 0).center_y(), 0);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_y(), 0);
			if (i == 3) {
				al_draw_bitmap(IC->get(LevelSetting::buffer_background_path[i]),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i]-640, 0).center_y(), 0);
			}
		} else if (DC->hero->shape->center_x() >= LevelSetting::lvl_bound_x[i+1] - 640 && DC->hero->shape->center_x() < LevelSetting::lvl_bound_x[i+1]) {
			background = IC->get(LevelSetting::lvl_background_path[i]);
			al_draw_bitmap(background,
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_x(),
						DC->camera->transform_bitmap(LevelSetting::lvl_bound_x[i], 0).center_y(), 0);
			al_draw_bitmap(IC->get(LevelSetting::buffer_background_path[i+1]),
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
	case 2: {
		// draw puzzle items
		// std::cout << "draw device";
		// Point offset = DC->camera->transform_bitmap(shooter_pt.center_x(), shooter_pt.center_y());
		Point offset1 = DC->camera->transform_bitmap(receive_pt.center_x(), receive_pt.center_y());
		// std::string path = "./assets/image/block/wall_shoot.png";
		std::string path1 = "./assets/image/block/wall_receive.png";
		// ALLEGRO_BITMAP *bitmap = IC->get(path);
		ALLEGRO_BITMAP *bitmap1 = IC->get(path1);
		// al_draw_bitmap(bitmap, offset.center_x() - al_get_bitmap_width(bitmap) / 2.0, offset.center_y() - al_get_bitmap_height(bitmap) / 2.0, 0);
		al_draw_bitmap(bitmap1, offset1.center_x() - al_get_bitmap_width(bitmap1) / 2.0, offset1.center_y() - al_get_bitmap_height(bitmap1) / 2.0, 0);
		// draw arena items
		break;
	}
	case 3: {
		// draw puzzle items
		// Point offset = DC->camera->transform_bitmap(shooter_pt.center_x(), shooter_pt.center_y());
		Point offset1 = DC->camera->transform_bitmap(receive_pt.center_x(), receive_pt.center_y());
		// std::string path = "./assets/image/block/wall_shoot.png";
		std::string path1 = "./assets/image/block/wall_receive.png";
		// ALLEGRO_BITMAP *bitmap = IC->get(path);
		ALLEGRO_BITMAP *bitmap1 = IC->get(path1);
		// al_draw_bitmap(bitmap, offset.center_x() - al_get_bitmap_width(bitmap) / 2.0, offset.center_y() - al_get_bitmap_height(bitmap) / 2.0, 0);
		al_draw_bitmap(bitmap1, offset1.center_x() - al_get_bitmap_width(bitmap1) / 2.0, offset1.center_y() - al_get_bitmap_height(bitmap1) / 2.0, 0);
		// draw arena items
		break;
	}
	case 4:
		// draw puzzle items
		// draw arena items
		break;
	default:
		break;
	}
}