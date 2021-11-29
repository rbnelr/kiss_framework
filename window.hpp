#pragma once
#include "kisslib/kissmath.hpp"
#include "input.hpp"

struct Rect {
	int2	 pos;
	int2	 dim;
};

struct GLFWwindow;

struct Window {
	GLFWwindow* window;
	Input input;

	// close down game after current frame
	void close ();
	
	bool fullscreen = false; // readonly
	bool borderless_fullscreen = true; // readonly, use borderless fullscreen as long as the cpu usage (gpu driver?) bug happens on my dev desktop
	Rect window_pos;

	bool switch_fullscreen (bool fullscreen, bool borderless_fullscreen);
	bool toggle_fullscreen ();

	Timing_Histogram fps_display;
	
	bool vsync = true;
	void set_vsync (bool vsync);

	bool imgui_enabled = true;
	bool imgui_show_demo_window = false;
};

inline Window g_window;
