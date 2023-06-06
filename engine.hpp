#pragma once
#include "common.hpp"
#include "kisslib/kissmath.hpp"
#include "input.hpp"

struct GLFWwindow;

struct Engine {
	SERIALIZE(Engine, input)

	Engine (const char* window_title);
	virtual ~Engine ();

	GLFWwindow* window;
	Input input = {};

	DirectoyChangeNotifier file_changes = DirectoyChangeNotifier("./", true);
	
	// close down game after current frame
	void close ();

	bool fullscreen = false; // readonly
	bool borderless_fullscreen = true; // readonly, use borderless fullscreen as long as the cpu usage (gpu driver?) bug happens on my dev desktop
	struct Rect {
		int2 pos, dim;
	};
	Rect window_pos;

	bool switch_fullscreen (bool fullscreen, bool borderless_fullscreen);
	bool toggle_fullscreen ();

	Timing_Histogram fps_display;

	bool vsync = true;
	void set_vsync (bool vsync);

	void draw_imgui ();

	bool imgui_enabled = true;
	bool imgui_show_demo_window = false;

	bool screenshot_hud = false;
	bool trigger_screenshot = false;


	virtual void frame () = 0;

	virtual void json_load () = 0;
	virtual void json_save () = 0;

	int main_loop ();
};
