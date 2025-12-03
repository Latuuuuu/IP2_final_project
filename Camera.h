#ifndef CAMERA_H_INCLUDED //include 保護，避免重複include 
#define CAMERA_H_INCLUDED

#include "Object.h"
#include <map>
#include <string>

class Camera : public Object{
public:
    Camera(int w, int h);
    void update_camera(Point hero_position, Point dist_to_hero);
    void draw() override;
    Point transform_object(Shape &object_position);
    Point transform_bitmap(int x, int y);
    Point camera_to_global(Point point);
private:
    int camera_x;
    int camera_y;
    int w;
    int h;
};

#endif