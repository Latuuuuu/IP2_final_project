#include "MonsterT.h"
#include "Monster1.h"
#include "Monster2.h"
#include "Monster3.h"
#include "Monster4.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../data/GIFCenter.h"
#include "../Hero.h"
#include "../towers/Bullet.h"
#include "../LevelT.h"
#include "../Camera.h"
#include "../shapes/Point.h"
#include "../shapes/Rectangle.h"
#include "../Utils.h"
#include "../algif5/algif.h"
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
	force_shape.x = borned_place.x;
	force_shape.y = borned_place.y;
	force_shape.r = 0;
	e = 0;
	this->type = type;
	this->action = Action::IDLE;
	this->last_action = this->action;
	this->action_timer[0] = 2000;
	this->action_timer[1] = 10000;
	this->action_timer[2] = 2000;
	this->action_timer[3] = 20000;
	this->action_timer[4] = 1000;
	this->attack_cd = 100;
	this->atk_period = 3000;
	this->can_attack = false;
	dir = Dir::RIGHT;
	this->timer = clock();
	this->cd_timer = clock();
	this->atk_period_timer = clock();
	this->need_effect = true;
	bitmap_img_id = 0;
	bitmap_switch_counter = 0;
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
Point MonsterT::random_direction() {
	Point dir_vector;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution{0, 359};
	double angle = distribution(generator) / 180.0 * 3.14;
	dir_vector.x = cos(angle);
	dir_vector.y = sin(angle);
	return dir_vector;
}
void MonsterT::update() {
	DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();

	// After a period, the bitmap for this monster should switch from (i)-th image to (i+1)-th image to represent animation.
	int dir_idx = static_cast<int>(dir);
    if (dir_idx >= 0 && dir_idx < (int)bitmap_img_ids.size() && !bitmap_img_ids[dir_idx].empty()) {
        if(bitmap_switch_counter) --bitmap_switch_counter;
        else {
            bitmap_img_id = (bitmap_img_id + 1) % (bitmap_img_ids[dir_idx].size());
            bitmap_switch_counter = bitmap_switch_freq;
        }
    }
	// v (velocity) divided by FPS is the actual moving pixels per frame.
	double movement = v / DC->FPS;
	// Keep trying to move to next destination in "path" while "path" is not empty and we can still move.
	if (int(clock() - this->cd_timer) >= attack_cd && can_attack) {
		attack();
		this->cd_timer = clock();
	}
	if (int(clock() - this->atk_period_timer) >= atk_period) {
		can_attack = !can_attack;
		this->atk_period_timer = clock();
	}
	double dx = 0, dy = 0;
	switch (this->action)
	{
	case Action::IDLE:
		// don't move
		// don't attack
		// start display born effect if haven't start
		can_attack = false;
		// if (need_effect) {
		// 	// draw effect 1
		// 	need_effect = false;
		// }
		draw_effect();
		if ((int)(clock() - this->timer) >= action_timer[(int)Action::IDLE]) {
			// this->action = distribution(generator);
			this->last_action = this->action;
			this->action = Action::STAND;
			this->timer = clock();
			std::cout << (int)action << std::endl;
		}
		break;
	case Action::STAND:
		// don't move
		// can_attack = true;
		// track hero
		dx = hero_tracker().center_x();
		dy = hero_tracker().center_y();
		dir = convert_dir(Point{dx, dy});
		this->last_action = this->action;
		if ((int)(clock() - this->timer) >= action_timer[(int)Action::STAND]) {
			// can_attack = false;
			// this->action = static_cast<Action>(unif(generator));
			this->action = Action::RUSH;
			this->timer = clock();
			std::cout << (int)action << std::endl;
		}
		break;
	case Action::RUSH:
		if (last_action != action || this->is_collid) {
			vect = random_direction();
			this->is_collid = false;
		}
		dx = vect.center_x() * movement * 3 + adjust_speed_x / DC->FPS;
		dy = vect.center_y() * movement * 3 + adjust_speed_y / DC->FPS;
		dir = convert_dir(Point{dx, dy});
		this->last_action = this->action;
		if ((int)(clock() - this->timer) >= action_timer[(int)Action::RUSH]) {
			this->action = Action::CHASE;
			this->timer = clock();
			std::cout << (int)action << std::endl;
		}
		break;
	case Action::CHASE:
		// cout << "chase mode" << endl;
		// can_attack = true;
		// follow hero
		dx = hero_tracker().center_x() * movement + adjust_speed_x / DC->FPS;
		dy = hero_tracker().center_y() * movement + adjust_speed_y / DC->FPS;
		dir = convert_dir(Point{dx, dy});
		// if (this->is_collid) {
		// 	sign = -1;
		// 	this->is_collid = false;
		// }
		// cout << "update pose" << endl;
		// cout << shape->center_x() << " " << shape->center_y() << endl;
		this->last_action = this->action;
		if ((int)(clock() - this->timer) >= action_timer[(int)Action::CHASE]) {
			this->action = Action::TELEPORT;
			this->timer = clock();
			std::cout << (int)action << std::endl;
		}
		// cout << can_attack << endl;
		break;
	case Action::TELEPORT:
		// transport to a point near hero (need to detect the place's validation)
		dx = hero_tracker().center_x();
		dy = hero_tracker().center_y();
		dx *= this->dist_to_hero - DC->hero->get_size().center_x() * 1.5;
		dy *= this->dist_to_hero - DC->hero->get_size().center_y() * 1.5;
		this->last_action = this->action;
		this->action = Action::STAND;
		std::cout << (int)action << std::endl;
		break;
	default:
		break;
	}
	double x = shape->center_x() + dx;
	double y = shape->center_y() + dy;
	if (x > LevelSetting::lvl_bound_x[static_cast<int>(this->type)+1] - 640 - graph_w) {
		x = LevelSetting::lvl_bound_x[static_cast<int>(this->type)+1] - 640 - graph_w;
	} else if (x < LevelSetting::puzzle_bound_x[static_cast<int>(this->type)] + graph_w) {
		x = LevelSetting::puzzle_bound_x[static_cast<int>(this->type)] + graph_w;
	}
	if (y > DC->window_height - graph_h) {
		y = DC->window_height - graph_h;
	} else if (shape->center_y() + dy < + graph_h) {
		y = graph_h;
	}
	shape->update_center_x(x);
    shape->update_center_y(y);
    force_shape.update_center_x(x);
    force_shape.update_center_y(y);
	adjust_speed_x = 0.0;
	adjust_speed_y = 0.0;
	last_HP = HP;
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

void MonsterT::draw_effect() {
	DataCenter *DC = DataCenter::get_instance();
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get("assets/gif/explode.gif");
    Point offset = DC->camera->transform_object(*shape);
	// std::cout << "d" << std::endl;
    algif_draw_gif(
        gif,
        offset.center_x() - gif->width / 2.0,
        offset.center_y() - gif->height / 2.0,
        0
    );
}