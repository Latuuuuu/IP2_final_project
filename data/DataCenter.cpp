#include "DataCenter.h"
#include <cstring>
#include "../LevelT.h"
#include "../monsters/MonsterT.h"
#include "../towers/Tool.h"
#include "../towers/Bullet.h"
#include "../Hero.h"
#include "../Camera.h"
#include "../towers/Block.h"

// fixed settings
namespace DataSetting {
	constexpr double FPS = 60;
	constexpr int window_width = 1280;
	constexpr int window_height = 720;
	constexpr int game_field_length = 1000;
}

DataCenter::DataCenter() {
	this->FPS = DataSetting::FPS;
	this->window_width = DataSetting::window_width;
	this->window_height = DataSetting::window_height;
	this->game_field_length = DataSetting::game_field_length;
	memset(key_state, false, sizeof(key_state));
	memset(prev_key_state, false, sizeof(prev_key_state));
	mouse = Point(0, 0);
	memset(mouse_state, false, sizeof(mouse_state));
	memset(prev_mouse_state, false, sizeof(prev_mouse_state));
	level = new LevelT();
	hero = new Hero();
	camera = new Camera(this->window_width, this->window_height);
	monster = nullptr;
}

DataCenter::~DataCenter() {
	delete camera;
	delete level;
	delete monster;
	for(Tool *&t : tools) {
		delete t;
	}
	for(Bullet *&b : bullets) {
		delete b;
	}
	for(Block *&b : blocks) {
		delete b;
	}
}

void DataCenter::reset_bullet() {
	monster = nullptr;
	for(size_t i = 0; i < bullets.size(); ++i) {
		delete bullets[i];
		bullets.erase(bullets.begin() + i);
		--i;
	}
	for (size_t i = 0; i < tools.size(); ++i) {
		delete tools[i];
		tools.erase(tools.begin() + i);
		--i;
	}
}