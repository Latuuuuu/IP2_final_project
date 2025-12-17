#include "OperationCenter.h"
#include "../algif5/algif.h"
#include "GIFCenter.h"
#include "DataCenter.h"
#include "../monsters/MonsterT.h"
#include "../towers/Bullet.h"
#include "../towers/Block.h"
#include "../Camera.h"
#include "../Hero.h"
#include "../physics.hpp"
#include <iostream>

void OperationCenter::update() {
	// Update monsters.
	// std::cout << 2 << std::endl;
	_update_monster();
	// Update tower bullets.
	// std::cout << 3 << std::endl;
	_update_bullet();
	// Update tools.
	// std::cout << 4 << std::endl;
	_update_tool();
	// If any bullet overlaps with any monster, we delete the bullet, reduce the HP of the monster, and delete the monster if necessary.
	// std::cout << 5 << std::endl;
	_update_monster_bullet();
	// std::cout << 6 << std::endl;
	_update_hero_bullet();
	// If any monster hits hero, monster dies.
	// std::cout << 7 << std::endl;
	_update_monster_hero();

	// std::cout << 8 << std::endl;
	_update_block();
	// std::cout << 9 << std::endl;
	_update_hero_block();
	// std::cout << 10 << std::endl;
	_update_bullet_block();

	// std::cout << 11 << std::endl;
	_update_bullet_bullet();
	// std::cout << 12 << std::endl;
	_update_tool_bullet();
	// std::cout << 13 << std::endl;
	_update_tool_others();
}

void OperationCenter::_update_monster() {
	MonsterT *monster = DataCenter::get_instance()->monster;
	if (monster != nullptr)
		monster->update();
}

void OperationCenter::_update_bullet() {
	std::vector<Bullet*> &bullets = DataCenter::get_instance()->bullets;
	std::vector<Bullet*> &matter_bullets = DataCenter::get_instance()->matterBullets;
	std::vector<Bullet*> &wave_bullets = DataCenter::get_instance()->waveBullets;
	std::vector<Bullet*> &electrode_bullets = DataCenter::get_instance()->electrodeBullets;
	for(Bullet *bullet : bullets)
		bullet->update();
	for(Bullet *bullet : matter_bullets)
		bullet->update();
	for(Bullet *bullet : wave_bullets)
		bullet->update();
	for(Bullet *bullet : electrode_bullets)
		bullet->update();
	// Detect if a bullet flies too far (exceeds its fly distance limit), which means the bullet lifecycle has ended.
	for(size_t i = 0; i < bullets.size(); ++i) {
		if(bullets[i]->get_fly_dist() <= 0 || !bullets[i]->alive) {
			delete bullets[i];
			bullets.erase(bullets.begin() + i);
			--i;
		}
	}
	for(size_t i = 0; i < matter_bullets.size(); ++i) {
		if(matter_bullets[i]->get_fly_dist() <= 0 || !matter_bullets[i]->alive) {
			delete matter_bullets[i];
			matter_bullets.erase(matter_bullets.begin() + i);
			--i;
		}
	}
	for(size_t i = 0; i < wave_bullets.size(); ++i) {
		if(wave_bullets[i]->get_fly_dist() <= 0 || !wave_bullets[i]->alive) {
			delete wave_bullets[i];
			wave_bullets.erase(wave_bullets.begin() + i);
			--i;
		}
	}
	for(size_t i = 0; i < electrode_bullets.size(); ++i) {
		if(electrode_bullets[i]->get_fly_dist() <= 0 || !electrode_bullets[i]->alive) {
			delete electrode_bullets[i];
			electrode_bullets.erase(electrode_bullets.begin() + i);
			--i;
		}
	}
}

void OperationCenter::_update_tool() {
	std::vector<Tool*> &tools = DataCenter::get_instance()->tools;
	int max_tool = DataCenter::get_instance()->hero->get_max_tool_num();
	for(Tool *tool : tools)
		tool->update();
	if ((int)tools.size() > max_tool) { // pop out oldest tool if there's too many tools
		delete tools[0];
		tools.erase(tools.begin());
	}
}

void OperationCenter::_update_block() {
	std::vector<Block*> &blocks = DataCenter::get_instance()->blocks;
	for(size_t j = 0; j < blocks.size(); ++j){
		if(!blocks[j]->alive){
			delete blocks[j];
			blocks.erase(blocks.begin() + j);
			continue;
		}
		else{
			blocks[j]->update();
		}
	}
}

void OperationCenter::_update_monster_bullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Bullet*> &bullets = DC->bullets;
	std::vector<Bullet*> &matter_bullets = DC->matterBullets;
	std::vector<Bullet*> &wave_bullets = DC->waveBullets;
	std::vector<Bullet*> &electrode_bullets = DC->electrodeBullets;
	if (DC->monster == nullptr) return;
	for (size_t j = 0; j < bullets.size(); ++j) {
		// Check if the bullet overlaps with the hero.
		if (DC->monster->shape->overlap(*(bullets[j]->shape))) {
			// Reduce the HP of the monster. Delete the bullet.
			DC->monster->HP -= bullets[j]->get_dmg();
			delete bullets[j];
			bullets.erase(bullets.begin() + j);
			--j;
		}
	}
	for (size_t j = 0; j < matter_bullets.size(); ++j) {
		// Check if the bullet overlaps with the monster.
		if (DC->monster->shape->overlap(*(matter_bullets[j]->shape))) {
			// Reduce the HP of the monster. Delete the bullet.
			matter_bullets[j]->update_matter(DC->monster->get_bullet_state());
			DC->monster->HP -= matter_bullets[j]->get_dmg();
			delete matter_bullets[j];
			matter_bullets.erase(matter_bullets.begin() + j);
			--j;
		}
		if (DC->monster->HP <= 0) {
			DC->monster = nullptr;
			return;
		}
	}
	for (size_t j = 0; j < wave_bullets.size(); ++j) {
		// Check if the bullet overlaps with the monster.
		if (DC->monster->shape->overlap(*(wave_bullets[j]->shape)) && wave_bullets[j]->get_state() == BulletState::LASER) {
			// Reduce the HP of the monster. Delete the bullet.
			DC->monster->HP -= wave_bullets[j]->get_dmg();
			delete wave_bullets[j];
			wave_bullets.erase(wave_bullets.begin() + j);
			--j;
		}
		if (DC->monster->HP <= 0) {
			DC->monster = nullptr;
			return;
		}
	}
	if (DC->monster->type != MonsterType::MONSTER3 && DC->monster->type != MonsterType::MONSTER4)
		return;
	for (size_t j = 0; j < electrode_bullets.size(); ++j) {
		// Check if the bullet overlaps with the hero.
		if (DC->monster->force_shape.overlap(electrode_bullets[j]->force_shape)) {
			std::pair<std::pair<double,double>,std::pair<double,double>> v = coulombs_law(6, 1, *DC->monster->shape, *electrode_bullets[j]->shape, DC->monster->e, electrode_bullets[j]->e);
			DC->monster->set_adjust_speed(v.first.first, v.first.second);
			electrode_bullets[j]->set_adjust_speed(v.second.first, v.second.second);
		}
		if (DC->monster->shape->overlap(*(electrode_bullets[j]->shape)) &&
			electrode_bullets[j]->update_electrode(DC->monster->get_bullet_state())) {
			// Reduce the HP of the hero. Delete the bullet.
			DC->monster->HP -= electrode_bullets[j]->get_dmg();
		}
		if (DC->monster->HP <= 0) {
			DC->monster = nullptr;
			return;
		}
	}
}

void OperationCenter::_update_hero_bullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Bullet*> &bullets = DC->bullets;
	std::vector<Bullet*> &matter_bullets = DC->matterBullets;
	std::vector<Bullet*> &wave_bullets = DC->waveBullets;
	std::vector<Bullet*> &electrode_bullets = DC->electrodeBullets;
	if (DC->hero == nullptr) return;
	for (size_t j = 0; j < bullets.size(); ++j) {
		// Check if the bullet overlaps with the hero.
		if (DC->hero->shape->overlap(*(bullets[j]->shape))) {
			// Reduce the HP of the hero. Delete the bullet.
			DC->hero->HP -= bullets[j]->get_dmg();
			delete bullets[j];
			bullets.erase(bullets.begin() + j);
			--j;
		}
	}
	for (size_t j = 0; j < matter_bullets.size(); ++j) {
		// Check if the bullet overlaps with the hero.
		if (DC->hero->shape->overlap(*(matter_bullets[j]->shape))) {
			// Reduce the HP of the hero. Delete the bullet.
			matter_bullets[j]->update_matter(DC->hero->get_bullet_state());
			DC->hero->HP -= matter_bullets[j]->get_dmg();
			delete matter_bullets[j];
			matter_bullets.erase(matter_bullets.begin() + j);
			--j;
		}
	}
	for (size_t j = 0; j < wave_bullets.size(); ++j) {
		// Check if the bullet overlaps with the hero.
		if (DC->hero->shape->overlap(*(wave_bullets[j]->shape)) && wave_bullets[j]->get_state() == BulletState::LASER) {
			// Reduce the HP of the hero. Delete the bullet.
			DC->hero->HP -= wave_bullets[j]->get_dmg();
			delete wave_bullets[j];
			wave_bullets.erase(wave_bullets.begin() + j);
			--j;
		}
	}
	for (size_t j = 0; j < electrode_bullets.size(); ++j) {
		// Check if the bullet overlaps with the hero.
		if (DC->hero->force_shape.overlap(electrode_bullets[j]->force_shape)) {
			// std::cout << "h e f" << std::endl;
			std::pair<std::pair<double,double>,std::pair<double,double>> v = coulombs_law(3, 1, *DC->hero->shape, *electrode_bullets[j]->shape, DC->hero->e, electrode_bullets[j]->e);
			DC->hero->set_adjust_speed(v.first.first, v.first.second);
			electrode_bullets[j]->set_adjust_speed(v.second.first, v.second.second);
		}
		if (DC->hero->shape->overlap(*(electrode_bullets[j]->shape)) &&
			electrode_bullets[j]->update_electrode(DC->hero->get_bullet_state())) {
			// std::cout << "h e c" << std::endl;
			// Reduce the HP of the hero. Delete the bullet.
			DC->hero->HP -= electrode_bullets[j]->get_dmg();
		}
	}
}

void OperationCenter::_update_bullet_block() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Block*> &blocks = DC->blocks;
	std::vector<Bullet*> &bullets = DC->bullets;
	std::vector<Bullet*> &matter_bullets = DC->matterBullets;
	// std::vector<Bullet*> &wave_bullets = DC->waveBullets;
	std::vector<Bullet*> &electrode_bullets = DC->electrodeBullets;
	for (size_t i = 0; i < bullets.size(); ++i) {
		for (size_t j = 0; j < blocks.size(); ++j) {
			// Check if the bullet overlaps with the monster.
			if (bullets[i]->shape->overlap(*(blocks[j]->shape))) {
				// Reduce the HP of the monster. Delete the bullet.
				blocks[j]->update_bullet_hit(bullets[i]->get_state(), bullets[i]);
				break;
			}
		}
	}
	for (size_t i = 0; i < matter_bullets.size(); ++i) {
		for (size_t j = 0; j < blocks.size(); ++j) {
			// Check if the bullet overlaps with the monster.
			if (matter_bullets[i]->shape->overlap(*(blocks[j]->shape))) {
				// Reduce the HP of the monster. Delete the bullet.
				blocks[j]->update_bullet_hit(matter_bullets[i]->get_state(), matter_bullets[i]);
				break;
			}
		}
	}
	for (size_t i = 0; i < electrode_bullets.size(); ++i) {
		for (size_t j = 0; j < blocks.size(); ++j) {
			// Check if the bullet overlaps with the monster.
			if (electrode_bullets[i]->shape->overlap(*(blocks[j]->shape))) {
				// Reduce the HP of the monster. Delete the bullet.
				blocks[j]->update_bullet_hit(electrode_bullets[i]->get_state(), electrode_bullets[i]);
				break;
			}
		}
	}
}

void OperationCenter::_update_hero_block() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Block*> &blocks = DC->blocks;
	if (DC->hero == nullptr) return;
	for (size_t j = 0; j < blocks.size(); ++j) {
		// Check if the bullet overlaps with the monster.
		if (DC->hero->shape->overlap(*(blocks[j]->shape))) {
			// Reduce the HP of the monster. Delete the bullet.
			blocks[j]->update_hero_hit(DC->hero->bullet_state);
		}
	}
}

void OperationCenter::_update_monster_hero() {
	DataCenter *DC = DataCenter::get_instance();
	if (DC->monster == nullptr)
		return;
	if(DC->monster->shape->overlap(*(DC->hero->shape))){
		// std::cout << "monster&hero collid" << std::endl;
		DC->hero->is_collid = true;
		DC->monster->is_collid = true;
		DC->hero->jump_back(Point(DC->monster->shape->center_x(), 
								  DC->monster->shape->center_y()));
		DC->monster->HP -= DC->hero->get_dmg();
		if (DC->monster->HP <= 0) {
			DC->monster = nullptr;
		}
	}
	if ((DC->monster->type != MonsterType::MONSTER3 && DC->monster->type != MonsterType::MONSTER4) || DC->hero->skill_state != SkillState::ELECTRIC)
		return;
	if (DC->monster->force_shape.overlap(DC->hero->force_shape)) {
		std::pair<std::pair<double,double>,std::pair<double,double>> v = coulombs_law(6, 3, *DC->monster->shape, *DC->hero->shape, DC->monster->e, DC->hero->e);
		DC->monster->set_adjust_speed(v.first.first, v.first.second);
		DC->hero->set_adjust_speed(v.second.first, v.second.second);
	}
}

void OperationCenter::_update_bullet_bullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Bullet*> &matter_bullets = DC->matterBullets;
	std::vector<Bullet*> &electrode_bullets = DC->electrodeBullets;
	for (size_t i = 0; i < matter_bullets.size(); ++i) {
		for (size_t j = i + 1; j < matter_bullets.size(); ++j) {
			if (matter_bullets[i]->shape->overlap(*matter_bullets[j]->shape)) {
				matter_bullets[i]->update_matter(matter_bullets[j]->get_state());
				matter_bullets[j]->update_matter(matter_bullets[i]->get_state());
				_draw_explode(matter_bullets[i]->shape->center_x(), matter_bullets[i]->shape->center_y(), 2);
			}
		}
	}
	for (size_t i = 0; i < electrode_bullets.size(); ++i) {
		for (size_t j = i + 1; j < electrode_bullets.size(); ++j) {
			if (electrode_bullets[i]->force_shape.overlap(electrode_bullets[j]->force_shape)) {
				std::pair<std::pair<double,double>,std::pair<double,double>> v = coulombs_law(1, 1, *electrode_bullets[i]->shape, *electrode_bullets[j]->shape, electrode_bullets[i]->e, electrode_bullets[j]->e);
				electrode_bullets[i]->set_adjust_speed(v.first.first, v.first.second);
				electrode_bullets[j]->set_adjust_speed(v.second.first, v.second.second);
			}
			if (electrode_bullets[i]->shape->overlap(*electrode_bullets[j]->shape) &&
				electrode_bullets[i]->update_electrode(electrode_bullets[j]->get_state())) { // 碰到，需不需要爆炸
				// std::cout << "e e c" << std::endl;
				_draw_explode(electrode_bullets[i]->shape->center_x(), electrode_bullets[i]->shape->center_y(), 0);
				electrode_bullets[i]->update_electrode(electrode_bullets[j]->get_state());
				electrode_bullets[j]->update_electrode(electrode_bullets[i]->get_state());
			}
		}
	}
}

// 這邊的遍歷演算法真的很破，需要優化一下複雜度
void OperationCenter::_update_tool_bullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Tool*> &tools = DC->tools;
	std::vector<Bullet*> &matter_bullets = DC->matterBullets;
	std::vector<Bullet*> &eletrode_bullets = DC->electrodeBullets;
	std::vector<Bullet*> &wave_bullets = DC->waveBullets;
	for (size_t i = 0; i < matter_bullets.size(); ++i) {
		for (size_t j = 0; j < tools.size(); ++j) {
			if (matter_bullets[i]->shape->overlap(*tools[j]->shape)) {
				// 普通子彈碰到道具要停止
				matter_bullets[i]->set_adjust_speed(-matter_bullets[i]->get_speed().first, -matter_bullets[i]->get_speed().second);
				break; // 因為每顆子彈只會同時碰到一個道具
			}
		}
	}
	// wave
	for (size_t i = 0; i < wave_bullets.size(); ++i) {
		for (size_t j = 0; j < tools.size(); ++j) {
			if (wave_bullets[i]->shape->overlap(*tools[j]->shape) && // 判斷 wave 有沒有碰到道具
				wave_bullets[i]->update_wave(tools[j]->shape->center_x(), tools[j]->shape->center_y(), tools[j]->get_angle(), tools[j]->get_type(), tools[j]->get_focus())) { // 進入函式：1. 判斷是不是聲波 (要不要破壞道具)。2. 改變子彈移動方向
					delete tools[j];
					tools.erase(tools.begin() + j);
				break;
			}
		}
	}
	for (size_t i = 0; i < eletrode_bullets.size(); ++i) {
		for (size_t j = 0; j < tools.size(); ++j) {
			if (eletrode_bullets[i]->shape->overlap(*tools[j]->shape) &&
				tools[j]->get_type() == ToolType::E_FIELD) {
				// std::cout << "tool&bulet" << std::endl;
				// reaction
				double mass_e = 20.0;
				std::pair<double, double> f = tools[j]->get_e_force();
				eletrode_bullets[i]->set_adjust_speed((eletrode_bullets[i]->get_state() == BulletState::NEGATIVE) ? f.first / mass_e : -f.first / mass_e,
													(eletrode_bullets[i]->get_state() == BulletState::NEGATIVE) ? f.second / mass_e : -f.second / mass_e);
				break; // 因為每顆子彈只會同時碰到一個道具
			}
		}
	}
}

void OperationCenter::_update_tool_others() {
	// tool is static object (固定的物體)，any other matter collide it will be stopped
	MonsterT *monster = DataCenter::get_instance()->monster;
	Hero *hero = DataCenter::get_instance()->hero;
	std::vector<Tool*> &tools = DataCenter::get_instance()->tools;
	
	for (size_t i = 0; i < tools.size(); ++i) {
		if (i != tools.size() - 1 && tools[i]->shape->overlap(*tools.back()->shape)) {
			std::cout << "tool tool overlap" <<std::endl;
			delete tools.back();
			tools.erase(tools.begin() + tools.size() - 1);
		}
		if (tools[i]->shape->overlap(*(hero->shape))) {
			hero->is_collid = true;
		}
		if (monster == nullptr)
			continue;
		if (tools[i]->shape->overlap(*(monster->shape))) {
			monster->is_collid = true;
		}
	}
}

void OperationCenter::draw() {
	// std::cout << 14 << std::endl;
	_draw_monster();
	// std::cout << 15 << std::endl;
	_draw_tool();
	// std::cout << 16 << std::endl;
	_draw_bullet();
	// std::cout << 17 << std::endl;
	_draw_block();
}

void OperationCenter::_draw_monster() {
	MonsterT *monster = DataCenter::get_instance()->monster;
	if (monster != nullptr){
		monster->draw();
	}
}

void OperationCenter::_draw_bullet() {
	std::vector<Bullet*> &bullets = DataCenter::get_instance()->bullets;
	std::vector<Bullet*> &matter_bullets = DataCenter::get_instance()->matterBullets;
	std::vector<Bullet*> &eletrode_bullets = DataCenter::get_instance()->electrodeBullets;
	std::vector<Bullet*> &wave_bullets = DataCenter::get_instance()->waveBullets;
	for(Bullet *bullet : bullets)
		bullet->draw();
	for(Bullet *bullet : matter_bullets)
		bullet->draw();
	for(Bullet *bullet : eletrode_bullets)
		bullet->draw();
	for(Bullet *bullet : wave_bullets)
		bullet->draw();
}

void OperationCenter::_draw_tool() {
	std::vector<Tool*> &tools = DataCenter::get_instance()->tools;
	for(Tool *tool : tools)
		tool->draw();
}

void OperationCenter::_draw_block() {
	std::vector<Block*> &blocks = DataCenter::get_instance()->blocks;
	for(Block *block : blocks)
		block->draw();
}

void OperationCenter::_draw_explode(int x, int y, int type) {
	DataCenter *DC = DataCenter::get_instance();
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(BulletSetting::explode_paths[type]);
    Point offset = DC->camera->transform_bitmap(x, y);
    algif_draw_gif(
        gif,
        offset.center_x() - gif->width / 2.0,
        offset.center_y() - gif->height / 2.0,
        0
    );
}