#include "Bullet.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Camera.h"
#include "../shapes/Point.h"
#include "../LevelT.h"
#include <cmath>
#include <algorithm>
#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_primitives.h>
#include "../physics.hpp"

Bullet::Bullet(const Point &p, const Point &vector, const std::string &path, double v, int dmg, double fly_dist) {
	ImageCenter *IC = ImageCenter::get_instance();
	// DataCenter *DC = DataCenter::get_instance();
	this->fly_dist = fly_dist;
	this->dmg = dmg;
	bitmap = IC->get(path);
	double r = std::min(al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap)) * 0.25;
	shape.reset(new Circle{p.x, p.y, r});
	double d = Point::dist(vector);
	vx = vector.x * v / d;
	vy = vector.y * v / d;
    this->state = BulletState::BALL;
	this->e = 0;
	force_shape.x = p.x;
	force_shape.y = p.y;
	force_shape.r = 0;
}

Bullet::Bullet(const Point &p, const Point &vector, const std::string &path, double v, int dmg, double fly_dist, BulletState state) {
	ImageCenter *IC = ImageCenter::get_instance();
	// DataCenter *DC = DataCenter::get_instance();
	this->fly_dist = fly_dist;
	this->dmg = dmg;
	bitmap = IC->get(path);
	double r = std::min(al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap)) * 0.25;
	double d = Point::dist(vector);
	vx = vector.x * v / d;
	vy = vector.y * v / d;
    this->state = state;
	force_shape.x = p.x;
	force_shape.y = p.y;
	force_shape.r = 0;
	if (this->state == BulletState::POSITIVE) {
		force_shape.r = 40;
		this->e = 10;
	}
	else if (this->state == BulletState::NEGATIVE) {
		force_shape.r = 40;
		this->e = -10;
	}
	shape.reset(new Circle{p.x, p.y, r});
}

/**
 * @brief Update the bullet position by its velocity and fly_dist by its movement per frame.
 * @details We don't detect whether to delete the bullet itself here because deleting a object itself doesn't make any sense.
 */
void
Bullet::update() {
	if(fly_dist == 0) return;
	DataCenter *DC = DataCenter::get_instance();
	vx = (vx + adjust_speed_x);
	vy = (vy + adjust_speed_y);
	adjust_speed_x = 0;
	adjust_speed_y = 0;
	double dx = (vx + adjust_speed_x) / DC->FPS;
	double dy = (vy + adjust_speed_y) / DC->FPS;
	double movement = Point::dist(Point{dx, dy}, Point{0, 0});
	if(fly_dist > movement) {
		shape->update_center_x(shape->center_x() + dx);
		shape->update_center_y(shape->center_y() + dy);
		force_shape.update_center_x(shape->center_x() + dx);
		force_shape.update_center_y(shape->center_y() + dy);
		fly_dist -= movement;
	} else {
		shape->update_center_x(shape->center_x() + dx * fly_dist / movement);
		shape->update_center_y(shape->center_y() + dy * fly_dist / movement);
		force_shape.update_center_x(shape->center_x() + dx * fly_dist / movement);
		force_shape.update_center_y(shape->center_y() + dy * fly_dist / movement);
		fly_dist = 0;
	}
	if ((shape->center_x() + dx > LevelSetting::lvl_bound_x[DC->level->get_level()] - 640) ||
		(shape->center_x() + dx < LevelSetting::lvl_bound_x[DC->level->get_level()-1] && DC->level->get_level_start()) ||
		(shape->center_x() + dx < LevelSetting::puzzle_bound_x[DC->level->get_level()-1] && DC->level->get_monster_spawn()) ||
		(shape->center_y() + dy > DC->window_height) ||
		(shape->center_y() + dy < 0))
        this->alive = false;
}

void Bullet::update_matter(BulletState collid_matter) {  // 第一關子彈的剪刀石頭布
	if ((this->state == BulletState::SOLID && collid_matter == BulletState::GAS) ||
	    (this->state == BulletState::GAS && collid_matter == BulletState::LIQUID) ||
		(this->state == BulletState::LIQUID && collid_matter == BulletState::SOLID)) {
		this->dmg = 0;
		this->alive = false;
	} else if ((this->state == BulletState::GAS && collid_matter == BulletState::SOLID) ||
			(this->state == BulletState::SOLID && collid_matter == BulletState::LIQUID) ||
			(this->state == BulletState::LIQUID && collid_matter == BulletState::GAS)) {
		this->dmg *= 2;
	}
	// 判斷兩個都是同樣的物質型態，讓速度變 0
}

bool Bullet::update_wave(int x, int y, double z, ToolType type, std::pair<Point, Point> focal) { // 第二關，撞到道具時的動作
	if (type == ToolType::E_FIELD) // 如果是電場，不要與光波&聲波交互
		return false;
	if (this->state == BulletState::SOUND) {
		return true; // need to destroy collided tool
	}
	if (this->state == BulletState::LASER) {
		std::pair<double, double> vec_f0, vec_f1, vec_f, vec_in;
		double cos2_in, vel;
		int sign = 1;
		vec_in.first = this->vx;
		vec_in.second = this->vy;
		vel = sqrt(dist2(vec_in));
		if (type == ToolType::MIRROR) {
			vec_f0.first = cos(z);
			vec_f0.second = sin(z);
			vec_f1.first = cos(z+3.14);
			vec_f1.second = sin(z+3.14);
			if (dot(vec_in, vec_f0) > 0) {
				vec_f = vec_f0;
				vec_f0 = vec_f1;
			}
			else if (dot(vec_in, vec_f1) > 0)
				vec_f = vec_f1;
			cos2_in = dot(vec_f, vec_in)*dot(vec_f, vec_in)/dist2(vec_f)/dist2(vec_in);
			if (cross(vec_in, vec_f) < 0)
				sign = -1;
			this->vx = (sqrt(cos2_in)*vec_f0.first+sign*sqrt(1-cos2_in)*vec_f0.second) * vel;
			this->vy = -(sign*sqrt(1-cos2_in)*vec_f0.first-sqrt(cos2_in)*vec_f0.second) * vel;
			return false;
		}
		if (dot({focal.first.x - this->shape->center_x(),focal.first.y - this->shape->center_y()}, vec_in) > 0) {
			vec_f0.first = focal.first.x - this->shape->center_x();
			vec_f0.second = focal.first.y - this->shape->center_y();
			vec_f1.first = this->shape->center_x() - focal.second.x;
			vec_f1.second = this->shape->center_y() - focal.second.y;
			// std::cout << "1";
		} else if (dot({focal.second.x - this->shape->center_x(), focal.second.y - this->shape->center_y()}, vec_in) > 0) {
			vec_f0.first = focal.second.x - this->shape->center_x();
			vec_f0.second = focal.second.y - this->shape->center_y();
			vec_f1.first = this->shape->center_x() - focal.first.x;
			vec_f1.second = this->shape->center_y() - focal.first.y;
			// std::cout << "2";
		} else {
			// std::cout << "3";
			return false;
		}
		// std::cout << " vec_f0: " << vec_f0.first << " " << vec_f0.second << std::endl;
		// std::cout << " vec_f1: " << vec_f1.first << " " << vec_f1.second << std::endl;
		// std::cout << " vec_in: " << vec_in.first << " " << vec_in.second << std::endl;
		if (type == ToolType::CONVEX) { // 凸透鏡
			vec_f = vec_f1;
		} else if (type == ToolType::CONCAVE) { // 凹透鏡
			vec_f = vec_f0;
		} else { // 不是透鏡也不是鏡子
			return false;
		}
		cos2_in = dot(vec_f, vec_in)*dot(vec_f, vec_in)/dist2(vec_f)/dist2(vec_in);
		double dist_f = sqrt(dist2(vec_f));
		vec_f.first /= dist_f;
		vec_f.second /= dist_f;
		double factor = 1.33;
		double sin2_out = factor*factor*(1 - cos2_in);
		if (sin2_out > 1) {
			// std::cout << "0" << std::endl;
			sin2_out = 0.95;
			// return true;
		}
		double cos2_out = 1 - sin2_out;
		if (cross(vec_in, vec_f) >= 0) {
			sign = -1;
		}
		// std::cout << "sign: " << sign << " cos2" << cos2_in << " sin2" << 1-cos2_in << std::endl;
		this->vx = (sqrt(cos2_out)*vec_f.first+sign*sqrt(sin2_out)*vec_f.second) * vel;
		this->vy = -(sign*sqrt(sin2_out)*vec_f.first-sqrt(cos2_out)*vec_f.second) * vel;
	}
	return false;
}

bool Bullet::update_electrode(BulletState collid_electrode) {
	if (this->state != BulletState::NEGATIVE && this->state != BulletState::POSITIVE)
		return false; // 這個子彈沒有電極 不理
	if (this->state == collid_electrode) {
		return false; // don't delete this bullet
	}
	// declare new Explode object
	this->alive = false;
	return true; // need to delete this bullet
}

void Bullet::draw() {
	DataCenter *DC = DataCenter::get_instance();
	Point offset = DC->camera->transform_object(*shape);
	al_draw_bitmap(
		bitmap,
		offset.center_x() - al_get_bitmap_width(bitmap) / 2,
		offset.center_y() - al_get_bitmap_height(bitmap) / 2, 0);
	// 測hitbox用 
	// double width = al_get_bitmap_width(bitmap) *0.25;
	// al_draw_circle(
    //     offset.center_x(),
    //     offset.center_y(),
    //     width,
    //     al_map_rgb(255, 0, 0), 1.0);
}
