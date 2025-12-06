#include "Shape.h"
#include "Point.h"
#include "Rectangle.h"
#include "Circle.h"
#include "Line.h"
#include "../Utils.h"
#include <algorithm>

using namespace std;

bool checkOverlap(const Line *l1, const Line *l2);
/**
 * @file Shape.cpp
 * @brief All functions and implementations are defined here.
 */
bool rect_contains_point(const Rectangle *r, const Point *p) {
    return (p->x >= r->x1 && p->x <= r->x2 &&
            p->y >= r->y1 && p->y <= r->y2);
}

/**
 * @brief 判斷三個點的相對方向 (用於線段交集)。
 * @return 0: 共線 (Collinear), 1: 順時針 (Clockwise), 2: 逆時針 (Counterclockwise)
 */
int orientation(const Line *l, const Point *r) {
    // 使用叉積 (Cross Product) 的概念來判斷
    double val = (l->y2 - l->y1) * (r->x - l->x2) - (l->x2 - l->x1) * (r->y - l->y2);
    
    // 考慮浮點數誤差
    if (abs(val) < 1e-6) return 0; 
    
    return (val > 0) ? 1 : 2; 
}

bool checkOverlap(const Point *p1, const Point *p2) {
	return (p1->x == p2->x) && (p1->y == p2->y);
}

bool checkOverlap(const Point *p, const Rectangle *r) {
	return (r->x1 <= p->x && p->x <= r->x2) && (r->y1 <= p->y && p->y <= r->y2);
}

bool checkOverlap(const Point *p, const Circle *c) {
	return (c->r * c->r) <= Point::dist2((*p), Point(c->x, c->y));
}

bool checkOverlap(const Point *p, const Line *l) {
	return ((p->x <= max(l->x1, l->x2) && p->x >= min(l->x1, l->x2) && // x 在線段範圍內
            p->y <= max(l->y1, l->y2) && p->y >= min(l->y1, l->y2)) && // y 在線段範圍內
			orientation(l, p) == 0); // 共線
}

bool checkOverlap(const Rectangle *r1, const Rectangle *r2) {
	return !(r1->x2 < r2->x1 || r2->x2 < r1->x1 || r1->y2 < r2->y1 || r2->y2 < r1->y1);
}

bool checkOverlap(const Rectangle *r, const Circle *c) {
	double x = max(r->x1, min(c->x, r->x2));
	double y = max(r->y1, min(c->y, r->y2));
	return (c->r * c->r) >= Point::dist2(Point(c->x, c->y), Point(x, y));
}

bool checkOverlap(const Rectangle *r, const Line *l) {
    // 1. Trivial Acceptance Check: 檢查線段的任一端點是否在矩形內部
    if (rect_contains_point(r, new Point(l->x1, l->y1)) || rect_contains_point(r, new Point(l->x2, l->y2))) {
        return true;
    }

    // 3. 定義矩形的四個邊緣線段
    Line edges[4] = {
        // Top edge
        {r->x1, r->y1, r->x2, r->y1}, 
        // Bottom edge
        {r->x1, r->y2, r->x2, r->y2}, 
        // Left edge
        {r->x1, r->y1, r->x1, r->y2}, 
        // Right edge
        {r->x2, r->y1, r->x2, r->y2}
    };

    // 4. 檢查線段與四個邊緣是否有交集
    for (int i = 0; i < 4; ++i) {
        if (checkOverlap(l, &edges[i])) {
            return true;
        }
    }

    // 5. 既沒有端點在內部，也沒有與邊緣相交，則不重疊。
    return false;
}

bool checkOverlap(const Circle *c1, const Circle *c2) {
	double d = c1->r + c2->r;
	return (d * d) >= Point::dist2(Point(c1->x, c1->y), Point(c2->x, c2->y));
}

bool checkOverlap(const Circle *c, const Line *l) {
    // 1. 計算向量
    // Vector AB (V = B - A)
    float v_x = l->x2 - l->x1;
    float v_y = l->y2 - l->y1;
    // Vector AC (W = C - A)
    float w_x = c->x - l->x1;
    float w_y = c->y - l->y1;
    // 2. 計算向量 AB 的長度平方 (|V|^2)
    float length_sq = v_x * v_x + v_y * v_y;
    // 3. 計算投影參數 t ( t = (AC . AB) / |AB|^2 )
    // t 代表線段 AB 上最近點的相對位置，t=0 是 A 點，t=1 是 B 點。
    float dot_product = w_x * v_x + w_y * v_y;
    float t = dot_product / length_sq;
    // 4. 夾持 t 於 [0, 1] 之間
    // t_closest 代表線段上（或端點）距離圓心最近的點的參數。
    float t_closest = std::max(0.0f, std::min(1.0f, t));
    // 5. 計算線段上距離圓心最近的點 P
    // P = A + t_closest * V
	const Point& C = Point(c->x, c->y);
    Point P(l->x1 + t_closest * v_x, l->y1 + t_closest * v_y);
    // 6. 檢查 P 到圓心 C 的距離平方是否小於等於半徑平方 R^2
    return Point::dist2(C, P) <= (c->r * c->r);
}

/**
 * @brief 檢查兩條線段 s1 和 s2 是否相交。
 * @note 使用 General Case + Special Case (Collinear) 算法。
 */
bool checkOverlap(const Line *l1, const Line *l2) {
    // 1. 檢查一般情況 (General Case)
    int o1 = orientation(l1, new Point(l2->x1, l2->y1));
    int o2 = orientation(l1, new Point(l2->x2, l2->y1));
    int o3 = orientation(l2, new Point(l1->x1, l1->y1));
    int o4 = orientation(l2, new Point(l1->x2, l1->y2));

    if (o1 != o2 && o3 != o4) {
        return true; // 一般交集 (線段交叉)
    }
    return false; // 沒有交集或共線交集
}

bool Point::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(this, static_cast<const Point*>(&s));
		case ShapeType::RECTANGLE: return checkOverlap(this, static_cast<const Rectangle*>(&s));
		case ShapeType::CIRCLE: return checkOverlap(this, static_cast<const Circle*>(&s));
		case ShapeType::LINE: return checkOverlap(this, static_cast<const Line*>(&s));
	}
	GAME_ASSERT(false, "Unknown ShapeType.");
}

bool Rectangle::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(static_cast<const Point*>(&s), this);
		case ShapeType::RECTANGLE: return checkOverlap(this, static_cast<const Rectangle*>(&s));
		case ShapeType::CIRCLE: return checkOverlap(this, static_cast<const Circle*>(&s));
		case ShapeType::LINE: return checkOverlap(this, static_cast<const Line*>(&s));
	}
	GAME_ASSERT(false, "Unknown ShapeType.");
}

bool Circle::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(static_cast<const Point*>(&s), this);
		case ShapeType::RECTANGLE: return checkOverlap(static_cast<const Rectangle*>(&s), this);
		case ShapeType::CIRCLE: return checkOverlap(this, static_cast<const Circle*>(&s));
		case ShapeType::LINE: return checkOverlap(this, static_cast<const Line*>(&s));
	}
	GAME_ASSERT(false, "Unknown ShapeType.");
}

bool Line::overlap(const Shape &s) const {
	switch(s.getType()) {
		case ShapeType::POINT: return checkOverlap(static_cast<const Point*>(&s), this);
		case ShapeType::RECTANGLE: return checkOverlap(static_cast<const Rectangle*>(&s), this);
		case ShapeType::CIRCLE: return checkOverlap(static_cast<const Circle*>(&s), this);
		case ShapeType::LINE: return checkOverlap(this, static_cast<const Line*>(&s));
	}
	GAME_ASSERT(false, "Unknown ShapeType.");
}