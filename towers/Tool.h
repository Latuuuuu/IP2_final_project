#ifndef TOOL_H_INCLUDED
#define TOOL_H_INCLUDED

#include "../Object.h"
#include "../shapes/Line.h"
#include <allegro5/bitmap.h>
#include <string>

namespace ToolSetting {
	static constexpr char root_path[40] = "./assets/image/tool";
	static constexpr char dir_path_postfix[][10] = {
		"convex","concave", "mirror"
	};
	static constexpr char tool_icon_path[][20] = {
		"convex_icon","concave_icon", "mirror_icon"
	};
}

enum class ToolType{
	CONVEX,
	CONCAVE,
	MIRROR,
	WAVETYPE_MAX
};

class Tool : public Object
{
public:
	Tool(const Point &p, const double z, ToolType type);
	void update();
	void draw();
	const ToolType get_type() { return type; }
	const double get_angle() { return angle; }
private:
	/**
	 * @brief rotation angle of the tool
	 */
	double angle;
	/**
	 * @brief ALLEGRO_BITMAP of the tool.
	 */
	ALLEGRO_BITMAP *bitmap;
	ToolType type;
};

#endif
