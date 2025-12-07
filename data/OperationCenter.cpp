#include "OperationCenter.h"
#include "DataCenter.h"
#include "../monsters/Monster.h"
#include "../monsters/MonsterT.h"
#include "../towers/Tower.h"
#include "../towers/Bullet.h"
#include "../towers/Block.h"
#include "../Player.h"
#include "../Hero.h"
#include <iostream>

void OperationCenter::update() {
	// Update monsters.
	_update_monster();
	// Update towers.
	_update_tower();
	// Update tower bullets.
	_update_bullet();
	// Update tools.
	_update_tool();
	// If any bullet overlaps with any monster, we delete the bullet, reduce the HP of the monster, and delete the monster if necessary.
	_update_monster_bullet();
	_update_hero_bullet();
	// If any monster hits hero, monster dies. 
	_update_monster_hero();

	_update_block();
	_update_hero_block();
	_update_bullet_block();

	_update_bullet_bullet();
	_update_tool_bullet();
	_update_tool_others();
}

void OperationCenter::_update_monster() {
	MonsterT *monster = DataCenter::get_instance()->monster;
	if (monster != nullptr)
		monster->update();
	std::vector<Monster*> &monsters = DataCenter::get_instance()->monsters;
	for(Monster *monster : monsters)
		monster->update();
}

void OperationCenter::_update_tower() {
	std::vector<Tower*> &towers = DataCenter::get_instance()->towers;
	for(Tower *tower : towers)
		tower->update();
}

void OperationCenter::_update_bullet() {
	std::vector<Bullet*> &bullets = DataCenter::get_instance()->bullets;
	for(Bullet *bullet : bullets)
		bullet->update();
	// Detect if a bullet flies too far (exceeds its fly distance limit), which means the bullet lifecycle has ended.
	for(size_t i = 0; i < bullets.size(); ++i) {
		if(bullets[i]->get_fly_dist() <= 0 || !bullets[i]->alive) {
			delete bullets[i];
			bullets.erase(bullets.begin() + i);
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
	std::vector<Monster*> &monsters = DC->monsters;
	// MonsterT *monster = DC->monster;
	std::vector<Bullet*> &bullets = DC->bullets;
	for(size_t i = 0; i < monsters.size(); ++i) {
		for(size_t j = 0; j < bullets.size(); ++j) {
			// Check if the bullet overlaps with the monster.
			if(monsters[i]->shape->overlap(*(bullets[j]->shape))) {
				// Reduce the HP of the monster. Delete the bullet.
				monsters[i]->HP -= bullets[j]->get_dmg();
				delete bullets[j];
				bullets.erase(bullets.begin() + j);
				--j;
			}
		}
	}
	if (DC->monster == nullptr) return;
	for (size_t j = 0; j < bullets.size(); ++j) {
		// Check if the bullet overlaps with the monster.
		if (DC->monster->shape->overlap(*(bullets[j]->shape))) {
			// Reduce the HP of the monster. Delete the bullet.
			DC->monster->HP -= bullets[j]->get_dmg();
			delete bullets[j];
			bullets.erase(bullets.begin() + j);
			--j;
		}
		if (DC->monster->HP <= 0) {
			DC->monster = nullptr;
			break;
		}
	}
}

void OperationCenter::_update_hero_bullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Bullet*> &bullets = DC->bullets;
	// std::vector<Bullet*> &matter_bullets = DC->matterBullets;
	// for (size_t i = 0; i < matter_bullets.size(); ++i) {
	// 	if (DC->hero->shape->overlap(*(matter_bullets[i]->shape))) {
	// 		matter_bullets[i]->update_matter(DC->hero->bullet_state);
	// 	}
	// }
	if (DC->hero == nullptr) return;
	for (size_t j = 0; j < bullets.size(); ++j) {
		// Check if the bullet overlaps with the monster.
		if (DC->hero->shape->overlap(*(bullets[j]->shape))) {
			// Reduce the HP of the monster. Delete the bullet.
			DC->hero->HP -= bullets[j]->get_dmg();
			delete bullets[j]; // 這邊的delete感覺要改，看起來只有刪bullets，沒有刪其他的bullet vector
			bullets.erase(bullets.begin() + j);
			--j;
		}
	}
}

void OperationCenter::_update_bullet_block() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Block*> &blocks = DC->blocks;
	std::vector<Bullet*> &bullets = DC->bullets;
	for (size_t i = 0; i < bullets.size(); ++i) {
		for (size_t j = 0; j < blocks.size(); ++j) {
			// Check if the bullet overlaps with the monster.
			if (bullets[i]->shape->overlap(*(blocks[j]->shape))) {
				// Reduce the HP of the monster. Delete the bullet.
				blocks[j]->update_bullet_hit(bullets[i]->get_state());
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
	std::vector<Monster*> &monsters = DC->monsters;
	// MonsterT *monster = DC->monster;
	for(size_t i = 0; i < monsters.size(); ++i) {
		if(monsters[i]->shape->overlap(*(DC->hero->shape))){
			monsters[i]->HP = 0;
		}
	}
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
}

void OperationCenter::_update_bullet_bullet() {
	DataCenter *DC = DataCenter::get_instance();
	std::vector<Bullet*> &matter_bullets = DC->matterBullets;
	std::vector<Bullet*> &eletrode_bullets = DC->electrodeBullets;
	bool need_delete = false;
	for (size_t i = 0; i < matter_bullets.size(); ++i) {
		for (size_t j = i + 1; j < matter_bullets.size(); ++j) {
			if (matter_bullets[i]->shape->overlap(*matter_bullets[j]->shape)) {
				matter_bullets[i]->update_matter(matter_bullets[j]->get_state());
				matter_bullets[j]->update_matter(matter_bullets[i]->get_state());
			}
		}
	}
	for (size_t i = 0; i < eletrode_bullets.size(); ++i) {
		for (size_t j = i + 1; j < eletrode_bullets.size(); ++j) {
			if (eletrode_bullets[i]->shape->overlap(*eletrode_bullets[j]->shape)) { // 碰到，需不需要爆炸
				need_delete = eletrode_bullets[i]->update_electrode(eletrode_bullets[j]->get_state());
				eletrode_bullets[j]->update_electrode(eletrode_bullets[i]->get_state());
			}
			if (need_delete) { // 爆炸後刪除兩顆子彈
				delete eletrode_bullets[i];
				delete eletrode_bullets[j];
				eletrode_bullets.erase(eletrode_bullets.begin() + i);
				eletrode_bullets.erase(eletrode_bullets.begin() + j - 1);
				--i;
				break;
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
	// 普通子彈碰到道具要停止?消失?反彈?
	for (size_t i = 0; i < matter_bullets.size(); ++i) {
		for (size_t j = 0; j < tools.size(); ++j) {
			if (matter_bullets[i]->shape->overlap(*tools[j]->shape)) {
				// reaction
				break; // 因為每顆子彈只會同時碰到一個道具
			}
		}
	}
	for (size_t i = 0; i < eletrode_bullets.size(); ++i) {
		for (size_t j = 0; j < tools.size(); ++j) {
			if (eletrode_bullets[i]->shape->overlap(*tools[j]->shape)) {
				// reaction
				break; // 因為每顆子彈只會同時碰到一個道具
			}
		}
	}
	// wave
	for (size_t i = 0; i < wave_bullets.size(); ++i) {
		for (size_t j = 0; j < tools.size(); ++j) {
			if (wave_bullets[i]->shape->overlap(*tools[j]->shape) && // 判斷 wave 有沒有碰到道具
				wave_bullets[i]->update_wave(tools[j]->shape->center_x(), tools[j]->shape->center_y(), tools[i]->get_angle(), tools[j]->get_type())) { // 進入函式：1. 判斷是不是聲波 (要不要破壞道具)。2. 改變子彈移動方向
					delete tools[j];
					tools.erase(tools.begin() + j);
					// --j;  // 反正會直接 break ，所以可以不用 --j
				break;
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
	_draw_monster();
	_draw_tower();
	_draw_tool();
	_draw_bullet();
	_draw_block();
}

void OperationCenter::_draw_monster() {
	MonsterT *monster = DataCenter::get_instance()->monster;
	if (monster != nullptr)
		monster->draw();
	std::vector<Monster*> &monsters = DataCenter::get_instance()->monsters;
	for(Monster *monster : monsters)
		monster->draw();
}

void OperationCenter::_draw_tower() {
	std::vector<Tower*> &towers = DataCenter::get_instance()->towers;
	for(Tower *tower : towers)
		tower->draw();
}

void OperationCenter::_draw_bullet() {
	std::vector<Bullet*> &bullets = DataCenter::get_instance()->bullets;
	for(Bullet *bullet : bullets)
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