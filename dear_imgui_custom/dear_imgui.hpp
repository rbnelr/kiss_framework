#pragma once
#include "kisslib/kissmath.hpp"
#include "kisslib/running_average.hpp"

#include "dear_imgui/imgui.h"
#include "dear_imgui/imgui_internal.h"
#include "dear_imgui/misc/cpp/imgui_stdlib.h"

#include "kisslib/stl_extensions.hpp"
#include <vector>

inline bool imgui_ColorEdit (const char* label, lrgb* col, ImGuiColorEditFlags flags=0) {
	float3 srgbf = float3( to_srgb(col->x), to_srgb(col->y), to_srgb(col->z) );
	bool ret = ImGui::ColorEdit3(label, &srgbf.x, flags);
	*col = float3( to_linear(srgbf.x), to_linear(srgbf.y), to_linear(srgbf.z) );
	return ret;
}
inline bool imgui_ColorEdit (const char* label, lrgba* col, ImGuiColorEditFlags flags=0) {
	float4 srgbaf = float4( to_srgb(col->x), to_srgb(col->y), to_srgb(col->z), col->w ); // alpha is linear
	bool ret = ImGui::ColorEdit4(label, &srgbaf.x, flags);
	*col = float4( to_linear(srgbaf.x), to_linear(srgbaf.y), to_linear(srgbaf.z), srgbaf.w );
	return ret;
}

inline bool imgui_ColorEdit (const char* label, srgb8* col, ImGuiColorEditFlags flags=0) {
	float3 srgbf = (float3)(*col) / 255.0f;
	bool ret = ImGui::ColorEdit3(label, &srgbf.x, flags);
	*col = (srgb8)roundi(srgbf * 255.0f);
	return ret;
}
inline bool imgui_ColorEdit (const char* label, srgba8* col, ImGuiColorEditFlags flags=0) {
	float4 srgbaf = (float4)(*col) / 255.0f;
	bool ret = ImGui::ColorEdit4(label, &srgbaf.x, flags);
	*col = (srgba8)roundi(srgbaf * 255.0f);
	return ret;
}

// needs a PopID()
inline bool imgui_Header (const char* label, bool default_open=false) {
	auto flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Framed |
	             ImGuiTreeNodeFlags_NoAutoOpenOnLog | (default_open ? ImGuiTreeNodeFlags_DefaultOpen : 0);
	if (!ImGui::TreeNodeEx(label, flags)) return false;
	ImGui::PushID(label);
	return true;
}

enum class GuiConfirm {
	PENDING = 0,
	NO,
	YES,
};
inline GuiConfirm imgui_delete_confirmation (const char* name, bool trigger_open) {
	if (trigger_open)
		ImGui::OpenPopup("POPUP_delete_confirmation");

	GuiConfirm conf = GuiConfirm::PENDING;
	
	if (ImGui::BeginPopup("POPUP_delete_confirmation")) {

		ImGui::Text("Really delete %s?", name);
		
		if (ImGui::Button("Keep")) {
			conf = GuiConfirm::NO;
			ImGui::CloseCurrentPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_Button,        0xFF120CFF);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xE24E48FF);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  0xB8241EFF);
		
		ImGui::SameLine();
		if (ImGui::Button("DELETE")) {
			conf = GuiConfirm::YES;
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopStyleColor(3);

		ImGui::EndPopup();
	}

	return conf;
}

enum class GuiUnsavedConfirm {
	PENDING = 0,
	CANCEL,
	SAVE,
	DISCARD,
};
inline GuiUnsavedConfirm imgui_unsaved_changes_confirmation () {
	ImGui::OpenPopup("POPUP_unsaved_confirmation");

	GuiUnsavedConfirm conf = GuiUnsavedConfirm::PENDING;
	
	if (ImGui::BeginPopup("POPUP_unsaved_confirmation")) {

		ImGui::Text("You have unsaved changes.");
		
		if (ImGui::Button("Cancel")) {
			conf = GuiUnsavedConfirm::CANCEL;
			ImGui::CloseCurrentPopup();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("Save & Close")) {
			conf = GuiUnsavedConfirm::SAVE;
			ImGui::CloseCurrentPopup();
		}

		ImGui::PushStyleColor(ImGuiCol_Button,        0xFF120CFF);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xE24E48FF);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  0xB8241EFF);
		
		ImGui::SameLine();
		if (ImGui::Button("Discard & Close")) {
			conf = GuiUnsavedConfirm::DISCARD;
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopStyleColor(3);

		ImGui::EndPopup();
	}

	return conf;
}

enum LogLevel {
	LOG,
	INFO,
	WARNING,
	ERROR,
};
static inline const lrgba LOG_LEVEL_COLS[] = {
	srgba(216, 216, 216, 120),
	srgba(234, 234, 232),
	srgba(255, 220, 80),
	srgba(255, 100, 40),
};

struct Timing_Histogram {
	RunningAverage<float> avg = RunningAverage<float>(64);
	float latest_avg;
	float latest_min, latest_max, latest_std_dev;

	float update_period = .5f; // sec
	float update_timer = 0;

	int imgui_histo_height = 60;
	float y_axis_height = 20;

	// add new timing into circular buffer
	void push_timing (float seconds) {
		avg.push(seconds);
	}

	// compute averages from circular buffer every update_period seconds (only includes pushed timings, not initial 0s)
	// and display them via imgui
	void imgui_display (char const* name, float dt, bool default_open=false) { // dt for updating imgui every update_period (not for actual value being averaged)
		if (update_timer <= 0) {
			latest_avg = avg.calc_avg(&latest_min, &latest_max, &latest_std_dev);
			update_timer += update_period;
		}
		update_timer -= dt;

		if (ImGui::TreeNodeEx(name, default_open ? ImGuiTreeNodeFlags_DefaultOpen : 0,
			"%12s - %6.3fms", name, latest_avg * 1000)) {
			float avg_hz = 1.0f / latest_avg;
			ImGui::Text("avg: %5.1f hz (%6.3f ms  min: %6.3f  max: %6.3f  stddev: %6.3f)",
				avg_hz, latest_avg * 1000, latest_min * 1000, latest_max * 1000, latest_std_dev * 1000);

			ImGui::SetNextItemWidth(-1);
			ImGui::PlotHistogram("##frametimes_histogram", avg.data(), (int)avg.count(), 0, nullptr, 0, y_axis_height / 1000, ImVec2(0, (float)imgui_histo_height));

			if (ImGui::BeginPopupContextItem("##frametimes_histogram popup")) {
				ImGui::SliderInt("imgui_histo_height", &imgui_histo_height, 20, 120);
				ImGui::DragFloat("y_axis_height [ms]", &y_axis_height, 0.1f);

				int cap = (int)avg.capacity();
				if (ImGui::SliderInt("avg_count", &cap, 16, 1024)) {
					avg.resize(cap);
				}

				ImGui::EndPopup();
			}

			ImGui::TreePop();
		}
	}
};
