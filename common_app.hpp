#pragma once
#include "common.hpp"
#include "opengl.hpp"
#include "opengl_text.hpp"
#include "camera.hpp"
using namespace ogl;

struct IApp {
	virtual ~IApp () {}

	virtual void frame (Input& I) = 0;
};
int run_window (IApp* (*make_app)(), char const* window_title);

//// Reusable Opengl renderer
namespace ogl {
	struct Renderer {
		StateManager state;
		CommonUniforms common_ubo;

		Vao dummy_vao = {"dummy_vao"};

		struct glDebugDraw {
			bool wireframe          = false;
			bool wireframe_no_cull  = false;
			bool wireframe_no_blend = true;

			float line_width = 2;

			void imgui () {
				if (!imgui_Header("Debug Draw")) return;

				ImGui::Checkbox("wireframe", &wireframe);
				ImGui::SameLine();
				ImGui::Checkbox("no cull", &wireframe_no_cull);
				ImGui::SameLine();
				ImGui::Checkbox("no blend", &wireframe_no_blend);

				ImGui::SliderFloat("line_width", &line_width, 1.0f, 8.0f);

				ImGui::PopID();
			}
		} debug_draw;

		void begin (View3D const& view, int2 const& viewport_size) {
			{
				//OGL_TRACE("set state defaults");

				state.wireframe          = debug_draw.wireframe;
				state.wireframe_no_cull  = debug_draw.wireframe_no_cull;
				state.wireframe_no_blend = debug_draw.wireframe_no_blend;

				state.set_default();

				glEnable(GL_LINE_SMOOTH);
				glLineWidth(debug_draw.line_width);
			}

			glViewport(0,0, viewport_size.x, viewport_size.y);
			common_ubo.set(view);
		}
	};
}
