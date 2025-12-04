#include "Hero.h"
#include "Camera.h"
#include "algif5/algif.h"
#include "data/GIFCenter.h"
#include "data/DataCenter.h"
#include "shapes/Rectangle.h"
#include <iostream>

namespace HeroSetting {
	static constexpr char hero_imgs_root_path[40] = "./assets/gif/Hero";
	static constexpr char dir_path_postfix[][10] = {
		"left", "right", "front", "back"
	};
}

namespace BulletSetting {
	static constexpr char hero_imgs_root_path[40] = "./assets/image/bullet";
	static constexpr char dir_path_postfix[][20] = {
		"Normal_Ball","Ice_Ball", "Water_Ball", "Vapor_Ball", "Positive_Ball", "Negative_Ball"
	};
}

void Hero::init() {
    for(size_t type = 0; type < static_cast<size_t>(HeroState::HEROSTATE_MAX); ++type){
        gifPath[static_cast<HeroState>(type)] = std::string(HeroSetting::hero_imgs_root_path) + "/dragonite_"+ HeroSetting::dir_path_postfix[type] + ".gif";
    }
    for(size_t type = 0; type < static_cast<size_t>(BulletState::BULLETSTATE_MAX); ++type){
        bullet_gifPath[static_cast<BulletState>(type)] = std::string(BulletSetting::hero_imgs_root_path) + "/" + BulletSetting::dir_path_postfix[type] + ".png";
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
    size.x = gif->width;
    size.y = gif->height;
    this->dmg = 50;
    this->HP = 500;
    is_collid = false;
}

void Hero::jump_back(Point obj_point) {
    shape->update_center_x(shape->center_x() * 2 - obj_point.center_x());
    shape->update_center_y(shape->center_y() * 2 - obj_point.center_y());
}

void Hero::update() {
    DataCenter *DC = DataCenter::get_instance();
    if(all_skill){//技能組切換
        if(DC->key_state[ALLEGRO_KEY_1] && !DC->prev_key_state[ALLEGRO_KEY_1]){ 
            if(skill_state == SkillState::SLG){
                skill_state = SkillState::NORMAL;
                bullet_state = BulletState::BALL;
            }
            else{
                skill_state = SkillState::SLG;
                bullet_state = BulletState::LIQUID;
            }
        }else if(DC->key_state[ALLEGRO_KEY_3] && !DC->prev_key_state[ALLEGRO_KEY_3]){
            if(skill_state == SkillState::ELECTRIC){
                skill_state = SkillState::NORMAL;
                bullet_state = BulletState::BALL;
            }
            else{
                skill_state = SkillState::ELECTRIC;
                bullet_state = BulletState::POSITIVE;
            }
        }
    }

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

    if(DC->mouse_state[2] && !DC->prev_mouse_state[2]){ //右鍵切換形態
        if(skill_state == SkillState::SLG){ //三態變化技
            if(bullet_state == SOLID){
                bullet_state = LIQUID;
                std :: cout << "to liquid!\n";
            }else if(bullet_state == LIQUID){
                bullet_state = GAS;
                std :: cout << "to gas!\n";
            }else if(bullet_state == GAS){
                bullet_state = SOLID;
                std :: cout << "to solid!\n";
            }else{
                bullet_state = LIQUID;
                std :: cout << "to solid!\n";
            }
        }else if(skill_state == SkillState::ELECTRIC){ //正負電變化技
            if(bullet_state == POSITIVE){
                bullet_state = NEGATIVE;
                std :: cout << "to negative!\n";
            }else if(bullet_state == NEGATIVE){
                bullet_state = POSITIVE;
                std :: cout << "to positive!\n";
            }else{
                bullet_state = POSITIVE;
                std :: cout << "to positive!\n";
            }
        }
    }
    
    if(DC->mouse_state[1] && !DC->prev_mouse_state[1] && mouse_l_timer <= 0){ //左鍵普攻 
        const Point &p = Point(shape->center_x(), shape->center_y());
        const Point &mouse = DC->camera->camera_to_global(DC->mouse);
		const Point &t = Point(mouse.center_x() - shape->center_x(), mouse.center_y() - shape->center_y());
        std::string bullet_path = bullet_gifPath[bullet_state];
		Bullet *atk = new Bullet(p, t, bullet_path, 480, 1, 500, bullet_state);
        DC->towerBullets.emplace_back(atk);
        mouse_l_timer = 10.0;
    }
    if (this->is_collid) {
        this->is_collid = false;
        return;
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
    DataCenter *DC = DataCenter::get_instance();
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gifPath[state]);
    Point offset = DC->camera->transform_object(*shape);
    algif_draw_gif(
        gif,
        offset.center_x() - gif->width / 2.0,
        offset.center_y() - gif->height / 2.0,
        0
    );
}