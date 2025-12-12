#ifndef OPERATIONCENTER_H_INCLUDED
#define OPERATIONCENTER_H_INCLUDED

/**
 * @brief Class that defines functions for all object operations.
 * @details Object self-update, draw, and object-to-object interact functions are defined here.
 */
class OperationCenter
{
public:
	static OperationCenter *get_instance() {
		static OperationCenter OC;
		return &OC;
	}
	/**
	 * @brief Highest level update function.
	 * @details Calls all other update functions.
	 */
	void update();
	/**
	 * @brief Highest level draw function.
	 * @details Calls all other draw functions.
	 */
	void draw();
private:
	OperationCenter() {}
private:
	void _update_monster();
	void _update_tool();
	void _update_bullet();
	void _update_hero_bullet();
	void _update_monster_bullet();
	void _update_bullet_bullet();
	void _update_tool_bullet();
	void _update_tool_others();
	void _update_monster_hero();
	void _update_block();
	void _update_hero_block();
	void _update_bullet_block();
private:
	void _draw_monster();
	void _draw_bullet();
	void _draw_tool();
	void _draw_block();
};

#endif
