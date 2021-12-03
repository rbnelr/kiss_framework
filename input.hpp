#pragma once
#include "kisslib/kissmath.hpp"
#include "input_buttons.hpp"
#include "dear_imgui.hpp"

struct Window;

struct ButtonState {
	bool is_down   : 1; // button is down
	bool went_down : 1; // button was pressed this frame
	bool went_up   : 1; // button was released this frame
};

struct Input {
	uint64_t frame_counter = 0;
	
	//// Time 
	uint64_t frame_begin_ts;

	// zero on first frame
	// else duration of the prev frame scaled by time_scale but never larger than max_dt
	float dt;

	// zero on first frame
	// else duration of the prev frame (not scaled by time_scale) but never larger than max_dt
	// use for camera flying, potentially for player physics if slow motion is not supposed to affect player for ex.
	float unscaled_dt;

	float real_dt; // for displaying fps etc.

	// Max dt to prevent large physics time steps
	float max_dt = 1.0f / 20;
	// Timer scaler (does not scale unscaled_dt)
	float time_scale = 1;
	// Pause time (equivalent to time_scale=0)
	bool pause_time = false;

	//// Others
	bool cursor_enabled = true;

	int2 window_size;
	float2 cursor_pos; // in pixels, float because glfw retuns doubles
	float2 cursor_pos_bottom_up; // in pixels, float because glfw retuns doubles

	float2 mouse_delta;
	int mouse_wheel_delta; // in "clicks"

	ButtonState buttons[BUTTONS_COUNT];

	// sens in  <screen heights / 1000 dots>  (dots from mouse dpi)
	float mouselook_sens = 1.0f;
	float mouselook_sens_x_scale = 1;

	//float view_elevation_down_limit = deg(5);
	//float view_elevation_up_limit = deg(5);

	// to fix glfw mouse input
	double _prev_mouse_pos_x, _prev_mouse_pos_y;
	bool _prev_cursor_enabled;
	

	void imgui () {
		if (buttons[KEY_COMMA].went_down)
			pause_time = !pause_time;

		if (!imgui_Header("Input")) return;

		ImGui::DragFloat("max_dt", &max_dt, 0.01f);
		ImGui::DragFloat("time_scale", &time_scale, 0.01f);
		ImGui::Checkbox("pause_time [,]", &pause_time);

		ImGui::Text("timestep (dt): %6.3f ms", dt * 1000);

		ImGui::DragFloat("mouselook_sens", &mouselook_sens, 0.01f);
		ImGui::DragFloat("mouselook_sens_x_scale", &mouselook_sens_x_scale, 0.05f);
		
		//ImGui::SliderAngle("view_elevation_down_limit", &view_elevation_down_limit, 0, 45);
		//ImGui::SliderAngle("view_elevation_up_limit", &view_elevation_up_limit, 0, 45);

		ImGui::PopID();
	}

	// used by gameloop to clear frame based input like deltas and flags
	void clear_frame_input () {
		mouse_delta = 0;
		mouse_wheel_delta = 0;

		for (auto& b : buttons) {
			b.went_down = 0;
			b.went_up = 0;
		}
	}

	void get_time () {
		// Calc next frame dt based on this frame duration
		uint64_t now = get_timestamp();

		real_dt = (float)(now - frame_begin_ts) / (float)timestamp_freq;
		
		dt = min(real_dt * (pause_time ? 0 : time_scale), max_dt);

		unscaled_dt = min(real_dt, max_dt);

		frame_begin_ts = now;
	}

	// used to ignore inputs that imgui has already captured
	void disable_keyboard () {
		for (int i=MOUSE_BUTTONS_COUNT+1; i<BUTTONS_COUNT; ++i)
			buttons[i] = {};
	}
	void disable_mouse () {
		mouse_delta = 0;
		mouse_wheel_delta = 0;
		for (int i=MOUSE_BUTTON_1; i<BUTTONS_COUNT; ++i)
			buttons[i] = {};
	}

	// Not sure where to put this, return mouselook_delta
	//  if cursor is disabled (fps mode)
	//  if cursor is enabled and rmb down
	float2 get_mouselook_delta () {
		float2 delta = 0;
		if (!cursor_enabled || buttons[MOUSE_BUTTON_RIGHT].is_down) {
			delta = mouse_delta;
		}
		return delta;
	}

	void set_cursor_mode (Window& window, bool enabled);
	void toggle_cursor_mode (Window& window) {
		set_cursor_mode(window, !cursor_enabled);
	}
};

////
void glfw_input_pre_gameloop (Window& window);
void glfw_sample_non_callback_input (Window& window);

void glfw_register_input_callbacks (Window& window);
