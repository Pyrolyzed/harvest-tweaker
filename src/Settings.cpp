#include "Settings.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>

namespace {
    const std::filesystem::path kPath{"Data/SKSE/Plugins/HarvestTweaks.ini"};

    using Ini = std::map<std::string, std::map<std::string, std::string>>;

    std::string Trim(const std::string& a_s) {
        const auto b = a_s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return {};
        const auto e = a_s.find_last_not_of(" \t\r\n");
        return a_s.substr(b, e - b + 1);
    }

    Ini Read() {
        Ini ini;
        std::ifstream in(kPath);
        std::string line, section;
        while (std::getline(in, line)) {
            line = Trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            if (line.front() == '[' && line.back() == ']') {
                section = Trim(line.substr(1, line.size() - 2));
                continue;
            }
            if (const auto eq = line.find('='); eq != std::string::npos) {
                ini[section][Trim(line.substr(0, eq))] = Trim(line.substr(eq + 1));
            }
        }
        return ini;
    }

    int GetInt(const Ini& a_ini, const std::string& a_section, const std::string& a_key, int a_default) {
        try {
            const auto s = a_ini.find(a_section);
            if (s == a_ini.end()) return a_default;
            const auto k = s->second.find(a_key);
            if (k == s->second.end()) return a_default;
            return std::clamp(std::stoi(k->second), Settings::kMin, Settings::kMax);
        } catch (...) {
            return a_default;
        }
    }
}

void Settings::Load() {
    const auto ini = Read();

    wood.firewoodPerChop = GetInt(ini, "Woodcutting", "iFirewoodPerChop", wood.firewoodPerChop);
    wood.chopsPerUse = GetInt(ini, "Woodcutting", "iChopsPerUse", wood.chopsPerUse);

    mining.orePerActivation = GetInt(ini, "Mining", "iOrePerActivation", mining.orePerActivation);
    mining.activationsPerVein = GetInt(ini, "Mining", "iActivationsPerVein", mining.activationsPerVein);
    mining.strikesPerActivation = GetInt(ini, "Mining", "iStrikesPerActivation", mining.strikesPerActivation);

    gathering.plantsGathered = GetInt(ini, "Gathering", "iPlantsGathered", gathering.plantsGathered);

    Save();  // creates the file on first run and normalizes out-of-range values
}

void Settings::Save() {
    std::error_code ec;
    std::filesystem::create_directories(kPath.parent_path(), ec);

    std::ofstream out(kPath, std::ios::trunc);
    if (!out) {
        SKSE::log::warn("Could not write {}", kPath.string());
        return;
    }

    out << "[Woodcutting]\n"
        << "iFirewoodPerChop=" << wood.firewoodPerChop << "\n"
        << "iChopsPerUse=" << wood.chopsPerUse << "\n\n"
        << "[Mining]\n"
        << "iOrePerActivation=" << mining.orePerActivation << "\n"
        << "iActivationsPerVein=" << mining.activationsPerVein << "\n"
        << "iStrikesPerActivation=" << mining.strikesPerActivation << "\n\n"
        << "[Gathering]\n"
        << "iPlantsGathered=" << gathering.plantsGathered << "\n";
}