#include "UI.h"
#include "Settings.h"
#include "SKSEMenuFramework.h"

namespace {
    void Slider(const char* a_label, int* a_value) {
        ImGuiMCP::SliderInt(a_label, a_value, Settings::kMin, Settings::kMax);
        if (ImGuiMCP::IsItemDeactivatedAfterEdit()) {
            Settings::Save();
        }
    }
    void Checkbox(const char* a_label, bool* a_value) {
        if (ImGuiMCP::Checkbox(a_label, a_value)) {
            Settings::Save();
        }
    }
}

void UI::Register() {
    if (!SKSEMenuFramework::IsInstalled()) {
        SKSE::log::warn("SKSE Menu Framework not found, skipping menu registration");
        return;
    }

    SKSEMenuFramework::SetSection("Harvest Settings");
    SKSEMenuFramework::AddSectionItem("Woodcutting", RenderWoodcutting);
    SKSEMenuFramework::AddSectionItem("Mining", RenderMining);
    SKSEMenuFramework::AddSectionItem("Gathering", RenderGathering);
}

void __stdcall UI::RenderWoodcutting() {
    Slider("Firewood per chop", &Settings::wood.firewoodPerChop);
    Slider("Chops per use", &Settings::wood.chopsPerUse);
    Checkbox("Infinite chops", &Settings::wood.infiniteChops);
}

void __stdcall UI::RenderMining() {
    Slider("Ore per activation", &Settings::mining.orePerActivation);
    Slider("Activations per vein", &Settings::mining.activationsPerVein);
    Slider("Strikes per activation", &Settings::mining.strikesPerActivation);
    Checkbox("Infinite ore veins", &Settings::mining.infiniteVeins); 
}

void __stdcall UI::RenderGathering() {
    // Slider("Plants gathered", &Settings::gathering.plantsGathered);
    Slider("Minimum plants gathered", &Settings::gathering.minPlantsGathered);
    Slider("Maximum plants gathered", &Settings::gathering.maxPlantsGathered);
}