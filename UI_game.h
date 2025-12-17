#ifndef UIGAME_H_INCLUDED
#define UIGAME_H_INCLUDED

#include "Utils.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "UI_T.h"
#include "string"

class UI_game : public UI {
public:
    UI_game() {}
    void init() override;
    void update();
    void draw() override;
private:
    void draw_player_hp();
    void draw_skill_status();
    std::string hp_path = "./assets/image/UI/HP_bar.png";
};

#endif
