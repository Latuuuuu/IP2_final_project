#ifndef LINE_H_INCLUDED
#define LINE_H_INCLUDED

#include "Shape.h"
#include "Point.h"
#include <cmath>
#include <iostream>

/**
 * @see Shape.cpp
 */
class Line : public Shape
{
public:
	bool overlap(const Shape &s) const;
	double center_x() const { return (x1 + x2) / 2; }
	double center_y() const { return (y1 + y2) / 2; }
	void update_center_x(const double &x) {
		double dx = x - center_x();
		x1 += dx, x2 += dx;
	}
	void update_center_y(const double &y) {
		double dy = y - center_y();
		y1 += dy, y2 += dy;
	}
	void update_center_z(const double &z, const Point &anchor) {
		Point v;
		v.x = x1 - anchor.center_x();
		v.y = y1 - anchor.center_y();
		x1 = std::cos(z)*v.x-std::sin(z)*v.y + anchor.center_x();
		y1 = std::sin(z)*v.x+std::cos(z)*v.y + anchor.center_y();
		v.x = x2 - anchor.center_x();
		v.y = y2 - anchor.center_y();
		x2 = std::cos(z)*v.x-std::sin(z)*v.y + anchor.center_x();
		y2 = std::sin(z)*v.x+std::cos(z)*v.y + anchor.center_y();
		// std::cout << x1 << " " << y1 << ", " << x2 << " " << y2 << std::endl;
	}
	const ShapeType getType() const { return ShapeType::LINE; }
public:
	Line() {}
	Line(double x1, double y1, double x2, double y2) : x1{x1}, y1{y1}, x2{x2}, y2{y2} {}
	Line(int x1, int y1, int x2, int y2) :
		x1{static_cast<double>(x1)},
		y1{static_cast<double>(y1)},
		x2{static_cast<double>(x2)},
		y2{static_cast<double>(y2)} {}
	double x1, y1, x2, y2;
};

#endif
