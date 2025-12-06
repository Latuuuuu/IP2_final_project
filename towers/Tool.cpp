#include "Tool.h"
#include "../data/DataCenter.h"
#include "../data/ImageCenter.h"
#include "../Camera.h"
#include "../Hero.h"
#include <allegro5/bitmap_draw.h>

Tool::Tool(const Point &p, const double z, ToolType type) {
	std::string path = std::string(ToolSetting::root_path) + "/" + ToolSetting::dir_path_postfix[static_cast<int>(this->type)] + ".png";
	bitmap = ImageCenter::get_instance()->get(path);
	int len = al_get_bitmap_height(bitmap);
	Line line(p.x, p.y+len/2, p.x, p.y-len/2);
	line.update_center_z(z, p);
	shape.reset(&line);
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
						offset.center_x(),
						offset.center_y(),
						offset.center_x() - al_get_bitmap_width(bitmap) / 2, 
						offset.center_y() - al_get_bitmap_height(bitmap) / 2, this->angle, 0);
}
