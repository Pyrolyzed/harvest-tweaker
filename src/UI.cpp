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
}

void __stdcall UI::RenderMining() {
    Slider("Ore per activation", &Settings::mining.orePerActivation);
    Slider("Activations per vein", &Settings::mining.activationsPerVein);
    Slider("Strikes per activation", &Settings::mining.strikesPerActivation);
}

void __stdcall UI::RenderGathering() {
    Slider("Plants gathered", &Settings::gathering.plantsGathered);
}