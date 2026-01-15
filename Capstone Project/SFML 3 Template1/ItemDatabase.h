#pragma once
#include "Item.h"
#include <vector>
#include <random>

class ItemDatabase
{
public:
    ItemDatabase();

    // Pick a random shop item using rarity weights
    Item getRandomShopItem();

private:
    std::vector<Item> m_common;
    std::vector<Item> m_uncommon;
    std::vector<Item> m_rare;
    std::vector<Item> m_epic;

    std::mt19937 m_rng;

    ItemRarity pickRarityWeighted() const;
};
