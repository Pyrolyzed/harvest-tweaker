#include "Hooks.h"
#include "Settings.h"

namespace {
    using ScriptPtr = RE::BSTSmartPointer<RE::BSScript::Object>;

    ScriptPtr GetScript(RE::TESObjectREFR* a_ref, const char* a_name) {
        ScriptPtr obj;
        auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        if (!vm || !a_ref) return obj;

        auto* policy = vm->GetObjectHandlePolicy();
        if (!policy) return obj;

        const auto handle =
            policy->GetHandleForObject(static_cast<RE::VMTypeID>(a_ref->GetFormType()), a_ref);
        vm->FindBoundObject(handle, a_name, obj);
        return obj;
    }

    // Auto properties are backed by a variable named "::<PropertyName>_var"
    bool NameContains(const char* a_name, std::string_view a_needle) {
        if (!a_name) return false;

        std::string name(a_name);
        std::transform(name.begin(), name.end(), name.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return name.find(a_needle) != std::string::npos;
    }
    void SetInt(RE::BSScript::Object* a_obj, const char* a_prop, std::int32_t a_value) {
        const std::string varName = std::string("::") + a_prop + "_var";
        if (auto* var = a_obj->GetVariable(RE::BSFixedString(varName.c_str()))) {
            var->SetSInt(a_value);
        } else {
            SKSE::log::warn("Property variable {} not found", varName);
        }
    }

    // ---- Woodcutting ----
    void ApplyWood(RE::TESObjectREFR* a_ref) {
        const auto obj = GetScript(a_ref, "ResourceFurnitureScript");
        if (!obj) return;

        const auto perChop = Settings::wood.firewoodPerChop;
        const auto chops = Settings::wood.chopsPerUse;

        // the script adds ResourceCount to a counter per chop and stops at
        // MaxResourcePerActivation, so chops = Max / ResourceCount
        SetInt(obj.get(), "ResourceCount", perChop);
        SetInt(obj.get(), "MaxResourcePerActivation", perChop * chops);
        SKSE::log::debug("Woodcutting applied: {} per chop, {} chops", perChop, chops);
    }

    // ---- Mining ----
    void ApplyMining(RE::TESObjectREFR* a_ref) {
        const auto obj = GetScript(a_ref, "MineOreScript");
        if (!obj) return;

        SetInt(obj.get(), "ResourceCount", Settings::mining.orePerActivation);
        SetInt(obj.get(), "ResourceCountTotal", Settings::mining.activationsPerVein);
        SetInt(obj.get(), "StrikesBeforeCollection", Settings::mining.strikesPerActivation);
        SetInt(obj.get(), "AttackStrikesBeforeCollection", Settings::mining.strikesPerActivation);
        SKSE::log::debug("Mining applied to {:08X}", a_ref->GetFormID());
    }

    // ---- Gathering ----
    bool IsHarvested(const RE::TESObjectREFR* a_ref) {
        return (a_ref->formFlags & static_cast<std::uint32_t>(RE::TESObjectREFR::RecordFlags::kHarvested)) != 0;
    }

    void Gather(const RE::TESActivateEvent* a_event) {
        auto* ref = a_event->objectActivated.get();
        auto* actor = a_event->actionRef.get();
        if (!ref || !actor || !actor->IsPlayerRef()) return;

        const auto extra = Settings::gathering.plantsGathered - 1;
        if (extra <= 0 || IsHarvested(ref)) return;

        auto* base = ref->GetBaseObject();
        if (!base) return;

        // Coin purses are excluded from gathering changes
        if (NameContains(base->GetName(), "coin purse")) return;

        RE::TESForm* produce = nullptr;
        if (auto* flora = base->As<RE::TESFlora>()) {
            produce = flora->produceItem;
        } else if (auto* tree = base->As<RE::TESObjectTREE>()) {
            produce = tree->produceItem;
        }

        auto* item = produce ? produce->As<RE::TESBoundObject>() : nullptr;
        if (!item || item->IsGold()) return;

        // vanilla activation still grants the final one
        RE::PlayerCharacter::GetSingleton()->AddObjectToContainer(item, nullptr, extra, nullptr);
        SKSE::log::debug("Gathering: added {} extra {:08X} from '{}'", extra, item->GetFormID(),
                        base->GetName());
    }

    // ---- Sinks ----
    class EventSinks : public RE::BSTEventSink<RE::TESActivateEvent>,
                       public RE::BSTEventSink<RE::TESHitEvent>,
                       public RE::BSTEventSink<RE::TESObjectLoadedEvent> {
    public:
        static EventSinks* GetSingleton() {
            static EventSinks instance;
            return &instance;
        }

        RE::BSEventNotifyControl ProcessEvent(const RE::TESActivateEvent* a_event,
                                              RE::BSTEventSource<RE::TESActivateEvent>*) override {
            if (a_event && a_event->objectActivated) {
                auto* ref = a_event->objectActivated.get();
                ApplyWood(ref);
                ApplyMining(ref);
                Gather(a_event);
            }
            return RE::BSEventNotifyControl::kContinue;
        }

        RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent* a_event,
                                              RE::BSTEventSource<RE::TESHitEvent>*) override {
            if (a_event && a_event->target) {
                ApplyMining(a_event->target.get());
            }
            return RE::BSEventNotifyControl::kContinue;
        }

        RE::BSEventNotifyControl ProcessEvent(const RE::TESObjectLoadedEvent* a_event,
                                              RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override {
            if (a_event && a_event->loaded) {
                if (auto* ref = RE::TESForm::LookupByID<RE::TESObjectREFR>(a_event->formID)) {
                    ApplyWood(ref);
                    ApplyMining(ref);
                }
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };
}

void Hooks::Install() {
    auto* holder = RE::ScriptEventSourceHolder::GetSingleton();
    if (!holder) {
        SKSE::log::error("ScriptEventSourceHolder unavailable");
        return;
    }

    auto* sinks = EventSinks::GetSingleton();
    holder->AddEventSink<RE::TESActivateEvent>(sinks);
    holder->AddEventSink<RE::TESHitEvent>(sinks);
    holder->AddEventSink<RE::TESObjectLoadedEvent>(sinks);
    SKSE::log::info("Harvest event sinks registered");
}