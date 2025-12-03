#ifndef HERO_H_INCLUDED //include 保護，避免重複include 
#define HERO_H_INCLUDED

#include "Object.h"
#include "shapes/Point.h"
#include <map>
#include <string>

enum HeroState{
    LEFT,
    RIGHT,
    FRONT,
    BACK,
    HEROSTATE_MAX
};

class OperationCenter;

class Hero : public Object{
public:
    void init();
    void update();
    void draw() override;
    const int &get_dmg() const { return dmg; }
    const Point get_size() { return size; }
    friend class OperationCenter;
    int HP;
private:
    void jump_back(Point obj_point);
    HeroState state = HeroState::FRONT;
    std::map<HeroState, std::string> gifPath;
    Point size;
    double speed = 5.0;
    int shift_timer = 0,mouse_l_timer = 0;
    int dmg;
    bool is_collid;
};

#endif