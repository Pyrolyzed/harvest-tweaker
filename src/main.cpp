#include "Hooks.h"
#include "Settings.h"
#include "UI.h"

namespace {
    void OnMessage(SKSE::MessagingInterface::Message* a_msg) {
        switch (a_msg->type) {
            case SKSE::MessagingInterface::kPostLoad:
                Settings::Load();
                UI::Register();
                break;
            case SKSE::MessagingInterface::kDataLoaded:
                Hooks::Install();
                break;
        }
    }
	SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
		SKSE::Init(a_skse);

		SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
		return true;
	}
}