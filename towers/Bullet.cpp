#include "Bullet.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Camera.h"
#include "../shapes/Point.h"
#include <algorithm>
#include <allegro5/bitmap_draw.h>

Bullet::Bullet(const Point &p, const Point &vector, const std::string &path, double v, int dmg, double fly_dist) {
	ImageCenter *IC = ImageCenter::get_instance();
	// DataCenter *DC = DataCenter::get_instance();
	this->fly_dist = fly_dist;
	this->dmg = dmg;
	bitmap = IC->get(path);
	double r = std::min(al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap)) * 0.8;
	shape.reset(new Circle{p.x, p.y, r});
	double d = Point::dist(vector);
	vx = vector.x * v / d;
	vy = vector.y * v / d;
    this->state = BulletState::BALL;
}

Bullet::Bullet(const Point &p, const Point &vector, const std::string &path, double v, int dmg, double fly_dist, BulletState state) {
	ImageCenter *IC = ImageCenter::get_instance();
	// DataCenter *DC = DataCenter::get_instance();
	this->fly_dist = fly_dist;
	this->dmg = dmg;
	bitmap = IC->get(path);
	double r = std::min(al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap)) * 0.8;
	shape.reset(new Circle{p.x, p.y, r});
	double d = Point::dist(vector);
	vx = vector.x * v / d;
	vy = vector.y * v / d;
    this->state = state;
}

/**
 * @brief Update the bullet position by its velocity and fly_dist by its movement per frame.
 * @details We don't detect whether to delete the bullet itself here because deleting a object itself doesn't make any sense.
 */
void
Bullet::update() {
	if(fly_dist == 0) return;
	DataCenter *DC = DataCenter::get_instance();
	double dx = vx / DC->FPS;
	double dy = vy / DC->FPS;
	double movement = Point::dist(Point{dx, dy}, Point{0, 0});
	if(fly_dist > movement) {
		shape->update_center_x(shape->center_x() + dx);
		shape->update_center_y(shape->center_y() + dy);
		fly_dist -= movement;
	} else {
		shape->update_center_x(shape->center_x() + dx * fly_dist / movement);
		shape->update_center_y(shape->center_y() + dy * fly_dist / movement);
		fly_dist = 0;
	}
}

void Bullet::update_matter(BulletState collid_matter) {  // 第一關子彈的剪刀石頭布
	if ((this->state == BulletState::SOLID && collid_matter == BulletState::GAS) ||
	    (this->state == BulletState::GAS && collid_matter == BulletState::LIQUID) ||
		(this->state == BulletState::LIQUID && collid_matter == BulletState::SOLID)) {
		this->dmg = 0;
	} else if ((this->state == BulletState::GAS && collid_matter == BulletState::SOLID) ||
			(this->state == BulletState::SOLID && collid_matter == BulletState::LIQUID) ||
			(this->state == BulletState::LIQUID && collid_matter == BulletState::GAS)) {
		this->dmg *= 2;
	}
	// 判斷兩個都是同樣的物質型態，讓速度變 0
}

bool Bullet::update_wave(int x, int y, double z, ToolType type) { // 第二關，撞到道具時的動作
	if (this->state == BulletState::SOUND)
		return true; // need to destroy collided tool
	if (this->state == BulletState::LASER) {
		if (type == ToolType::CONVEX) {

		} else if (type == ToolType::CONCAVE) {

		} else if (type == ToolType::MIRROR) {

		}
	}
	return false;
}

bool Bullet::update_electrode(BulletState collid_electrode) {
	if (this->state != BulletState::NEGATIVE || this->state != BulletState::POSITIVE)
		return false; // 這個子彈沒有電極 不理
	if (this->state == collid_electrode) {
		return false; // don't delete this bullet
	}
	// declare new Explode object
	return true; // need to delete this bullet
}
void Bullet::update_force(Point force_source) { // 第三關要用的
	// modify velocity of the bullet
}

void Bullet::draw() {
	DataCenter *DC = DataCenter::get_instance();
	Point offset = DC->camera->transform_object(*shape);
	al_draw_bitmap(
		bitmap,
		offset.center_x() - al_get_bitmap_width(bitmap) / 2,
		offset.center_y() - al_get_bitmap_height(bitmap) / 2, 0);
}
