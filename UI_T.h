#ifndef UIT_H_INCLUDED
#define UIT_H_INCLUDED

#include <allegro5/bitmap.h>
#include <vector>
#include <tuple>
#include "./shapes/Point.h"

// fixed settings
namespace MenuSetting {
	enum class ButtonType {
		// RETURN, INTRO, SETTING, START, LEVEL1, LEVEL2, LEVEL3, LEVEL4, BUTTON_MAX
		INTRO, SETTING, LEVELS, BUTTON_MAX
	};
	const std::array<std::string, static_cast<int>(ButtonType::BUTTON_MAX)> button_img_path = {
		"./assets/image/button/intro.png",
		"./assets/image/button/setting.png",
		"./assets/image/button/levels.png"
	};
};
namespace IntroSetting {
	enum class ButtonType {
		// RETURN, INTRO, SETTING, START, LEVEL1, LEVEL2, LEVEL3, LEVEL4, BUTTON_MAX
		RETURN, BUTTON_MAX
	};
	const std::array<std::string, static_cast<int>(ButtonType::BUTTON_MAX)> button_img_path = {
		"./assets/image/button/menu.png"
	};
};
namespace SettingSetting {
	enum class ButtonType {
		// RETURN, INTRO, SETTING, START, LEVEL1, LEVEL2, LEVEL3, LEVEL4, BUTTON_MAX
		DIFFICULTY, SOUND, MUSIC, RETURN, BUTTON_MAX
	};
	const std::array<std::string, static_cast<int>(ButtonType::BUTTON_MAX)> button_img_path = {
		"./assets/image/button/setting.png",
		"./assets/image/button/setting.png",
		"./assets/image/button/setting.png",
		"./assets/image/button/menu.png"
	};
};
namespace LevelsSetting {
	enum class ButtonType {
		LEVEL1, LEVEL2, LEVEL3, LEVEL4, RETURN, BUTTON_MAX
	};
	const std::array<std::string, static_cast<int>(ButtonType::BUTTON_MAX)> button_img_path = {
		"./assets/image/button/level1.png",
		"./assets/image/button/level2.png",
		"./assets/image/button/level3.png",
		"./assets/image/button/level4.png",
		"./assets/image/button/menu.png"
	};
};
namespace PauseSetting {
	enum class ButtonType {
		RESUME, RESTART, MENU, BUTTON_MAX
	};
	const std::array<std::string, static_cast<int>(ButtonType::BUTTON_MAX)> button_img_path = {
		"./assets/image/button/resume.png",
		"./assets/image/button/restart.png",
		"./assets/image/button/menu.png"
	};
};

class UI {
public:
	UI() {}
	void virtual init() {}
	int update();
	void virtual draw();
protected:
	enum class STATE {
		HALT, // -> SELECT
		SELECT // -> HALT
	};
	STATE state;
	std::vector<std::pair<ALLEGRO_BITMAP*, Point>> button_items;
	int on_item;
	bool intro = false;
	bool easter = false;
	// fixed settings
	const int button_img_left_padding = -55;
	const int button_img_top_padding_main = 160;
	const int button_img_top_padding_set = 120;
	const int button_img_top_padding_level = 45;
	const int button_img_h_padding_set = 40;
	const int button_img_h_padding_main = 75;
	const int button_img_h_padding_level = 30;
private:
};

#endif
