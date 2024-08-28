#pragma once
#include "common.hpp"
#include "kisslib/kissmath.hpp"
#include "input.hpp"

#if OGL_USE_REVERSE_DEPTH
namespace ogl {
	extern bool reverse_depth;
}
#endif

struct View3D {
	// forward VP matrices
	float4x4	world2clip;
	float4x4	world2cam;
	float4x4	cam2clip;

	// inverse VP matrices
	float4x4	clip2world;
	float4x4	cam2world;
	float4x4	clip2cam;

	// more details for simpler calculations
	float2		frust_near_size; // width and height of near plane (for casting rays for example)
	// near & far planes
	float		clip_near;
	float		clip_far;
	// just the camera center in world space
	float3		cam_pos;
	// viewport width over height, eg. 16/9
	float		aspect_ratio;

	// viewport size (pixels)
	float2		viewport_size;
	float2		inv_viewport_size;

	bool cursor_ray (Input& I, float3* out_ray_pos=nullptr, float3* out_ray_dir=nullptr) const {
		if (!I.cursor_enabled) return false;

		float2 px_center = I.cursor_pos_bottom_up() + 0.5f;

		float2 uv = px_center * inv_viewport_size - 0.5f;

		// TODO: fix this for ortho

		float3 ray_dir = (float3x3)cam2world * float3(frust_near_size * uv, -clip_near);
		float3 ray_pos = cam_pos + ray_dir;

		if (out_ray_pos) *out_ray_pos = ray_pos;
		if (out_ray_dir) *out_ray_dir = normalize(ray_dir);

		return true;
	}
};

inline View3D ortho_view (float w, float h, float near, float far,
		float3x4 const& world2cam, float3x4 const& cam2world, float2 viewport_size) {
	
	float a, b;
#if OGL_USE_REVERSE_DEPTH
	if (ogl::reverse_depth) {
		// maps cam_z linearily [-near, -far] -> [1,0]
		// NOTE: can't really do reverse_depth (infinine far plane) with ortho projection since we can't do divide by z
		// and can't map infinity into 0,1 range without it
		a = 1.0f / (far - near);
		b = near * a + 1.0f;
	}
	else
#endif
	{
		// maps cam_z linearily [-near, -far] -> [-1,+1]
		a = -2.0f / (far - near);
		b = near * a - 1.0f;
	}
	
	View3D v;

	v.cam2clip = float4x4(
		2.0f/w,      0,      0,     0,
		     0, 2.0f/h,      0,     0,
		     0,      0,      a,     b,
		     0,      0,      0,     1);

	v.clip2cam = float4x4(
		w*0.5f,      0,      0,     0,
		     0, h*0.5f,      0,     0,
		     0,      0, 1.0f/a,  -b/a,
		     0,      0,      0,     1);
	
	// V matrices
	v.world2cam = (float4x4)world2cam;
	v.cam2world = (float4x4)cam2world;
	// VP inverses
	v.world2clip = v.cam2clip * v.world2cam;
	v.clip2world = v.cam2world * v.clip2cam;

	// misc
	v.frust_near_size    = float2(w,h);
	v.clip_near          = near;
	v.clip_far           = far;
	v.cam_pos            = v.cam2world.arr[3];
	v.aspect_ratio       = w / h;
	v.viewport_size      = viewport_size;
	v.inv_viewport_size  = 1.0f / viewport_size;

	return v;
}

inline View3D persp_view (float vfov, float aspect, float near, float far,
		float3x4 const& world2cam, float3x4 const& cam2world, float2 viewport_size) {
	float y = tanf(vfov * 0.5f);
	float x = y * aspect;

	float2 frust_size = float2(x,y); // half-widths of frustrum at 1 unit depth

	//float hfov = atanf(frust_scale.x) * 2.0f;

	float a, b;
#if OGL_USE_REVERSE_DEPTH
	if (ogl::reverse_depth) {
		// maps cam_z [-near,inf) -> depth [1,0]
		
		// use_reverse_depth with use infinite far plane
		// visible range on z axis in opengl and vulkan goes from -near to -inf
		// depth formula is: (with input w=1) z' = near, w' = -z  ->  depth = near/-z
		// depth values go from 1 at the near plane to 0 at infinity
		// inverse formula ist -z = near / depth
		far = 1000000.0f; // can't actually set far to be infinite if I want frustrum culling to work without modification
		a = 0.0f;
		b = near;
	}
	else
#endif
	{
		// maps cam_z [-near,-far] -> depth [-1,+1]
		a = (far + near) / (near - far);
		b = (2.0f * far * near) / (near - far);
	}

	View3D v;

	v.cam2clip = float4x4(
		1.0f/x,      0,      0,     0,
		     0, 1.0f/y,      0,     0,
		     0,      0,      a,     b,
		     0,      0,     -1,     0);
	v.clip2cam = float4x4(
		     x,      0,      0,     0,
		     0,      y,      0,     0,
		     0,      0,      0,    -1,
		     0,      0, 1.0f/b,   a/b);
	
	// V matrices
	v.world2cam = (float4x4)world2cam;
	v.cam2world = (float4x4)cam2world;
	// VP inverses
	v.world2clip = v.cam2clip * v.world2cam;
	v.clip2world = v.cam2world * v.clip2cam;

	// misc
	v.frust_near_size    = frust_size * 2.0f * near;
	v.clip_near          = near;
	v.clip_far           = far;
	v.cam_pos            = v.cam2world.arr[3];
	v.aspect_ratio       = aspect;
	v.viewport_size      = viewport_size;
	v.inv_viewport_size  = 1.0f / viewport_size;

	return v;
}

inline void azimuthal_mount (float3 const& aer, float3x3* out_world2cam, float3x3* out_cam2world) {
	if (out_world2cam) *out_world2cam = rotate3_Z(+aer.z) * rotate3_X(-aer.y - deg(90)) * rotate3_Z(-aer.x);
	if (out_cam2world) *out_cam2world = rotate3_Z(+aer.x) * rotate3_X(+aer.y + deg(90)) * rotate3_Z(-aer.z);
}

inline void rotate_with_mouselook (Input& I, float vfov, float3* aer,
		float azim_min=deg(-180), float azim_max=deg(180), float elev_min=deg(-85), float elev_max=deg(85)) {
	float& azimuth   = aer->x;
	float& elevation = aer->y;
	float& roll      = aer->z;

	// Mouselook
	auto raw_mouselook = I.get_fps_mouselook_delta();

	float2 sens = I.mouselook_sens / 1000;
	sens.x *= I.mouselook_sens_x_scale;
	// sens scales with fov since muscle memory works with visual distances on screen, not with in game angles
	// (mostly? 180deg flip might also be muscle memory?)
	float2 delta = raw_mouselook * vfov * sens;

	azimuth   -= delta.x;
	elevation += delta.y;

	azimuth = wrap(azimuth, azim_min, azim_max);
	elevation = clamp(elevation, elev_min, elev_max);

	// roll with keys
	float roll_dir = 0;
	//if (I.buttons[KEY_Q].is_down) roll_dir += 1;
	//if (I.buttons[KEY_E].is_down) roll_dir -= 1;

	float roll_speed = deg(90);

	roll += roll_dir * roll_speed * I.real_dt;
	roll = wrap(roll, deg(-180), deg(180));
}

inline float smooth_var (float dt, float cur, float target, float smooth_fac, float smooth_const=1) {
	if (smooth_fac <= 0.0f)
		return target;

	// smoothed zoom via animating towards zoom_target
	float delta = target - cur;
	float dir = copysignf(1.0f, delta);
	delta = abs(delta);
	float vel = delta * smooth_fac + 1.0f; // proportional velocity + small constant to actually reach target

	cur += dir * min(delta, vel * dt); // min(delta, vel) to never overshoot target
	return cur;
} 

// 2D Orthographic camera
struct Camera2D {
	// TODO: serialize binds
	//SERIALIZE(Camera2D, pos, zoom, rot, move_speed, zoom_smooth_fac, zoom_speed, stretch, clip_near, clip_far)
	
	// Need to manually serialize to avoid animated zoom in at load
	friend SERIALIZE_TO_JSON(Camera2D) {
		SERIALIZE_TO_JSON_EXPAND(pos, zoom, rot, move_speed, zoom_smooth_fac, zoom_speed, stretch, clip_near, clip_far)
	}
	friend SERIALIZE_FROM_JSON(Camera2D) {
		SERIALIZE_FROM_JSON_EXPAND(pos, zoom, rot, move_speed, zoom_smooth_fac, zoom_speed, stretch, clip_near, clip_far)

		// HERE: need this to avoid animated zoom in at load
		t.zoom_target = t.zoom;
		t.stretch_target = t.stretch;
	}

	float3 pos  = 0; // center of visible region in world space (note z component can be left at 0 usually)
	float  zoom = 10; // log2 of height of visible region in world space
	float  rot  = 0; // rotation in radians

	float  move_speed = 1;

	float  zoom_target;
	float  zoom_smooth_fac = 16; // 0 to disable
	float  zoom_speed = 0.25f;

	float  stretch = 0.0f;
	float  stretch_target;
	
	float  clip_near  = -10;
	float  clip_far   = +10;

	float2 _drag_pos = 0;

	struct Binds {
		Button drag        = MOUSE_BUTTON_RIGHT;
		
		Button move_left   = KEY_A;
		Button move_right  = KEY_D;
		Button move_down   = KEY_S;
		Button move_up     = KEY_W;

		//Button rot_left    = KEY_Q;
		//Button rot_right   = KEY_E;
		Button rot_left    = KEY_NULL;
		Button rot_right   = KEY_NULL;

		Button zoom_in     = KEY_KP_ADD;
		Button zoom_out    = KEY_KP_SUBTRACT;

		//Button stretch_X   = KEY_LEFT_ALT;
		//Button stretch_Y   = KEY_LEFT_CONTROL;
		Button stretch_X   = KEY_NULL;
		Button stretch_Y   = KEY_NULL;
	};

	Binds binds;

	Camera2D (float3 pos=0, float vsize=20.0f, float rot=0, float clip_near=-10, float clip_far=+10):
			pos{pos}, zoom{-log2f(vsize)}, rot{rot}, clip_near{clip_near}, clip_far{clip_far} {
		zoom_target = zoom;
		stretch_target = stretch;
	}

	void imgui (const char* label="Camera2D") {
		if (!ImGui::TreeNode(label)) return;

		ImGui::DragFloat3("pos", &pos.x, 0.05f);
		if (ImGui::DragFloat("zoom", &zoom, 0.018f))
			zoom_target = zoom;

		float rot_deg = to_degrees(rot);
		if (ImGui::DragFloat("rot", &rot_deg, 0.3f))
			rot = to_radians(rot_deg);

		if (ImGui::TreeNodeEx("details")) {

			ImGui::DragFloat("zoom_smooth_fac", &zoom_smooth_fac, 0.05f);
			if (ImGui::DragFloat("stretch", &stretch, 0.05f))
				stretch_target = stretch;

			ImGui::DragFloat("clip_near", &clip_near, 0.05f);
			ImGui::DragFloat("clip_far" , &clip_far,  0.05f);

			ImGui::DragFloat("zoom_speed", &zoom_speed, 0.05f);
			ImGui::DragFloat("move_speed", &move_speed, 0.05f);

			ImGui::TreePop();
		}
		
		ImGui::TreePop();
	}

	void zoom_to (float view_height) {
		zoom_target = -log2f(view_height);
	}
	void zoom_to_noanim (float view_height) {
		zoom_to(view_height);
		zoom = zoom_target;
	}

	View3D update (Input& I, float2 const& viewport_size) {
		// key rotation
		float rot_dir = 0;
		if (binds.rot_left  >= 0 && I.buttons[binds.rot_left ].is_down) rot_dir -= 1;
		if (binds.rot_right >= 0 && I.buttons[binds.rot_right].is_down) rot_dir += 1;

		rot += rot_dir * deg(90.0f) * I.real_dt;

		// key movment
		float2 move_dir = 0;
		if (binds.move_left  >= 0 && I.buttons[binds.move_left ].is_down) move_dir.x -= 1;
		if (binds.move_right >= 0 && I.buttons[binds.move_right].is_down) move_dir.x += 1;
		if (binds.move_down  >= 0 && I.buttons[binds.move_down ].is_down) move_dir.y -= 1;
		if (binds.move_up    >= 0 && I.buttons[binds.move_up   ].is_down) move_dir.y += 1;
		move_dir = normalizesafe(move_dir);

		// key zoom
		float zoom_dir = 0;
		if (binds.zoom_in  >= 0 && I.buttons[binds.zoom_in ].is_down) zoom_dir += 1;
		if (binds.zoom_out >= 0 && I.buttons[binds.zoom_out].is_down) zoom_dir -= 1;

		float zoom_delta = zoom_dir * zoom_speed*8.0f * I.real_dt;

		// mousewheel zoom
		zoom_delta += (float)I.mouse_wheel_delta * zoom_speed;

		if (I.buttons[binds.stretch_X].is_down) {      // zoom on X axis (keep Y size same)
			stretch_target += zoom_delta;
		}
		else if (I.buttons[binds.stretch_Y].is_down) { // zoom on Y axis (keep X size same)
			// doing this makes zoom and stretch cancel each other out for the X scale
			zoom_target    += zoom_delta;
			stretch_target -= zoom_delta;
		}
		else { // normal zoom (both axes)
			zoom_target += zoom_delta;
		}

		zoom    = smooth_var(I.real_dt, zoom,    zoom_target,    zoom_smooth_fac, 1);
		stretch = smooth_var(I.real_dt, stretch, stretch_target, zoom_smooth_fac, 1);

		float2 size;
		size.y = powf(2.0f, -zoom);
		size.x = powf(2.0f, -zoom - stretch) * viewport_size.x / viewport_size.y;

		float aspect = size.x / size.y;

		float2x2 mat_rot = rotate2(rot);

		float2 delta;
		if (binds.drag >= 0 && I.buttons[binds.drag].is_down) {
			// mouse drag move
			float2 cursor_px = I.cursor_pos_bottom_up() + 0.5f; // ~[0, window_size]
			float2 cursor = cursor_px / (float2)I.window_size - 0.5f; // [-0.5, +0.5]
			float2 cursor_world = (mat_rot * (size * cursor)) + (float2)pos; // cursor in world space with current cam pos

			if (I.buttons[binds.drag].went_down) {
				_drag_pos = cursor_world; // remember cursor position
			}

			delta = _drag_pos - cursor_world;
		} else {
			delta = mat_rot * size.y * (move_dir * move_speed * I.real_dt);
		}
		pos.x += delta.x;
		pos.y += delta.y;
		
		float3x4 world2cam = float3x4(rotate3_Z(-rot)) * translate(-pos);
		float3x4 cam2world = translate(pos) * float3x4(rotate3_Z(rot));

		return ortho_view(size.x, size.y, clip_near, clip_far, world2cam, cam2world, viewport_size);
	}
};

struct Flycam {
	SERIALIZE(Flycam, pos, rot_aer, vfov, clip_near, clip_far, base_speed, max_speed, speedup_factor, fast_multiplier, planar)

	float3 pos = 0;
	float3 rot_aer = 0;

	float vfov = deg(70);
	float ortho_size = 50;

	float clip_near = 1.0f/32;
	float clip_far  = 8192;

	float base_speed = 0.5f;
	float max_speed = 1000000.0f;
	float speedup_factor = 2;
	float fast_multiplier = 4;

	float cur_speed = 0;

	bool ortho = false;

	bool planar = true;

	Flycam (float3 pos=0, float3 rot_aer=0, float base_speed=0.5f): pos{pos}, rot_aer{rot_aer}, base_speed{base_speed} {}

	void imgui (const char* label="Flycam") {
		if (!ImGui::TreeNode(label)) return;

		ImGui::DragFloat3("pos", &pos.x, 0.05f);

		float3 rot_deg = to_degrees(rot_aer);
		if (ImGui::DragFloat3("rot_aer", &rot_deg.x, 0.3f))
			rot_aer = to_radians(rot_deg);
		
		ImGui::Checkbox("ortho", &ortho);

		if (!ortho) ImGui::SliderAngle("vfov", &vfov, 0,180);
		else        ImGui::DragFloat("ortho_size", &ortho_size, 0.1f, 0, 8000, "%.3f", ImGuiSliderFlags_Logarithmic);

		ImGui::Text("cur_speed: %.3f", cur_speed);

		if (ImGui::TreeNodeEx("details")) {

			ImGui::DragFloat("clip_near", &clip_near, 0.05f);
			ImGui::DragFloat("clip_far" , &clip_far,  0.05f);

			ImGui::DragFloat("base_speed", &base_speed, 0.05f, 0, FLT_MAX, "%.3f", ImGuiSliderFlags_Logarithmic);
			ImGui::DragFloat("max_speed", &max_speed,   0.05f, 0, FLT_MAX, "%.3f", ImGuiSliderFlags_Logarithmic);
			ImGui::DragFloat("speedup_factor", &speedup_factor, 0.001f);
			ImGui::DragFloat("fast_multiplier", &fast_multiplier, 0.05f);

			ImGui::Checkbox("planar", &planar);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	View3D update (Input& I, float2 const& viewport_size) {

		//// look
		rotate_with_mouselook(I, vfov, &rot_aer);

		float3x3 cam2world_rot, world2cam_rot;
		azimuthal_mount(rot_aer, &world2cam_rot, &cam2world_rot);
		
		{ //// movement
			float3 move_dir = 0;
			if (I.buttons[KEY_A]           .is_down) move_dir.x -= 1;
			if (I.buttons[KEY_D]           .is_down) move_dir.x += 1;
			if (I.buttons[KEY_W]           .is_down) move_dir.z -= 1;
			if (I.buttons[KEY_S]           .is_down) move_dir.z += 1;
			if (I.buttons[KEY_LEFT_CONTROL].is_down) move_dir.y -= 1;
			if (I.buttons[KEY_SPACE]       .is_down) move_dir.y += 1;

			move_dir = normalizesafe(move_dir);
			float move_speed = length(move_dir); // could be analog with gamepad

			if (move_speed == 0.0f)
				cur_speed = base_speed; // no movement resets speed

			if (I.buttons[KEY_LEFT_SHIFT].is_down) {
				move_speed *= fast_multiplier;

				cur_speed += base_speed * speedup_factor * I.unscaled_dt;
			}

			cur_speed = clamp(cur_speed, base_speed, max_speed);

			float3 delta_cam = cur_speed * move_dir * I.unscaled_dt;

			if (planar) {
				float2 delta2 = rotate2(rot_aer.x) * float2(delta_cam.x, -delta_cam.z);
				pos.x += delta2.x;
				pos.y += delta2.y;
				pos.z += delta_cam.y;
			}
			else {
				pos += cam2world_rot * delta_cam;
			}
		}

		{ //// speed or fov change with mousewheel
			if (!I.buttons[KEY_F].is_down) {
				float delta_log = 0.1f * I.mouse_wheel_delta;
				base_speed = powf(2.0f, log2f(base_speed) +delta_log );
			} else {
				float delta_log = -0.1f * I.mouse_wheel_delta;
				vfov = clamp(powf(2.0f, log2f(vfov) + delta_log), deg(1.0f/10), deg(170));
			}
		}

		return clac_view(viewport_size);
	}

	View3D clac_view (float2 const& viewport_size) {

		float aspect = viewport_size.x / viewport_size.y;

		float3x3 cam2world_rot, world2cam_rot;
		azimuthal_mount(rot_aer, &world2cam_rot, &cam2world_rot);

		float3x4 world2cam = float3x4(world2cam_rot) * translate(-pos);
		float3x4 cam2world = translate(pos) * float3x4(cam2world_rot);
		
		if (!ortho) {
			return persp_view(vfov, aspect, clip_near, clip_far, world2cam, cam2world, viewport_size);
		}
		else {
			return ortho_view(aspect * ortho_size, ortho_size, clip_near, clip_far, world2cam, cam2world, viewport_size);
		}
	}
};
