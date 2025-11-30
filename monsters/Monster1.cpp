#include "Monster1.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Hero.h"
#include "../towers/Bullet.h"

using namespace std;

void Monster1::attack() {
	DataCenter *DC = DataCenter::get_instance();
	const Point &p = Point(shape->center_x(), shape->center_y());
	Bullet *atk;
	if (action == Action::CHASE) {
		const Point &t = Point(DC->hero->shape->center_x(), DC->hero->shape->center_y());
		atk = new Bullet(p, t, "assets/image/tower/Arcane_Beam.png", 480, 20, 500);
	} else {
		const Point &t = dir_to_vector(dir);
		atk = new Bullet(p, t, "assets/image/tower/Arcane_Beam.png", 480, 20, 500);
	}
	DC->towerBullets.emplace_back(atk);
}
