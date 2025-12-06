#include "Monster1.h"
#include "../Hero.h"
#include "../towers/Bullet.h"
#include <iostream>
using namespace std;

void Monster1::attack() {
	// cout << "monster atk" << endl;
	DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();
	Bullet *atk;
	if (action == Action::CHASE) {
		// cout << "chase and atk" << endl;
		const Point &t = Point(DC->hero->shape->center_x() - shape->center_x(), DC->hero->shape->center_y() - shape->center_y());
		double d = Point::dist(t);
		const Point &p = Point(t.x / d * max(graph_h, graph_w) + shape->center_x(), 
							   t.y / d * max(graph_h, graph_w) + shape->center_y());
		atk = new Bullet(p, t, "assets/image/tower/Arcane_Beam.png", 480, 20, 500);
	} else {
		const Point &t = dir_to_vector(dir);
		const Point &p = Point(t.x * max(graph_h, graph_w) + shape->center_x(), 
							   t.y * max(graph_h, graph_w) + shape->center_y());
		atk = new Bullet(p, t, "assets/image/tower/Arcane_Beam.png", 480, 20, 500);
	}
	DC->bullets.emplace_back(atk);
}
