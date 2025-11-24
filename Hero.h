#ifndef HERO_H_INCLUDED //include 保護，避免重複include 
#define HERO_H_INCLUDED

#include "Object.h"
#include <map>
#include <string>

enum HeroState{
    LEFT,
    RIGHT,
    FRONT,
    BACK,
    HEROSTATE_MAX
};

class Hero : public Object{
public:
    void init();
    void update();
    void draw() override;
private:
    HeroState state = HeroState::FRONT;
    std::map<HeroState, std::string> gifPath;
    double speed = 5.0;
    int shift_timer = 0,mouse_l_timer = 0;
};

#endif