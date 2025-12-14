#include "Tool.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Camera.h"
#include "../Hero.h"
#include <allegro5/bitmap_draw.h>
#include <iostream>

Tool::Tool(const Point &p, const double z, ToolType type) {
	std::string path = std::string(ToolSetting::root_path) + "/" + ToolSetting::dir_path_postfix[static_cast<int>(type)] + ".png";
	bitmap = ImageCenter::get_instance()->get(path);
	double len = al_get_bitmap_height(bitmap) * 1.1;
	Line *line = new Line(p.x, p.y+len/2, p.x, p.y-len/2);
	focal_lenght = 20.0;
	focus0.x = cos(z)*focal_lenght+p.x;
	focus0.y = sin(z)*focal_lenght+p.y;
	focus1.x = cos(z)*(-focal_lenght)+p.x;
	focus1.y = sin(z)*(-focal_lenght)+p.y;
	// std::cout << "focus: " << focus0.x << " " << focus0.y << " " << focus1.x << " " << focus1.y << std::endl;
	line->update_center_z(z, p);
	shape.reset(line);
	this->type = type;
	this->angle = z;
}
/**
 * @brief
 * @details 
 */
void Tool::update() {

}

void Tool::draw() {
	DataCenter *DC = DataCenter::get_instance();
	// ImageCenter *IC = ImageCenter::get_instance();
	Point offset = DC->camera->transform_object(*shape);
	al_draw_rotated_bitmap(bitmap,
						al_get_bitmap_width(bitmap) / 2,
						al_get_bitmap_height(bitmap) / 2,
						offset.center_x(), 
						offset.center_y(), this->angle, 0);
}
