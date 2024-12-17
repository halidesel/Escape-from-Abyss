<h1 align="center">Escape  from  Abyss</h1>

<p align="center">A text-based basic dungeon exploration game where players navigate through interconnected dungeons, battle creatures, and attempt to escape from the Abyss.</p>

<hr>

## Game Overview:
In Escape from Abyss, you play as a hero trapped in a dangerous dungeon system. Your goal is to navigate through various areas, defeat various creatures, collect items, and find your way to freedom.

## Key Features:
- Text-based interface for a classic dungeon exploration experience.
- Turn-based combat system.
- Inventory management system.
- Multiple unique dungeons with different themes and challenges.
- Battle system with various creatures and their unique abilities.
- Item collection system and upgrade system.
- Save/Load game functionality.
- A quest to find the exit and escape from the Abyss.

## Installation:

1. Clone the repository:
```bash
git clone https://github.com/halidesel/Escape-from-Abyss.git
```

2. Navigate to the project directory:
```bash
cd escape-from-abyss
```

3. Run the game:
```bash
make run
```

## Gameplay:

### Basic Commands:
- `move`: Navigate between dungeons (up/down/left/right)
- `look`: Examine current dungeon
- `inventory`: Check and use items
- `pickup`: Collect items
- `drop`: Discard items
- `attack`: Engage in combat
- `menu`: Access main menu
- `exit`: Quit game

### Game Progression
1. Start in "Beginning of The Road"
2. Explore dungeons and collect items
3. Defeat creatures to clear paths
4. Find key items to access special areas
5. Upgrade equipment
6. Reach and defeat the final boss
7. Escape through the final dungeon

## Game Features:

### Combat System
- Turn-based battles
- Multiple weapon choices
- Armor defense system
- Various enemy types
- Boss battles

### Item System
- Weapons (Sword, Katana)
- Armor (Shield, Dragon Shield)
- Consumables (Healing Potions)
- Key Items
- Special Artifacts

### Save System
- Three save slots
- Complete game state preservation
- Load game functionality

## Code Structure:

```c
src/
├── Hw1.c # Main game logic
├── items.h # Item definitions and templates
└── text.h # Game text and dungeon descriptions
```

### Data Structures:

```c
// Player Structure
typedef struct {
char nickName;
int health;
int strength;
int inventoryCapacity;
Item inventory;
Dungeon currentDungeon;
} Player;
// Dungeon Structure
typedef struct Dungeon {
char name;
int code;
int itemCount;
int creatureCount;
Item items;
Creature creatures;
struct Dungeon up, down, left, right;
} Dungeon;
// Creature Structure
typedef struct {
    char *name;
    int health;
    int strength; 
} Creature;
// Item Structure
typedef struct {
    const ItemTemplate *template;
    int count;
} Item;
```
