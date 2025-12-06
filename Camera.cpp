#include "Camera.h"
#include "shapes/Point.h"
#include <iostream>
using namespace std;

Camera::Camera(int w, int h) {
    this->camera_x = w / 2.0;
    this->camera_y =  h / 2.0;
    cout << this->camera_x << " " << this->camera_y << endl;
    this->w = w / 2.0;
    this->h = h / 2.0;
}
void Camera::draw() {
    return;
}
void Camera::update_camera(Point hero_position, Point dist_to_hero) { // 給定角色位置以及希望相機與角色的距離
    this->camera_x = hero_position.center_x() + dist_to_hero.center_x();
    this->camera_y = hero_position.center_y() + dist_to_hero.center_y();
}
Point Camera::transform_bitmap(int x, int y) { // 從 global 轉到相機坐標系 (input 是 int)
    Point p(x - this->camera_x + w, y - this->camera_y + h);
    return p;
}
Point Camera::transform_object(Shape &object_position) { // 從 global 轉到相機坐標系 (input 是 Shape)
    Point p(object_position.center_x() - this->camera_x + w, object_position.center_y() - this->camera_y + h);
    return p;
}
Point Camera::camera_to_global(Point point) { // 從相機坐標系轉到 global
    Point p(point.center_x() + this->camera_x - w, point.center_y() + this->camera_y - h);
    return p;
}