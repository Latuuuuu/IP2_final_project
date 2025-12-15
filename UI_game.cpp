#include "UI_game.h"
#include "Hero.h"
#include "towers/Bullet.h"
#include "data/FontCenter.h"
#include <allegro5/allegro_primitives.h>

void UI_game::init() {
    // 因為沒有商店按鈕，所以這裡清空按鈕列表，確保不會誤點
    button_items.clear();
    
    // 設定狀態為 HALT (雖然沒有互動，但保持狀態一致性)
    state = STATE::HALT;
    on_item = -1;

    debug_log("<UI_game> HUD Initialized.\n");
}

void UI_game::draw() {
    // 直接呼叫兩個繪圖輔助函式
    draw_player_hp();
    // draw_skill_status();
}

void UI_game::draw_player_hp() {
    DataCenter *DC = DataCenter::get_instance();
    ImageCenter *IC = ImageCenter::get_instance();
    ALLEGRO_BITMAP *hp_bar = IC->get(hp_path);
    
    if(!DC->hero) return;

    int current_hp = DC->hero->HP; 
    int max_hp = DC->hero->max_HP; 

    // --- UI 參數設定 ---
    float x = 35;    // 距離左邊界 35 pixel
    float y = DC->window_height - 200;    // 距離下邊界 200 pixel
    float w = 40;    // 試管寬度
    float h = 160;   // 試管總高度

    // --- 計算液體高度 ---
    float ratio = (float)current_hp / max_hp;
    // 限制範圍在 0~1 之間，避免爆表
    if(ratio < 0) ratio = 0;
    if(ratio > 1) ratio = 1;
    
    float liquid_h = h * ratio; // 液體實際高度

    // --- 開始繪圖 (順序很重要：背景 -> 液體 -> 高光 -> 外框) ---

    // 座標計算：底部是 y+h，往上扣掉 liquid_h
    BulletState bullet_state = DC->hero->get_bullet_state();
    ALLEGRO_COLOR liquid_color;
    switch(bullet_state) {
        case BulletState::BALL:
            liquid_color = al_map_rgb(119, 255, 171); // 史萊姆綠
            break;
        case BulletState::SOLID:
            liquid_color = al_map_rgb(175, 220, 255); // 淡藍色
            break;
        case BulletState::LIQUID:
            liquid_color = al_map_rgb(108, 190, 255); // 深藍色
            break;
        case BulletState::GAS:
            liquid_color = al_map_rgb(205,237,255); // 淺藍色
            break;
        case BulletState::POSITIVE:
            liquid_color = al_map_rgb(255,142,123); // 淡粉色
            break;
        case BulletState::NEGATIVE:
            liquid_color = al_map_rgb(152, 160, 255); // 淡紫色
            break;
        case BulletState::LASER:
            liquid_color = al_map_rgb(255, 255, 0); // 黃色
            break;
        default:
            liquid_color = al_map_rgb(119, 255, 171); // 預設為史萊姆綠
            break;
    }
    al_draw_filled_rectangle(x, y + h - liquid_h, x + w, y + h, liquid_color); // 使用對應顏色

    // C. (細節) 畫液體表面的一條亮線，增加立體感
    if(ratio > 0) {
        al_draw_line(x+8, y + h - liquid_h*0.95, x+8, y + h - liquid_h*0.6, al_map_rgb(255, 255, 255), 2.5);
        al_draw_line(x+8, y + h - liquid_h*0.5, x+8, y + h - liquid_h*0.45, al_map_rgb(255, 255, 255), 3);
    }

    // D. 畫玻璃管外框 (灰白色)
    // 線寬設為 3，比較有像素風格的厚實感
    // al_draw_rectangle(x, y, x + w, y + h, al_map_rgb(200, 200, 200), 3);
    al_draw_bitmap(
        hp_bar,
        x - 80, // 微調位置讓外框對齊
        y - 20,
        0
    );
    
    // E. (選配) 在試管下方顯示 HP 數字
    FontCenter *FC = FontCenter::get_instance();
    al_draw_textf(FC->courier_new[FontSize::MEDIUM], al_map_rgb(255, 255, 255),
                x + w/2, y + h + 15, ALLEGRO_ALIGN_CENTRE, "%d", current_hp);
}