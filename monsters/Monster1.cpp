#include "Monster1.h"
#include "../Hero.h"
#include <iostream>
using namespace std;

void Monster1::attack() {
	// cout << "monster atk" << endl;
	DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();
	Bullet *atk;
	vector<string> atk_paths = {
		"assets/image/bullet/Ice_Ball.png",
		"assets/image/bullet/Water_Ball.png",
		"assets/image/bullet/Vapor_Ball.png"
	};
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
			
			atk = new Bullet(p, t, atk_paths[rand_state-1], 480, 10, 500, bullet_state);
		} else {
			const Point &t = dir_to_vector(dir);
			const Point &p = Point(t.x * max(graph_h, graph_w) + shape->center_x(), 
								t.y * max(graph_h, graph_w) + shape->center_y());
			atk = new Bullet(p, t, atk_paths[rand_state-1], 480, 10, 500, bullet_state);
			// atk = new Bullet(p, t, "assets/image/tower/Arcane_Beam.png", 480, 10, 500, static_cast<BulletState>(rand_state));
		}
		DC->bullets.emplace_back(atk);
}
