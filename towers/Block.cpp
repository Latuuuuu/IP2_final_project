#include "Block.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Camera.h"
#include "../Hero.h"
#include "../shapes/Rectangle.h"
#include <algorithm>
#include <allegro5/bitmap_draw.h>
#include <string>
#include <iostream>
#include <allegro5/allegro_primitives.h>

Block::Block(const Point &p, double v, BlockState state) {
	ImageCenter *IC = ImageCenter::get_instance();
	// DataCenter *DC = DataCenter::get_instance();
    std::string path = std::string("./assets/image/block/") + (state == BlockState::ICE ? "ice" :state == BlockState::WATER ? "water" :state == BlockState::VAPOR ? "vapor" : "") + "_block.png";
	bitmap = IC->get(path);
    double width = al_get_bitmap_width(bitmap) * 0.65;
    double height = al_get_bitmap_height(bitmap) * 0.65;
	shape.reset(new Rectangle{p.x - width / 2, p.y - height / 2,p.x + width / 2, p.y + height / 2});
	// std::cout << "x1: " << p.x - width / 2 << " y1: " << p.y - height / 2 << " x2: " << p.x + width / 2 << " y2: " << p.y + height / 2 << "\n";
    vx = 0;
    vy = v;
    this->state = state;
}

/**
 * @brief Update the bullet position by its velocity and fly_dist by its movement per frame.
 * @details We don't detect whether to delete the bullet itself here because deleting a object itself doesn't make any sense.
 */
void
Block::update() {
	DataCenter *DC = DataCenter::get_instance();
	double dx = vx / DC->FPS;
	double dy = vy / DC->FPS;
    shape->update_center_x(shape->center_x() + dx);
	shape->update_center_y(shape->center_y() + dy);
}
    
void Block::draw() {
	DataCenter *DC = DataCenter::get_instance();
	Point offset = DC->camera->transform_object(*shape);
	al_draw_bitmap(
		bitmap,
		offset.center_x() - al_get_bitmap_width(bitmap) / 2,
		offset.center_y() - al_get_bitmap_height(bitmap) / 2, 0);
		// 測hitbox用 
	// double width = al_get_bitmap_width(bitmap) * 0.65;
    // double height = al_get_bitmap_height(bitmap) * 0.65;
	// al_draw_rectangle(
    //     offset.center_x() - width / 2,
    //     offset.center_y() - height / 2,
    //     offset.center_x() + width / 2,
    //     offset.center_y() + height / 2,
    //     al_map_rgb(255, 0, 0), 1.0);
}

void Block::update_hero_hit(BulletState hero_state) {
	DataCenter *DC = DataCenter::get_instance();
	Hero *hero = DC->hero;
	if ((this->state == BlockState::ICE && hero_state == BulletState::LIQUID) || (this->state == BlockState::WATER && hero_state == BulletState::GAS) || (this->state == BlockState::VAPOR && hero_state == BulletState::SOLID)) {
		hero->HP = 0;
	} else if ((this->state == BlockState::ICE && hero_state == BulletState::GAS) || (this->state == BlockState::WATER && hero_state == BulletState::SOLID) || (this->state == BlockState::VAPOR && hero_state == BulletState::LIQUID)) {
		this->alive = false;
	} else {
		hero->HP -= 1;
		double dis_y = hero->shape->center_y() - this->shape->center_y() * (vy ? 1 : -1);
		double ad_speed = vy * pow(2,(1+1/dis_y));
		hero->set_adjust_speed(0.0,ad_speed);
	}
}
