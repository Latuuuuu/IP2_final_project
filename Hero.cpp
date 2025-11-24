#include "Hero.h"
#include "algif5/algif.h"
#include "data/GIFCenter.h"
#include "data/DataCenter.h"
#include "shapes/Rectangle.h"
#include "towers/Bullet.h"
#include <iostream>

namespace HeroSetting {
	static constexpr char hero_imgs_root_path[40] = "./assets/gif/Hero";
	static constexpr char dir_path_postfix[][10] = {
		"left", "right", "front", "back"
	};
}

void Hero::init() {
    for(size_t type = 0; type < static_cast<size_t>(HeroState::HEROSTATE_MAX); ++type){
        gifPath[static_cast<HeroState>(type)] = std::string(HeroSetting::hero_imgs_root_path) + "/dragonite_"+ HeroSetting::dir_path_postfix[type] + ".gif";
    }
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gifPath[state]);
    DataCenter *DC = DataCenter::get_instance();
    shape.reset(new Rectangle{
        DC->window_width / 2.0 ,
        DC->window_height / 2.0 ,
        DC->window_width / 2.0 + gif->width,
        DC->window_height / 2.0 + gif->height
    });
}

void Hero::update() {
    DataCenter *DC = DataCenter::get_instance();

    if(shift_timer >= 0){ //shift 加速 timer
        if(shift_timer<=50) speed = 5.0;
        shift_timer--;
    }
    if(mouse_l_timer >= 0){ //left mouse 普攻 timer
        mouse_l_timer--;
    }

    if(DC->key_state[ALLEGRO_KEY_LSHIFT] && shift_timer<=0){ //shift 加速
        std :: cout << "fast!\n";
        shift_timer = 60;
        speed = 10.0;
    }

    if(DC->mouse_state[1] == 1 && mouse_l_timer <= 0){ //左鍵普攻 
        std :: cout << "atk!\n";
        const Point &p = Point(shape->center_x(), shape->center_y());
		const Point &t = Point(DC->mouse.center_x(), DC->mouse.center_y());
		Bullet *atk = new Bullet(p, t, "assets/image/tower/Arcane_Beam.png", 480, 20, 500);
        DC->towerBullets.emplace_back(atk);
        mouse_l_timer = 10.0;
    }

    if(DC->key_state[ALLEGRO_KEY_W]){ //上下左右
        shape->update_center_y(shape->center_y() - speed);
        state = HeroState::BACK;
    }else if(DC->key_state[ALLEGRO_KEY_A]){
        shape->update_center_x(shape->center_x() - speed);
        state = HeroState::LEFT;
    }else if(DC->key_state[ALLEGRO_KEY_S]){
        shape->update_center_y(shape->center_y() + speed);
        state = HeroState::FRONT;
    }else if(DC->key_state[ALLEGRO_KEY_D]){
        shape->update_center_x(shape->center_x() + speed);
        state = HeroState::RIGHT;
    }
}

void Hero::draw() {
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gifPath[state]);
    algif_draw_gif(
        gif,
        shape->center_x() - gif->width / 2.0,
        shape->center_y() - gif->height / 2.0,
        0
    );
}