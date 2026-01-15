#pragma once
#include <string>

enum class ItemRarity { Common, Uncommon, Rare, Epic };

struct Item
{
    std::string name;
    int cost = 0;
    ItemRarity rarity = ItemRarity::Common;
};
