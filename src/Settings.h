#pragma once

namespace Settings {
    inline constexpr int kMin = 1;
    inline constexpr int kMax = 1000;

    struct Woodcutting {
        int firewoodPerChop = 2;
        int chopsPerUse = 3;
        bool infiniteChops = false;
    };
    struct Mining {
        int orePerActivation = 1;
        int activationsPerVein = 3;
        int strikesPerActivation = 3;
        bool infiniteVeins = false;
    };
    struct Gathering {
        // int plantsGathered = 1;
        int minPlantsGathered = 1;
        int maxPlantsGathered = 1;
    };

    inline Woodcutting wood;
    inline Mining mining;
    inline Gathering gathering;

    void Load();
    void Save();
}