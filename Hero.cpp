#include "Hero.h"
#include "algif5/algif.h"
#include "data/GIFCenter.h"
#include "data/DataCenter.h"
#include "shapes/Rectangle.h"

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
    if(DC->key_state[ALLEGRO_KEY_W]){
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