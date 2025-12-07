#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include "../Object.h"
#include "../shapes/Rectangle.h"
#include "Bullet.h"
#include <allegro5/bitmap.h>
#include <string>

/**
 * @brief The block in level one
 */

enum class BlockState{ // 使用 enum class 比較安全
    ICE,
    WATER,
    VAPOR,
};

class Block : public Object
{
public:
	Block(const Point &p, double v, BlockState state);
	void update();
	void draw();
	const BlockState get_state() { return state; }
	void update_hero_hit(BulletState hero_state);
	void update_bullet_hit(BulletState hero_state);
	bool alive = true;
	// void update_matter(BulletState collid_matter); // 剪刀石頭布
	// void update_force(Point force_source); // 處理引力與斥力
	// bool update_electrode(BulletState collid_electrode); // 正負極碰撞的時候要爆炸
	// bool update_wave(int x, int y, double z, ToolType type); // 子彈碰到道具要轉彎
	// Circle force_shape; // 放 public 有點不安全，要考慮改成 private
private:
	/**
	 * @brief Velocity in x direction.
	 */
	double vx;
	/**
	 * @brief Velocity in y direction.
	 */
	double vy;
	ALLEGRO_BITMAP *bitmap;
    BlockState state;
};

#endif
