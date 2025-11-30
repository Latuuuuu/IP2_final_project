#include "Camera.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "shapes/Point.h"
#include "Hero.h"

using namespace std;

Camera::Camera() {
    DataCenter *DC = DataCenter::get_instance();
    this->camera_x = DC->hero->shape->center_x();
    this->camera_y =  DC->hero->shape->center_y();
    this->w = DC->window_width / 2;
    this->h = DC->window_height / 2;
}
void Camera::draw() {
    return;
}
void Camera::update_camera(Point &dist_to_hero) {
    this->camera_x += dist_to_hero.center_x();
    this->camera_y += dist_to_hero.center_y();
}
Point Camera::transform_bitmap(int x, int y) {
    Point p(x - this->camera_x + w, y - this->camera_y + h);
    return p;
}
Point Camera::transform_object(Shape &object_position) {
    Point p(object_position.center_x() - this->camera_x + w, object_position.center_y() - this->camera_y + h);
    return p;
}