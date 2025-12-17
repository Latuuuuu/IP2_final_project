#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "../Object.h"
#include "Tool.h"
#include "../shapes/Line.h"
#include "../shapes/Circle.h"
#include <allegro5/bitmap.h>
#include <string>
#include <utility>

/**
 * @brief The bullet shot from Tower.
 * @see Tower
 */

enum class BulletState{ // 使用 enum class 比較安全
    BALL,
    SOLID,
    LIQUID,
    GAS,
	POSITIVE,
	NEGATIVE,
	LASER,
	SOUND,
    BULLETSTATE_MAX
};
namespace BulletSetting {
	const std::array<std::string, static_cast<int>(BulletState::BULLETSTATE_MAX)> bullet_paths = {
		"assets/image/bullet/Normal_Ball.png",
		"assets/image/bullet/Ice_Ball.png",
		"assets/image/bullet/Water_Ball.png",
		"assets/image/bullet/Vapor_Ball.png",
		"assets/image/bullet/Positive_Ball.png",
		"assets/image/bullet/Negative_Ball.png",
		"assets/image/bullet/BeamBullet.png",
		"assets/image/bullet/SoundBullet.png"
	};
	const std::array<int, static_cast<int>(BulletState::BULLETSTATE_MAX)> bullet_dmgs = {
		10, 50, 50, 50, 50, 50, 1, 0
	};
	const std::array<std::string, 3> explode_paths = {
		"assets/gif/explode.gif",
		"assets/gif/explode1.gif",
		"assets/gif/explode2.gif"
	};
}

class Bullet : public Object
{
public:
	Bullet(const Point &p, const Point &target, const std::string &path, double v, int dmg, double fly_dist);
	Bullet(const Point &p, const Point &target, const std::string &path, double v, int dmg, double fly_dist, BulletState state);
	void update();
	void draw();
	const double &get_fly_dist() const { return fly_dist; }
	const int &get_dmg() const { return dmg; }
	const BulletState get_state() { return state; }
	void update_matter(BulletState collid_matter); // 剪刀石頭布
	bool update_electrode(BulletState collid_electrode); // 正負極碰撞的時候要爆炸
	bool update_wave(int x, int y, double z, ToolType type, std::pair<Point, Point> focal); // 子彈碰到道具要轉彎
	//
	Circle force_shape; // 放 public 有點不安全，要考慮改成 private
	bool alive = true;
	std::pair<double,double> get_speed() { return std::make_pair(vx, vy); }
	void set_adjust_speed(double dx, double dy) { vx += dx; vy += dy; }
	int e;
private:
	/**
	 * @brief Velocity in x direction.
	 */
	double vx;
	/**
	 * @brief Velocity in y direction.
	 */
	double vy;
	/**
	 * @brief Flying distance limit of the bullet. If the flying distance exceeds the limit, the bullet should be erased.
	 */
	double fly_dist;
	/**
	 * @brief Base damage of the bullet when hit anything.
	 */
	int dmg;
	/**
	 * @brief ALLEGRO_BITMAP of the bullet.
	 */
	ALLEGRO_BITMAP *bitmap;
    BulletState state;
};

#endif
