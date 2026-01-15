#include "ItemDatabase.h"
#include <chrono>

ItemDatabase::ItemDatabase()
{
    // Seed RNG
    auto seed = static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    m_rng.seed(seed);

    // ----- Define items -----
    m_common = {
        { "Small Potion",  25, ItemRarity::Common },
        { "Throwing Knife", 40, ItemRarity::Common },
        { "Bandage",       15, ItemRarity::Common }
    };

    m_uncommon = {
        { "Medium Potion", 75, ItemRarity::Uncommon },
        { "Sturdy Boots", 120, ItemRarity::Uncommon }
    };

    m_rare = {
        { "Large Potion", 150, ItemRarity::Rare },
        { "Samurai Charm", 200, ItemRarity::Rare }
    };

    m_epic = {
        { "Blade of Rhythm", 500, ItemRarity::Epic }
    };
}

ItemRarity ItemDatabase::pickRarityWeighted() const
{
    // Example weights: Common 60, Uncommon 25, Rare 12, Epic 3
    static const int weights[] = { 60, 25, 12, 3 };
    int total = 0;
    for (int w : weights) total += w;

    // uniform int [0, total-1]
    std::uniform_int_distribution<int> dist(0, total - 1);
    int r = dist(const_cast<std::mt19937&>(m_rng)); // rng is mutable in effect

    int acc = 0;
    for (int i = 0; i < 4; ++i)
    {
        acc += weights[i];
        if (r < acc)
            return static_cast<ItemRarity>(i);
    }
    return ItemRarity::Common; // fallback
}

Item ItemDatabase::getRandomShopItem()
{
    ItemRarity rarity = pickRarityWeighted();
    std::uniform_int_distribution<std::size_t> dist;

    switch (rarity)
    {
    case ItemRarity::Common:
        dist = std::uniform_int_distribution<std::size_t>(0, m_common.size() - 1);
        return m_common[dist(m_rng)];

    case ItemRarity::Uncommon:
        dist = std::uniform_int_distribution<std::size_t>(0, m_uncommon.size() - 1);
        return m_uncommon[dist(m_rng)];

    case ItemRarity::Rare:
        dist = std::uniform_int_distribution<std::size_t>(0, m_rare.size() - 1);
        return m_rare[dist(m_rng)];

    case ItemRarity::Epic:
        dist = std::uniform_int_distribution<std::size_t>(0, m_epic.size() - 1);
        return m_epic[dist(m_rng)];
    }

    return { "Error Item", 0, ItemRarity::Common };
}