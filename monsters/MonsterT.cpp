#include "MonsterT.h"
#include "Monster1.h"
#include "Monster2.h"
#include "Monster3.h"
#include "Monster4.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Hero.h"
#include "../towers/Bullet.h"
// #include "../Level.h"
#include "../Camera.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include <allegro5/allegro_primitives.h>
#include <random>
#include <iostream>

using namespace std;

MonsterT *MonsterT::create_monster(MonsterType type, Point borned_place) {
	switch (type) {
		case MonsterType::MONSTER1: {
			cout << "create monster1";
			return new Monster1(borned_place);
		}
		case MonsterType::MONSTER2: {
			cout << "create monster2";
			return new Monster2(borned_place);
		}
		case MonsterType::MONSTER3: {
			cout << "create monster3";
			return new Monster3(borned_place);
		}
		case MonsterType::MONSTER4: {
			cout << "create monster4";
			return new Monster4(borned_place);
		}
		case MonsterType::MONSTERTYPE_MAX: {}
	}
	GAME_ASSERT(false, "monster type error.");
}

Dir MonsterT::convert_dir(const Point &v) {
	if(v.y < 0 && abs(v.y) >= abs(v.x))
		return Dir::UP;
	if(v.y > 0 && abs(v.y) >= abs(v.x))
		return Dir::DOWN;
	if(v.x < 0 && abs(v.x) >= abs(v.y))
		return Dir::LEFT;
	if(v.x > 0 && abs(v.x) >= abs(v.y))
		return Dir::RIGHT;
	return Dir::RIGHT;
}

Point MonsterT::dir_to_vector(const Dir dir) {
	if (dir == Dir::UP) {
		const Point p(0, -1);
		return p;
	} else if (dir == Dir::DOWN) {
		const Point p(0, 1);
		return p;
	} else if (dir == Dir::LEFT) {
		const Point p(-1, 0);
		return p;
	} else if (dir == Dir::RIGHT) {
		const Point p(1, 0);
		return p;
	} else {
		const Point p(0, 0);
		return p;
	}
}

MonsterT::MonsterT(MonsterType type, Point borned_place) {

	shape.reset(new Rectangle{borned_place.x, borned_place.y, borned_place.x, borned_place.y});
	this->type = type;
	this->action = Action::IDLE;
	this->action_timer[0] = 5000;
	this->action_timer[1] = 5000;
	this->action_timer[2] = 20000;
	this->action_timer[3] = 1000;
	this->attack_cd = 100;
	this->can_attack = false;
	dir = Dir::RIGHT;
	this->timer = clock();
	this->cd_timer = clock();
	this->need_effect = true;
	bitmap_img_id = 0;
	bitmap_switch_counter = 0;
	bullet_state = BulletState::SOLID;
}
void MonsterT::attack() {}
Point MonsterT::hero_tracker() {
	DataCenter *DC = DataCenter::get_instance();
	Point dir_vector;
	const Point &hero_position = Point(DC->hero->shape->center_x(), DC->hero->shape->center_y());
	this->dist_to_hero = Point::dist(Point{shape->center_x(), shape->center_y()}, hero_position);
	dir_vector.update_center_x((hero_position.center_x() - shape->center_x()) / this->dist_to_hero);
	dir_vector.update_center_y((hero_position.center_y() - shape->center_y()) / this->dist_to_hero);
	return dir_vector;
	// track hero
}
void MonsterT::update() {
	DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();

	// After a period, the bitmap for this monster should switch from (i)-th image to (i+1)-th image to represent animation.
	if(bitmap_switch_counter) --bitmap_switch_counter;
	else {
		bitmap_img_id = (bitmap_img_id + 1) % (bitmap_img_ids[static_cast<int>(dir)].size());
		bitmap_switch_counter = bitmap_switch_freq;
	}
	// v (velocity) divided by FPS is the actual moving pixels per frame.
	double movement = v / DC->FPS;
	// Keep trying to move to next destination in "path" while "path" is not empty and we can still move.
	std::random_device rd;
	/* 亂數產生器 */
	std::default_random_engine generator(rd());
	/* 亂數的機率分布 */
	std::uniform_int_distribution<int> unif(static_cast<int>(Action::STAND), static_cast<int>(Action::TELEPORT));
	/* 產生亂數 */
	// cout << int(clock() - this->cd_timer) << " " << attack_cd << " " << can_attack << endl;
	if (int(clock() - this->cd_timer) >= attack_cd && can_attack) {
		// cout << "can attack" << endl;
		attack();
		this->cd_timer = clock();
	}
	double dx = hero_tracker().center_x();
	double dy = hero_tracker().center_y();
	switch (this->action)
	{
	case Action::IDLE:
		// don't move
		// don't attack
		// start display born effect if haven't start
		if (need_effect) {
			// draw effect 1
			need_effect = false;
		}
		if ((int)(clock() - this->timer) >= action_timer[(int)Action::IDLE]) {
			// this->action = static_cast<Action>(unif(generator));
			this->action = Action::STAND;
			this->timer = clock();
		}
		break;
	case Action::STAND:
		// don't move
		can_attack = true;
		// track hero
		dir = convert_dir(Point{dx, dy});
		if ((int)(clock() - this->timer) >= action_timer[(int)Action::STAND]) {
			can_attack = false;
			// this->action = static_cast<Action>(unif(generator));
			this->action = Action::CHASE;
			this->timer = clock();
			// cout << "convert to chase" << endl;
		}
		break;
	case Action::CHASE:
		// cout << "chase mode" << endl;
		can_attack = true;
		// follow hero
		dir = convert_dir(Point{dx, dy});
		if (!is_collid) {
			shape->update_center_x(shape->center_x() + dx * movement);
			shape->update_center_y(shape->center_y() + dy * movement);
		} else {
			is_collid = false;
		}
		// cout << "update pose" << endl;
		// cout << shape->center_x() << " " << shape->center_y() << endl;
		if ((int)(clock() - this->timer) >= action_timer[(int)Action::CHASE]) {
			can_attack = false;
			this->action = Action::TELEPORT;
			// this->action = static_cast<Action>(unif(generator));
			this->timer = clock();
		}
		// cout << can_attack << endl;
		break;
	case Action::TELEPORT:
		// transport to a point near hero (need to detect the place's validation)
		// cout << "teleport" << endl;
		// cout << "hero x: " << DC->hero->shape->center_x() << ", hero y: " << DC->hero->shape->center_y() << endl;
		// cout << "dist to hero: " << this->dist_to_hero << ", hero_w: " << DC->hero->get_size().center_x() << ", hero_h: " << DC->hero->get_size().center_y() << endl;
		dx *= this->dist_to_hero - DC->hero->get_size().center_x() * 1.5;
		dy *= this->dist_to_hero - DC->hero->get_size().center_y() * 1.5;
		// cout << "(dx, dy): " << dx << ", " << dy << endl;
		if (!is_collid) {
			shape->update_center_x(shape->center_x() + dx);
			shape->update_center_y(shape->center_y() + dy);
		} else {
			is_collid = false;
		}
		// display transport effect
		this->can_attack = false;
		this->action = Action::STAND;
		// this->action = static_cast<Action>(unif(generator));
		break;
	default:
		break;
	}
	// // Update real hit box for monster.
	// char buffer[50];
	// sprintf(
	// 	buffer, "%s/%s_%d.png",
	// 	MonsterSetting::monster_imgs_root_path[static_cast<int>(type)],
	// 	MonsterSetting::dir_path_prefix[static_cast<int>(dir)],
	// 	bitmap_img_ids[static_cast<int>(dir)][bitmap_img_id]);
	// ALLEGRO_BITMAP *bitmap = IC->get(buffer);
	// const double &cx = shape->center_x();
	// const double &cy = shape->center_y();
	// // We set the hit box slightly smaller than the actual bounding box of the image because there are mostly empty spaces near the edge of a image.
	// const int &h = al_get_bitmap_width(bitmap) * 0.8;
	// const int &w = al_get_bitmap_height(bitmap) * 0.8;
	// shape.reset(new Rectangle{
	// 	(cx - w / 2.), (cy - h / 2.),
	// 	(cx - w / 2. + w), (cy - h / 2. + h)
	// });
}

void MonsterT::draw() {
	DataCenter *DC = DataCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	char buffer[50];
	sprintf(
		buffer, "%s/%s_%d_%s.png",
		MonsterSetting::monster_imgs_root_path[static_cast<int>(type)],
		MonsterSetting::dir_path_prefix[static_cast<int>(dir)],
		bitmap_img_ids[static_cast<int>(dir)][bitmap_img_id],
		MonsterSetting::bullet_prefix[static_cast<int>(bullet_state)]);
	ALLEGRO_BITMAP *bitmap = IC->get(buffer);
	Point offset = DC->camera->transform_object(*shape);
	al_draw_bitmap(
		bitmap,
		offset.center_x() - al_get_bitmap_width(bitmap) / 2,
		offset.center_y() - al_get_bitmap_height(bitmap) / 2, 0);
}
