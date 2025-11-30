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
void Camera::update_camera(Point hero_position, Point dist_to_hero) {
    this->camera_x = hero_position.center_x() + dist_to_hero.center_x();
    this->camera_y = hero_position.center_y() + dist_to_hero.center_y();
}
Point Camera::transform_bitmap(int x, int y) {
    Point p(x - this->camera_x + w, y - this->camera_y + h);
    return p;
}
Point Camera::transform_object(Shape &object_position) {
    Point p(object_position.center_x() - this->camera_x + w, object_position.center_y() - this->camera_y + h);
    return p;
}