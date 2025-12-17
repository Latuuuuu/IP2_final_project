#include "Monster1.h"
#include "../Hero.h"
#include "../algif5/algif.h"
#include "../data/GIFCenter.h"
#include "../Camera.h"
#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <algorithm>
using namespace std;

vector<string> atk_paths = {
	"assets/image/bullet/Ice_Ball.png",
	"assets/image/bullet/Water_Ball.png",
	"assets/image/bullet/Vapor_Ball.png"
};

Monster1::Monster1(Point borned_place) : MonsterT(MonsterType::MONSTER1, borned_place) {
	// DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();
    GIFCenter *GIFC = GIFCenter::get_instance();
	HP = max_hp;
	v = 60;
	int monster_type_idx = static_cast<int>(MonsterType::MONSTER1);
    for(size_t type = 0; type <= static_cast<size_t>(Dir::RIGHT); ++type){
        // Construct path: ./assets/image/monster/Monster1_UP.gif
        string dir_str = MonsterSetting::dir_path_prefix[type];
        gifPath[static_cast<Dir>(type)] = string(MonsterSetting::monster_imgs_root_path[monster_type_idx]) + "_" + dir_str + ".gif";
        
        // Pre-load GIF
        GIFC->get(gifPath[static_cast<Dir>(type)]);
    }
	ALGIF_ANIMATION *gif = GIFC->get(gifPath[static_cast<Dir>(dir)]);
	graph_w = gif->width * 0.5;
	graph_h = gif->height * 0.5;
	shape.reset(new Rectangle{
		(shape->center_x() - graph_w / 2.), (shape->center_y() - graph_h / 2.),
		(shape->center_x() + graph_w / 2.), (shape->center_y() + graph_h / 2.)
	});
	force_shape.r = 0;
}

void Monster1::attack() {
	// cout << "monster atk" << endl;
	DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();
	Bullet *atk;
	
	if(!state_timer){
		rand_state = distribution(generator);
		state_timer = 120;
		bullet_state = static_cast<BulletState>(rand_state);
	}
	else{
		state_timer--;
	}
	if (action == Action::CHASE) {
		// cout << "chase and atk" << endl;
			const Point &t = Point(DC->hero->shape->center_x() - shape->center_x(), DC->hero->shape->center_y() - shape->center_y());
			double d = Point::dist(t);
			const Point &p = Point(t.x / d * max(graph_h, graph_w) + shape->center_x(), 
								t.y / d * max(graph_h, graph_w) + shape->center_y());
			
			atk = new Bullet(p, t, BulletSetting::bullet_paths[rand_state], 480, BulletSetting::bullet_dmgs[rand_state], 500, bullet_state);
		} else {
			const Point &t = dir_to_vector(dir);
			const Point &p = Point(t.x * max(graph_h, graph_w) + shape->center_x(), 
								t.y * max(graph_h, graph_w) + shape->center_y());
			atk = new Bullet(p, t, BulletSetting::bullet_paths[rand_state], 480, BulletSetting::bullet_dmgs[rand_state], 500, bullet_state);
			// atk = new Bullet(p, t, "assets/image/tower/Arcane_Beam.png", 480, 10, 500, static_cast<BulletState>(rand_state));
		}
		DC->matterBullets.emplace_back(atk);
}

void Monster1::draw() {
    DataCenter *DC = DataCenter::get_instance();
    GIFCenter *GIFC = GIFCenter::get_instance();
    
    // Get current GIF based on direction
    ALGIF_ANIMATION *gif = GIFC->get(gifPath[static_cast<Dir>(dir)]);
	Point offset = DC->camera->transform_object(*shape);
	algif_draw_gif(
		gif,
		offset.center_x() - gif->width / 2,
		offset.center_y() - gif->height / 2 + 15 , 0);
	
	ALLEGRO_COLOR bar_color;
	Hero *hero = DC->hero;
	if((hero->get_bullet_state() == BulletState::SOLID && bullet_state == BulletState::LIQUID) || 
	   (hero->get_bullet_state() == BulletState::LIQUID && bullet_state == BulletState::GAS) ||
	   (hero->get_bullet_state() == BulletState::GAS && bullet_state == BulletState::SOLID)){
		bar_color = al_map_rgb(255, 190, 0); // golden
	}
	else if(hero->get_bullet_state() == bullet_state){
		bar_color = al_map_rgb(220, 20, 60); // red
	}else{
		bar_color = al_map_rgb(100, 100, 100); // gray
	}
	float bar_w = 120;
    float bar_h = 8;
    float bar_x = offset.center_x() - (bar_w / 2);
    float bar_y = offset.center_y() - (graph_h / 2) - 25;
	float hp_ratio = (float)this->HP / (float)max_hp;
    if(hp_ratio < 0) hp_ratio = 0;
	al_draw_filled_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, al_map_rgb(50, 50, 50));
    al_draw_filled_rectangle(bar_x, bar_y, bar_x + (bar_w * hp_ratio), bar_y + bar_h, bar_color);
    al_draw_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, bullet_state == BulletState::GAS ? al_map_rgb(255, 255, 255) :bullet_state == BulletState::SOLID ? al_map_rgb(124, 189, 255) : al_map_rgb(108, 190, 255), 2);

	// al_draw_rectangle(
    //     offset.center_x() - graph_w / 2,
    //     offset.center_y() - graph_h / 2,
    //     offset.center_x() + graph_w / 2,
    //     offset.center_y() + graph_h / 2,
    //     al_map_rgb(255, 0, 0), 1.0);
}