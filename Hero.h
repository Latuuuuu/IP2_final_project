#ifndef HERO_H_INCLUDED //include 保護，避免重複include 
#define HERO_H_INCLUDED

#include "Object.h"
#include "shapes/Point.h"
#include "towers/Bullet.h" //為了拿共用的bullet state
#include <map>
#include <string>

enum HeroState{
    LEFT,
    RIGHT,
    FRONT,
    BACK,
    HEROSTATE_MAX
};

enum SkillState{
    NORMAL,
    SLG, //solid liguid gas
    ELECTRIC,
};

// enum BulletState{
//     BALL,
//     SOLID,
//     LIQUID,
//     GAS,
//     BULLETSTATE_MAX

// };

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
    void change_skill_state(SkillState new_state){skill_state = new_state;};
    bool all_skill = true; //全技能開關
private:
    void jump_back(Point obj_point);
    HeroState state = HeroState::FRONT;
    SkillState skill_state = SkillState::NORMAL;
    BulletState bullet_state = BulletState::BALL;
    std::map<HeroState, std::string> gifPath;
    std::map<BulletState, std::string> bullet_gifPath;
    Point size;
    double speed = 5.0;
    int shift_timer = 0,mouse_l_timer = 0;
    int dmg;
    bool is_collid;
};

#endif