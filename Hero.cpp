#include "Hero.h"
#include "Camera.h"
#include "LevelT.h"
#include "towers/Tool.h"
#include "algif5/algif.h"
#include "data/GIFCenter.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
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
		"Normal_Ball","Ice_Ball", "Water_Ball", "Vapor_Ball", "Positive_Ball", "Negative_Ball", "BeamBullet", "SoundBullet"
	};
}

void Hero::init(int lvl) {
    for(size_t type = 0; type < static_cast<size_t>(HeroState::HEROSTATE_MAX); ++type){
        gifPath[static_cast<HeroState>(type)] = std::string(HeroSetting::hero_imgs_root_path) + "/dragonite_"+ HeroSetting::dir_path_postfix[type] + ".gif";
    }
    for(size_t type = 0; type < static_cast<size_t>(BulletState::BULLETSTATE_MAX); ++type){
        bullet_gifPath[static_cast<BulletState>(type)] = std::string(BulletSetting::hero_imgs_root_path) + "/" + BulletSetting::dir_path_postfix[type] + ".png";
    }
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gifPath[state]);
    DataCenter *DC = DataCenter::get_instance();
    size.x = gif->width;
    size.y = gif->height;
    this->dmg = 50;
    this->HP = 500;
    is_collid = false;
    if (lvl == 0) {
        this->level += 1;
    } else {
        this->level = lvl;
        shape.reset(new Rectangle{
            LevelSetting::hero_spawn_x[lvl-1] - size.x / 2 , DC->window_height / 2 - size.y / 2,
            LevelSetting::hero_spawn_x[lvl-1] + size.x / 2, DC->window_height / 2 + size.y / 2
        });
    }
    if (this->level == 2 || this->level == 4)
        this->max_tool_num = 5;
    else
        this->max_tool_num = 0;
}

void Hero::jump_back(Point obj_point) {
    shape->update_center_x(shape->center_x() * 2 - obj_point.center_x());
    shape->update_center_y(shape->center_y() * 2 - obj_point.center_y());
}

void Hero::update() {
    DataCenter *DC = DataCenter::get_instance();
    if (all_skill) {//技能組切換
        if (DC->key_state[ALLEGRO_KEY_1] && !DC->prev_key_state[ALLEGRO_KEY_1]) { 
            if (skill_state == SkillState::SLG) {
                skill_state = SkillState::NORMAL;
                bullet_state = BulletState::BALL;
            } else {
                skill_state = SkillState::SLG;
                bullet_state = BulletState::LIQUID;
            }
        } else if (DC->key_state[ALLEGRO_KEY_2] && !DC->prev_key_state[ALLEGRO_KEY_2]) {
            if (skill_state == SkillState::WAVE) {
                skill_state = SkillState::NORMAL;
                bullet_state = BulletState::BALL;
            } else {
                skill_state = SkillState::WAVE;
                bullet_state = BulletState::LASER;
            }
        } else if (DC->key_state[ALLEGRO_KEY_3] && !DC->prev_key_state[ALLEGRO_KEY_3]) {
            if (skill_state == SkillState::ELECTRIC) {
                skill_state = SkillState::NORMAL;
                bullet_state = BulletState::BALL;
            } else {
                skill_state = SkillState::ELECTRIC;
                bullet_state = BulletState::POSITIVE;
            }
        }
    }
    if (DC->key_state[ALLEGRO_KEY_Q] && !DC->prev_key_state[ALLEGRO_KEY_Q] && skill_state == SkillState::WAVE) {
        tool_type = ToolType::CONCAVE;
    } else if (DC->key_state[ALLEGRO_KEY_E] && !DC->prev_key_state[ALLEGRO_KEY_E] && skill_state == SkillState::WAVE) {
        tool_type = ToolType::CONVEX;
    } else if (DC->key_state[ALLEGRO_KEY_R] && !DC->prev_key_state[ALLEGRO_KEY_R] && skill_state == SkillState::WAVE && !DC->level->get_monster_spawn()) {
        tool_type = ToolType::MIRROR;
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
    const Point &hero_center = Point(shape->center_x(), shape->center_y());
    const Point &mouse = DC->camera->camera_to_global(DC->mouse);
    const double dist = Point::dist(hero_center, mouse) / 80;
    Point v = Point((mouse.center_x() - hero_center.center_x()) / dist, (mouse.center_y() - hero_center.center_y()) / dist);
    this->tool_angle = std::atan2(v.center_y(), v.center_x());
    tool_place = v + hero_center;
    // std::cout << tool_place.x << " " << tool_place.y << " " << this->tool_angle << std::endl;

    if (DC->mouse_state[2] && !DC->prev_mouse_state[2]) { //右鍵切換形態
        if (skill_state == SkillState::SLG) { //三態變化技
            if (bullet_state == BulletState::SOLID) {
                bullet_state = BulletState::LIQUID;
                std :: cout << "to liquid!\n";
            } else if (bullet_state == BulletState::LIQUID) {
                bullet_state = BulletState::GAS;
                std :: cout << "to gas!\n";
            } else if (bullet_state == BulletState::GAS) {
                bullet_state = BulletState::SOLID;
                std :: cout << "to solid!\n";
            } else {
                bullet_state = BulletState::LIQUID;
                std :: cout << "to solid!\n";
            }
        } else if (skill_state == SkillState::WAVE) { // 放置道具
            Tool *tool = new Tool(tool_place, this->tool_angle, this->tool_type);
            DC->tools.emplace_back(tool);
        } else if (skill_state == SkillState::ELECTRIC) { //正負電變化技
            if (bullet_state == BulletState::POSITIVE) {
                bullet_state = BulletState::NEGATIVE;
                std :: cout << "to negative!\n";
            } else if (bullet_state == BulletState::NEGATIVE) {
                bullet_state = BulletState::POSITIVE;
                std :: cout << "to positive!\n";
            } else {
                bullet_state = BulletState::POSITIVE;
                std :: cout << "to positive!\n";
            }
        }
    }
    
    if (DC->mouse_state[1] && !DC->prev_mouse_state[1] && mouse_l_timer <= 0) { //左鍵普攻 
        mouse_l_timer = cd_time;
        if (skill_state == SkillState::WAVE) {
            bullet_state = BulletState::LASER;
            mouse_l_timer = 1;
        }
        const Point &p = Point(shape->center_x(), shape->center_y());
		const Point &t = Point(mouse.center_x() - shape->center_x(), mouse.center_y() - shape->center_y());
        std::string bullet_path = bullet_gifPath[bullet_state];
		Bullet *atk = new Bullet(p, t, bullet_path, 480, 1, 500, bullet_state);
        DC->bullets.emplace_back(atk);
    }
    if (this->is_collid) {
        this->is_collid = false;
        return;
    }
    if (DC->key_state[ALLEGRO_KEY_W]) { //上下左右
        shape->update_center_y(shape->center_y() - speed);
        state = HeroState::BACK;
    } else if (DC->key_state[ALLEGRO_KEY_A]) {
        shape->update_center_x(shape->center_x() - speed);
        state = HeroState::LEFT;
    } else if (DC->key_state[ALLEGRO_KEY_S]) {
        shape->update_center_y(shape->center_y() + speed);
        state = HeroState::FRONT;
    } else if (DC->key_state[ALLEGRO_KEY_D]) {
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
    if (skill_state == SkillState::WAVE)
        draw_tool_icon();
}

void Hero::draw_tool_icon() {
	DataCenter *DC = DataCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	Point offset = DC->camera->transform_bitmap(tool_place.center_x(), tool_place.center_y());
	Point hero_pt = DC->camera->transform_object(*shape);
    // std::cout << "tp1:" << offset.x << " " << offset.y << std::endl;
    // std::cout << "hp1:" << hero_pt.x << " " << hero_pt.y << std::endl;
	std::string path = std::string(ToolSetting::root_path) + "/" + ToolSetting::tool_icon_path[static_cast<int>(this->tool_type)] + ".png";
	ALLEGRO_BITMAP *bitmap = IC->get(path);
    al_draw_rotated_bitmap(bitmap,
						al_get_bitmap_width(bitmap) / 2.0,
						al_get_bitmap_height(bitmap) / 2.0,
						offset.center_x(), 
						offset.center_y(), this->tool_angle, 0);
}