#include "common.hpp"

#include "kisslib/clean_windows_h.hpp"

#include "common_app.hpp"

#include "dear_imgui_custom/imgui_impl_glfw.h"
#include "dear_imgui_custom/imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"

inline int _vsync_on_interval = 1;
void Window::set_vsync (bool vsync) {
	this->vsync = vsync;
	glfwSwapInterval(vsync ? _vsync_on_interval : 0);
}

//// Imgui stuff
void imgui_setup () {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(g_window.window, true);
	ImGui_ImplOpenGL3_Init();
}
void imgui_shutdown () {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void imgui_begin_frame () {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame(g_window.imgui_enabled && g_window.input.cursor_enabled);

	auto& io = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	if (io.WantCaptureKeyboard)
		g_window.input.disable_keyboard();
	if (io.WantCaptureMouse)
		g_window.input.disable_mouse();

	ImGui::NewFrame();
}
void imgui_end_frame () {
	ImGui::Render();

	if (g_window.imgui_enabled)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//// GLFW & Opengl setup
bool renderer_setup (char const* window_title) {
	if (!glfwInit()) {
		printf("glfwInit error!");
		return false;
	}
	
	glfwWindowHint(GLFW_RESIZABLE, 1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1); // glLineWidth generated GL_INVALID_VALUE with GLFW_OPENGL_FORWARD_COMPAT

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#if RENDERER_DEBUG_OUTPUT
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	g_window.window = glfwCreateWindow(1024, 1024, window_title, NULL, NULL);
	if (!g_window.window) {
		printf("glfwCreateWindow error!");
		return false;
	}

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(g_window.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfw_register_input_callbacks(g_window);

	//
	glfwMakeContextCurrent(g_window.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("gladLoadGLLoader error!");
		return false;
	}

#if RENDERER_DEBUG_OUTPUT
	if (glfwExtensionSupported("GL_ARB_debug_output")) {
		glDebugMessageCallbackARB(ogl::debug_callback, &g_window);
	#if RENDERER_DEBUG_OUTPUT_BREAKPOINT
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB); // Call message on thread that call was made
	#endif
	}
#endif

	if (glfwExtensionSupported("WGL_EXT_swap_control_tear"))
		_vsync_on_interval = -1;

	g_window.set_vsync(g_window.vsync);

	// srgb enabled by default if supported
	// TODO: should I use glfwExtensionSupported or GLAD_GL_ARB_framebuffer_sRGB? does it make a difference?
	if (glfwExtensionSupported("GL_ARB_framebuffer_sRGB"))
		glEnable(GL_FRAMEBUFFER_SRGB);
	else
		fprintf(stderr, "[OpenGL] No sRGB framebuffers supported! Shading will be wrong!\n");

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // core since 3.2

	{
		auto* vend = glGetString(GL_VENDOR);
		auto* rend = glGetString(GL_RENDERER);
		auto* vers = glGetString(GL_VERSION);
		
		printf( "GL_VENDOR:   %s\n"
		        "GL_RENDERER: %s\n"
		        "GL_VERSION:  %s\n", vend, rend, vers);
	}

	TracyGpuContext;
	
	imgui_setup();
	
	return true;
}
void renderer_shutdown () {
	imgui_shutdown();
	g_shaders.shutdown();
	glfwTerminate();
}

void common_imgui (Input& I) {
	{
		bool fullscreen = g_window.fullscreen;
		bool borderless_fullscreen = g_window.borderless_fullscreen;

		bool changed = ImGui::Checkbox("fullscreen", &fullscreen);
		ImGui::SameLine();
		changed = ImGui::Checkbox("borderless", &borderless_fullscreen) || changed;

		if (changed)
			g_window.switch_fullscreen(fullscreen, borderless_fullscreen);
	}

	ImGui::SameLine();
	bool vsync = g_window.vsync;
	if (ImGui::Checkbox("Vsync", &vsync)) {
		g_window.set_vsync(vsync);
	}

	ImGui::SameLine();
	if (ImGui::Button("exit"))
		g_window.close();

	//ImGui::SameLine();
	//ImGui::Checkbox("Logger", &g_logger.shown);

	ImGui::SameLine();
	ImGui::Checkbox("ImGui Demo", &g_window.imgui_show_demo_window);

	if (g_window.imgui_show_demo_window)
		ImGui::ShowDemoWindow(&g_window.imgui_show_demo_window);


	ImGui::Spacing();

	if (imgui_Header("Performance", true)) {
		g_window.fps_display.push_timing(I.real_dt);
		g_window.fps_display.imgui_display("framerate", I.dt, true);

		//ImGui::Text("Chunks drawn %4d / %4d", world->chunks.chunks.count() - world->chunks.count_culled, world->chunks.chunks.count());
		ImGui::PopID();
	}

	g_window.input.imgui();
}

void update_files_changed (IApp* app, DirectoyChangeNotifier& file_changes) {
	ZoneScopedN("update_files_changed");
	
	// could poll this less frequently if it were to block the main thread significantly
	// but that would just cause stutter every N frames
	// if the OS were that bad I would be forced to run this in a background thread instead
	
	kiss::ChangedFiles changed_files;
	{
		ZoneScopedN("file_changes.poll_changes()");
		changed_files = file_changes.poll_changes();
	}
	
	if (changed_files.any()) {
		ZoneScopedN("reload");
		
		// Repeat reloading of assets because reacting to filechanges often fails because of half-written files
		int max_retries = 100;
		int retry_delay = 10; // ms

		for (int i=0; i<max_retries; ++i) {
			Sleep(retry_delay); // start out with a delay in hopes of getting a working file the first time
			
			bool success = g_shaders.update_recompilation(changed_files);
			
			/*
			if (changed_files.any_starts_with("textures/", FILE_ADDED|FILE_MODIFIED|FILE_RENAMED_NEW_NAME)) {
				clog(INFO, "[OpenglRenderer] Reload textures due to file change");
				try_reloading([&] () { return load_static_data(); });
			}*/
			
			if (success)
				break; // success
		}
	}
}

IApp* app;

DirectoyChangeNotifier* g_file_changes;

void window_frame () {
	FrameMark;

	glfw_sample_non_callback_input(g_window);

	update_files_changed(app, *g_file_changes);

	imgui_begin_frame();

	common_imgui(g_window.input);

	app->frame(g_window.input);

	imgui_end_frame();

	{
		ZoneScopedN("glfwSwapBuffers");
		glfwSwapBuffers(g_window.window);
	}

	g_window.input.clear_frame_input();
	g_window.input.get_time();
	g_window.input.frame_counter++;
}

// need to make sure window_frame only gets called if the (series of) glfw_window_size_event
// was the result of a glfwPollEvents() call (which blocks until resizing is done, forcing me to do this in the first place)
// glfw_window_size_event can also be emitted by switch_fullscreen for example
// which casues recursive window_frame() calls, which are not desired and also trigger imgui asserts
bool draw_on_size_events = false;

// enable drawing frames when resizing the window
void glfw_window_size_event (GLFWwindow* wnd, int width, int height) {
	if (!draw_on_size_events) return;
	
	auto& I = ((Window*)glfwGetWindowUserPointer(wnd))->input;

	window_frame();
}

int run_window (IApp* (*make_app)(), char const* window_title) {
	
	if (!renderer_setup(window_title))
		return 1;

	app = make_app();
	
	glfw_input_pre_gameloop(g_window);
	
	DirectoyChangeNotifier file_changes = DirectoyChangeNotifier("./", true);
	g_file_changes = &file_changes;
	
	while (!glfwWindowShouldClose(g_window.window)) {
		{
			ZoneScopedN("glfwPollEvents");
			draw_on_size_events = true;
			glfwPollEvents();
			draw_on_size_events = false;
		}

		if (glfwWindowShouldClose(g_window.window))
			break;
		
		window_frame();
	}
	
	delete app;
	renderer_shutdown();
	
	return 0;
}

//// fullscreen mode
struct Monitor {
	GLFWmonitor* monitor;
	GLFWvidmode const* vidmode;
	int2	 pos;
	int2	 size;
};

bool select_monitor_from_window_pos (Rect window_positioning, Monitor* selected_monior) {
	int count;
	auto** glfw_monitors = glfwGetMonitors(&count);

	std::vector<Monitor> monitors;
	monitors.resize(count);

	auto window_monitor_overlap = [=] (Monitor const& mon) {
		int2 a = clamp(window_positioning.pos, mon.pos, mon.pos + mon.size);
		int2 b = clamp(window_positioning.pos + window_positioning.dim, mon.pos, mon.pos + mon.size);

		int2 size = b - a;
		float overlap_area = (float)(size.x * size.y);
		return overlap_area;
	};

	float max_overlap = -INF;
	Monitor* max_overlap_monitor = nullptr;

	for (int i=0; i<count; ++i) {
		auto& m = monitors[i];

		m.monitor = glfw_monitors[i];
		m.vidmode = glfwGetVideoMode(m.monitor);
		glfwGetMonitorPos(m.monitor, &m.pos.x, &m.pos.y);

		m.size.x = m.vidmode->width;
		m.size.y = m.vidmode->height;

		float overlap = window_monitor_overlap(m);
		if (overlap > max_overlap) {
			max_overlap = overlap;
			max_overlap_monitor = &m;
		}
	}

	if (!max_overlap_monitor)
		return false; // fail, glfw returned no monitors

	*selected_monior = *max_overlap_monitor;
	return true;
}
bool Window::switch_fullscreen (bool fullscreen, bool borderless_fullscreen) {
	if (!this->fullscreen) {
		// store windowed window placement
		glfwGetWindowPos(window, &window_pos.pos.x, &window_pos.pos.y);
		glfwGetWindowSize(window, &window_pos.dim.x, &window_pos.dim.y);
	}

	if (fullscreen) {
		Monitor monitor;
		if (!select_monitor_from_window_pos(window_pos, &monitor))
			return false; // fail

		if (borderless_fullscreen) {
			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
			glfwSetWindowMonitor(window, NULL, monitor.pos.x, monitor.pos.y, monitor.size.x, monitor.size.y, GLFW_DONT_CARE);
		} else {
			glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
			glfwSetWindowMonitor(window, monitor.monitor, 0,0, monitor.vidmode->width, monitor.vidmode->height, monitor.vidmode->refreshRate);
		}
	} else {
		// restore windowed window placement
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowMonitor(window, NULL, window_pos.pos.x, window_pos.pos.y,
			window_pos.dim.x, window_pos.dim.y, GLFW_DONT_CARE);
	}

	// reset vsync to make sure 
	g_window.set_vsync(g_window.vsync);

	this->fullscreen = fullscreen;
	this->borderless_fullscreen = borderless_fullscreen;
	return true;
}
bool Window::toggle_fullscreen () {
	return switch_fullscreen(!fullscreen, borderless_fullscreen);
}

void Window::close () {
	glfwSetWindowShouldClose(window, 1);
}

//// Input stuff
void Input::set_cursor_mode (Window& window, bool enabled) {
	cursor_enabled = enabled;

	if (enabled)
		glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Cursor enabled, can interact with Imgui
	else
		glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Cursor disabled & Imgui interaction disabled, all controls go to game
}

////
void glfw_input_pre_gameloop (Window& window) {
	// Get initial mouse position
	glfwGetCursorPos(window.window, &window.input._prev_mouse_pos_x, &window.input._prev_mouse_pos_y);

	// Set initial cursor mode
	window.input.set_cursor_mode(window, window.input.cursor_enabled);
	window.input._prev_cursor_enabled = window.input.cursor_enabled;

	g_window.input.dt = 0; // dt zero on first frame
	g_window.input.frame_begin_ts = get_timestamp();
}
void glfw_sample_non_callback_input (Window& window) {
	ZoneScoped;

	glfwGetFramebufferSize(window.window, &window.input.window_size.x, &window.input.window_size.y);

	double x, y;
	glfwGetCursorPos(window.window, &x, &y);

	window.input.cursor_pos = float2((float)x, (float)y);

	window.input.cursor_pos_bottom_up = window.input.cursor_pos;
	window.input.cursor_pos_bottom_up.y = window.input.window_size.y - 1 - window.input.cursor_pos_bottom_up.y;

	//logf("cursor_pos: %f %f\n", input.cursor_pos.x, input.cursor_pos.y);
}

//// Callbacks
void glfw_key_event (GLFWwindow* wnd, int key, int scancode, int action, int mods) {
	assert(action == GLFW_PRESS || action == GLFW_RELEASE || action == GLFW_REPEAT);
	auto& window = *(Window*)glfwGetWindowUserPointer(wnd);
	auto& I = window.input;

	bool went_down =	action == GLFW_PRESS;
	bool went_up =		action == GLFW_RELEASE;

	bool alt =			(mods & GLFW_MOD_ALT) != 0;

	// Toggle fullscreen with F11 or CTRL-ENTER
	if (key == GLFW_KEY_F11 || (alt && key == GLFW_KEY_ENTER)) {
		if (went_down) window.toggle_fullscreen();
		return;
	}

	// Toggle Imgui visibility with F1
	if (key == GLFW_KEY_F1) {
		if (went_down) window.imgui_enabled = !window.imgui_enabled;
		return;
	}

	// Toggle between Imgui interaction and game control
	if (key == GLFW_KEY_F2) {
		if (went_down) I.toggle_cursor_mode(window);
		return;
	}

	// only process keys after GLFW_KEY_SPACE (32) to allow me to pack mouse buttons into the same array
	if ((went_down || went_up) && key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST) {
		I.buttons[key].is_down = went_down;
		I.buttons[key].went_down = went_down;
		I.buttons[key].went_up = went_up;
	}
}
void glfw_char_event (GLFWwindow* wnd, unsigned int codepoint, int mods) {
	auto& I = ((Window*)glfwGetWindowUserPointer(wnd))->input;

	// for typing input
}
void glfw_mouse_button_event (GLFWwindow* wnd, int button, int action, int mods) {
	assert(action == GLFW_PRESS || action == GLFW_RELEASE);
	auto& I = ((Window*)glfwGetWindowUserPointer(wnd))->input;

	bool went_down = action == GLFW_PRESS;
	bool went_up =	 action == GLFW_RELEASE;

	if ((went_down || went_up) && button >= GLFW_MOUSE_BUTTON_1 && button <= GLFW_MOUSE_BUTTON_8) {
		I.buttons[button].is_down = went_down;
		I.buttons[button].went_down = went_down;
		I.buttons[button].went_up = went_up;
	}
}

// The initial event seems to report the same position as our initial glfwGetCursorPos, so that delta is fine
// But when toggling the cursor from disabled to visible cursor jumps back to the prev position, and an event reports this as delta so we need to discard this 
void glfw_mouse_move_event (GLFWwindow* wnd, double xpos, double ypos) {
	auto& I = ((Window*)glfwGetWindowUserPointer(wnd))->input;

	float2 delta = float2((float)(xpos - I._prev_mouse_pos_x), (float)(ypos - I._prev_mouse_pos_y));
	delta.y = -delta.y; // convert to bottom up

	I._prev_mouse_pos_x = xpos;
	I._prev_mouse_pos_y = ypos;

	bool cursor_enabled = glfwGetInputMode(wnd, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;
	bool cursor_toggled = cursor_enabled != I._prev_cursor_enabled;
	I._prev_cursor_enabled = cursor_enabled;

	bool discard_delta = cursor_toggled;

	//logf("glfw_mouse_move_event: %7d: %f %f%s\n", frame_counter, delta.x, delta.y, discard_delta ? " (discarded)":"");

	if (!discard_delta)
		I.mouse_delta += delta;
}
void glfw_mouse_scroll (GLFWwindow* wnd, double xoffset, double yoffset) {
	auto& I = ((Window*)glfwGetWindowUserPointer(wnd))->input;

	// assume int, if glfw_mouse_scroll ever gives us 0.2 for ex. this might break
	// But the gameplay code wants to assume mousewheel moves in "clicks", for item swapping
	// I've personally never seen a mousewheel that does not move in "clicks" anyway
	I.mouse_wheel_delta += (int)ceil(abs(yoffset)) * (int)normalizesafe((float)yoffset); // -1.1f => -2    0 => 0    0.3f => +1
}

void glfw_register_input_callbacks (Window& window) {
	glfwSetWindowUserPointer(window.window, &window);

	glfwSetKeyCallback        (window.window, glfw_key_event);
	glfwSetCharModsCallback   (window.window, glfw_char_event);
	glfwSetCursorPosCallback  (window.window, glfw_mouse_move_event);
	glfwSetMouseButtonCallback(window.window, glfw_mouse_button_event);
	glfwSetScrollCallback     (window.window, glfw_mouse_scroll);
	glfwSetWindowSizeCallback (window.window, glfw_window_size_event);
}
