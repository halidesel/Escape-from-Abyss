#ifndef ITEMS_H
#define ITEMS_H

#include <string.h>

typedef enum {
    WEAPON,
    ARMOR,
    CONSUMABLE,
    KEY_ITEM,
    ARTIFACT
} ItemType;

// Item definitions
#define ITEM_COUNT 8

typedef struct {
    const char* name;
    const char* description;
    int damage;          // For weapons
    int defense;         // For armor
    int healing;         // For consumables
    ItemType type;
} ItemTemplate;

static const ItemTemplate ITEM_TEMPLATES[] = {
    {
        "Sword",
        "A sharp blade forged in the depths of the Abyss. Deals 20 damage.",
        20, 0, 0,
        WEAPON
    },
    {
        "Shield",
        "A solid shield that provides basic protection. Reduces incoming damage by 15%.",
        0, 15, 0,
        ARMOR
    },
    {
        "Healing Potion",
        "A mystical red potion that restores 20 HP when consumed.",
        0, 0, 20,
        CONSUMABLE
    },
        {
        "Big Healing Potion",
        "A great mystical potion that restores 50 HP when consumed, prepared by a wise wizard who claims to know every corner of the Forest.",
        0, 0, 50,
        CONSUMABLE
    },
    {
        "Key of The King's Palace",
        "A big key dropped by the Skeleton King. Grants access to the King's Palace.",
        0, 0, 0,
        KEY_ITEM
    },
    {
        "Dragon's Scale",
        "A legendary scale from the Abyss Dragon. Can be used to upgrade your shield.",
        0, 30, 0,
        ARTIFACT
    },
    {
        "Katana",
        "A masterfully crafted blade. Swift and deadly, deals 40 damage.",
        40, 0, 0,
        WEAPON
    },
    {
        "Torch of the Abyss",
        "Ancient torch that never burns out. Effective against demons and darkness.",
        15, 0, 0,
        WEAPON
    },
    {
        "King's Amulet",
        "A powerful artifact that protects against demon curses and dark magic. It is said that the King's Amulet was given to the King by the wise wizard",
        0, 20, 0,
        ARMOR
    }
};

#endif // ITEMS_H