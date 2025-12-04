#include "DataCenter.h"
#include <cstring>
#include "../Level.h"
#include "../LevelT.h"
#include "../Player.h"
#include "../monsters/Monster.h"
#include "../monsters/MonsterT.h"
#include "../towers/Tower.h"
#include "../towers/Bullet.h"
#include "../Hero.h"
#include "../Camera.h"

// fixed settings
namespace DataSetting {
	constexpr double FPS = 60;
	constexpr int window_width = 800;
	constexpr int window_height = 600;
	constexpr int game_field_length = 600;
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
	player = new Player();
	level = new LevelT();
	level_old = new Level();
	hero = new Hero();
	camera = new Camera(this->window_width, this->window_height);
	monster = nullptr;
}

DataCenter::~DataCenter() {
	delete camera;
	delete player;
	delete level;
	for(Monster *&m : monsters) {
		delete m;
	}
	delete monster;
	for(Tower *&t : towers) {
		delete t;
	}
	for(Bullet *&b : towerBullets) {
		delete b;
	}
}

void DataCenter::reset_bullet() {
	monster = nullptr;
	for(size_t i = 0; i < towerBullets.size(); ++i) {
		delete towerBullets[i];
		towerBullets.erase(towerBullets.begin() + i);
		--i;
	}
}