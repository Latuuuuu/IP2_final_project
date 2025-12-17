#ifndef SQUARE_H_INCLUDED
#define SQUARE_H_INCLUDED

#include "Shape.h"
#include "Line.h"
#include "Point.h"

/**
 * @see Shape.cpp
 */
class Square : public Shape
{
public:
	bool overlap(const Shape &s) const;
	double center_x() const { return x; }
	double center_y() const { return y; }
	void update_center_x(const double &x) {
		this->x += x;
	}
	void update_center_y(const double &y) {
		this->y += y;
	}
	const ShapeType getType() const { return ShapeType::SQUARE; }
public:
	Square(double x, double y, double len, double z) : x{x}, y{y}, len{len}, z{z} {
		edge_up = new Line(x-len/2, y-len/2, x+len/2, y-len/2);
		edge_right = new Line(x+len/2, y-len/2, x+len/2, y+len/2);
		edge_down = new Line(x-len/2, y+len/2, x+len/2, y+len/2);
		edge_left = new Line(x-len/2, y-len/2, x-len/2, y+len/2);
		edge_up->update_center_z(z, Point(x, y));
		edge_right->update_center_z(z, Point(x, y));
		edge_down->update_center_z(z, Point(x, y));
		edge_left->update_center_z(z, Point(x, y));
	}
	Square(int x, int y, int len, int z) :
		x{static_cast<double>(x)},
		y{static_cast<double>(y)},
		len{static_cast<double>(len)},
		z{static_cast<double>(z)} {
		edge_up = new Line(x-len/2, y-len/2, x+len/2, y-len/2);
		edge_right = new Line(x+len/2, y-len/2, x+len/2, y+len/2);
		edge_down = new Line(x-len/2, y+len/2, x+len/2, y+len/2);
		edge_left = new Line(x-len/2, y-len/2, x-len/2, y+len/2);
		edge_up->update_center_z(z, Point(x, y));
		edge_right->update_center_z(z, Point(x, y));
		edge_down->update_center_z(z, Point(x, y));
		edge_left->update_center_z(z, Point(x, y));
	}
	double x, y, len, z;
	Line *edge_up, *edge_right, *edge_down, *edge_left;
};

#endif
