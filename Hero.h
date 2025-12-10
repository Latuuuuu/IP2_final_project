#ifndef HERO_H_INCLUDED //include 保護，避免重複include 
#define HERO_H_INCLUDED

#include "Object.h"
#include "shapes/Point.h"
#include "towers/Bullet.h" //為了拿共用的bullet state
#include <map>
#include <string>
#include <vector>

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
    WAVE
};

class OperationCenter;

class Hero : public Object{
public:
    void init(int lvl);
    void update();
    void draw() override;
    const int &get_dmg() const { return dmg; }
    const Point get_size() { return size; }
    const int get_max_tool_num() { return max_tool_num; }
    friend class OperationCenter;
    int HP;
    void change_skill_state(SkillState new_state);
    bool all_skill = true; //全技能開關
    void set_adjust_speed(double dx, double dy) { adjust_speed_x = dx; adjust_speed_y = dy; }
private:
    void apply_skin(int);
    void jump_back(Point obj_point);
    void draw_tool_icon();
    HeroState state = HeroState::FRONT;
    SkillState skill_state = SkillState::NORMAL;
    BulletState bullet_state = BulletState::BALL;
    ToolType tool_type = ToolType::CONCAVE;
    std::map<HeroState, std::string> gifPath;
    std::vector<std::vector<std::string>> all_skin_paths;
    std::map<BulletState, std::string> bullet_gifPath;
    Point size;
    Point tool_place;
    double tool_angle;
    double speed = 250.0;
    double speed_x = 0.0;
    double speed_y = 0.0;
    double adjust_speed_x = 0.0;
    double adjust_speed_y = 0.0;
    int cd_time = 10;
    int shift_timer = 0,mouse_l_timer = 0;
    int dmg;
    bool is_collid;
    int max_tool_num;
    int level;
};

#endif