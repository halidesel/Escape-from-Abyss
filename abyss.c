#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "items.h"
#include "text.h"

// Global array for dungeon creature states
int dungeonCreatureStates[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

typedef struct {
    char *name;
    int health;
    int strength; 
} Creature;

typedef struct {
    const ItemTemplate *template;
    int count;
} Item;

typedef struct Dungeon {
    char *name;
    int code;
    int itemCount;
    int creatureCount;
    Item *items;
    Creature *creatures;
    struct Dungeon *up, *down, *left, *right;
} Dungeon;

typedef struct {
    char *nickName;
    int health;
    int strength;
    int inventoryCapacity;
    Item *inventory;
    Dungeon *currentDungeon;
} Player;

void menu(Player *player, Dungeon **dungeon);
void commands(char *commandName, Player *player, Dungeon **dungeon);
void createGame(Player *player, Dungeon **dungeon);
void cleanupPlayer(Player *player);
void saveGame(Player *player, const char *fileName);
void loadGame(Player *player, const char *fileName);
void move(Player *player, const char *direction);
void pickup(Player *player);
void drop(Player *player);
void inventory(Player *player);
void attack(Player *player);
Dungeon *createDungeon(const char *name,int code, int itemCount, int creatureCount, Item *items, Creature *creatures);
void printDungeon(Dungeon *dungeon);
void connectDungeons(Dungeon *from, Dungeon *to, const char *direction);
void setupDungeons(Dungeon **startingDungeon);
void cleanupDungeon(Dungeon *dungeon);
void generateCreatures(Dungeon **dungeon, int forcedCreatureType);
void cleanupGame(Player *player, Dungeon **dungeon);
void displayDeathScreen(Player *player);
void restoreConnections(Dungeon *currentDungeon);
Dungeon *createConnectedDungeon(Dungeon *templateDungeon);
Item *createItems(const ItemTemplate **templates, const int *counts, int numItems);

void menu(Player *player, Dungeon **dungeon) {
    int choice;
    printf("------------ ESCAPE FROM ABYSS ----------\n");
    printf("Select an option:\n");
    printf("1. Create a new game.\n");
    printf("2. Load game.\n");
    printf("3. Save game.\n");
    printf("4. List all commands.\n");
    printf("5. Exit.\n");
    printf("-----------------------------------------\n");
    printf("Developed by: Halide SEL\n");

    printf("\nEnter your choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            printf("Creating a new game...\n");
            createGame(player, dungeon);
            break;
        case 2:
            printf("\nAvailable save files:\n");
            for (int i = 1; i <= 3; i++) {
                char tempFilename[20];
                sprintf(tempFilename, "save_slot_%d.txt", i);
                FILE *file = fopen(tempFilename, "r");
                if (file != NULL) {
                    char buffer[256];
                    char nickname[100];
                    fgets(buffer, sizeof(buffer), file); // Skip "Player:" line
                    fgets(buffer, sizeof(buffer), file);
                    sscanf(buffer, "Nickname: %[^\n]", nickname);
                    printf("%d. save_slot_%d.txt - Player: %s\n", i, i, nickname);
                    fclose(file);
                } else {
                    printf("%d. save_slot_%d.txt - Player: NULL\n", i, i);
                }
            }
            printf("Choose a save file (1-3): ");
            int loadSlot;
            scanf("%d", &loadSlot);
            if (loadSlot >= 1 && loadSlot <= 3) {
                char filename[20];
                sprintf(filename, "save_slot_%d.txt", loadSlot);
                printf("Loading game from %s...\n", filename);
                loadGame(player, filename);
            } else {
                printf("ERROR!!: Invalid save file number. Please try again.\n");
            }
            break;
        case 3:
            printf("\nChoose a save slot:\n");
            for(int i = 1; i <= 3; i++) {
                char tempFilename[20];
                sprintf(tempFilename, "save_slot_%d.txt", i);
                FILE *tempFile = fopen(tempFilename, "r");
                if(tempFile != NULL) {
                    char buffer[256];
                    char nickname[100];
                    fgets(buffer, sizeof(buffer), tempFile); // Skip "Player:" line
                    fgets(buffer, sizeof(buffer), tempFile);
                    sscanf(buffer, "Nickname: %[^\n]", nickname);
                    printf("%d. save_slot_%d.txt - Player: %s\n", i, i, nickname);
                    fclose(tempFile);
                } else {
                    printf("%d. save_slot_%d.txt - Player: NULL\n", i, i);
                }
            }
            printf("Choose slot (1-3): ");
            int saveSlot;
            scanf("%d", &saveSlot);
            if(saveSlot >= 1 && saveSlot <= 3) {
                char filename[20];
                sprintf(filename, "save_slot_%d.txt", saveSlot);
                printf("Saving game to %s...\n", filename);
                saveGame(player, filename);
            } else {
                printf("ERROR!!: Invalid save file number. Please try again.\n");
            }
            break;
        case 4:
            printf("\nList of commands:\n");
            printf("-----------------------------------------\n");
            printf("\nMove: Move to a different dungeon.\n");
            printf("\nLook: Look around the dungeon.\n");
            printf("\nInventory: Check your inventory.\n");
            printf("\nPickup: Pickup an item.\n");
            printf("\nDrop: Drop an item.\n");
            printf("\nAttack: Attack a creature.\n");
            printf("\nMenu: Open the menu\n");
            printf("\nExit: Exit the game.\n");
            printf("\nPlease write all command in lowercase!\n");
            printf("-----------------------------------------\n");
            break;
        case 5:
            printf("Exiting...\n");
            exit(0);
        default:
            printf("ERROR!!: Invalid choice. Please try again.\n");
            break;
    }
}

void commands(char *commandName, Player *player, Dungeon **dungeon) {
    char direction[100];

    if (strcmp(commandName, "move") == 0) {
        move(player, direction);
    } else if (strcmp(commandName, "look") == 0) {
        printDungeon(player->currentDungeon);
    } else if (strcmp(commandName, "inventory") == 0) {
        inventory(player);
    } else if (strcmp(commandName, "pickup") == 0) {
        pickup(player);
    } else if (strcmp(commandName, "drop") == 0) {
        drop(player);
    } else if (strcmp(commandName, "attack") == 0) {
        attack(player);
    } else if (strcmp(commandName, "menu") == 0) {
        menu(player, dungeon);
    } else {
        printf("ERROR!!: Invalid command. Please try again.\n");
    }
}

void createGame(Player *player, Dungeon **dungeon) {
    player->nickName = (char*)malloc(100 * sizeof(char));
    if (player->nickName == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        exit(1);
    }

    printf("\nGive a name for your hero: ");
    scanf("%s", player->nickName);

    player->health = 100; // Default Value
    player->strength = 10; // Default Value
    player->inventoryCapacity = 10; // Default Value
    player->inventory = (Item*)malloc(player->inventoryCapacity * sizeof(Item));
    
    if (player->inventory == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < player->inventoryCapacity; i++) {
        player->inventory[i].template = NULL; // NULL olarak başlıyor
        player->inventory[i].count = 0; // NULL olarak başlıyor
    }

    Item *startingItems = (Item*)malloc(3 * sizeof(Item));
    if (startingItems == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        exit(1);
    }

    // STARTING ITEMS
    startingItems[0].template = &ITEM_TEMPLATES[0]; // Sword
    startingItems[0].count = 1;
    startingItems[1].template = &ITEM_TEMPLATES[1];  // Shield
    startingItems[1].count = 1;
    startingItems[2].template = &ITEM_TEMPLATES[2];  // Healing Potion
    startingItems[2].count = 1;

    *dungeon = createDungeon("Beginning of The Road",0, 3, 0, startingItems, NULL);
    setupDungeons(dungeon);
    player->currentDungeon = *dungeon;
    printf("Welcome, %s! You are in the Dungeons of Abyss. Please type 'look' to look around.\n", player->nickName);
}

void cleanupPlayer(Player *player) {
    if (player->nickName != NULL) {
        free(player->nickName);
        player->nickName = NULL;
    }
    if (player->inventory != NULL) {
            free(player->inventory);
            player->inventory = NULL;
        }
    player->health = 0;
    player->strength = 0;
    player->inventoryCapacity = 0;
    player->currentDungeon = NULL;
}

void saveGame(Player *player, const char *fileName) {
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        printf("ERROR!!: Failed to save game.\n");
        return;
    }

    fprintf(file, "Player:\n");
    fprintf(file, "Nickname: %s\n", player->nickName);
    fprintf(file, "Health: %d\n", player->health);
    fprintf(file, "Strength: %d\n", player->strength);
    fprintf(file, "Inventory Capacity: %d\n", player->inventoryCapacity);
    fprintf(file, "Inventory:\n");
    for (int i = 0; i < player->inventoryCapacity; i++) {
      if (player->inventory[i].template != NULL) {
            fprintf(file, "%s: %s\n", player->inventory[i].template->name, player->inventory[i].template->description);
        }
    }
    if (player->currentDungeon == NULL) {
        fprintf(file, "Current Dungeon: None\n");
    } else if (player->currentDungeon->name != NULL) {
        fprintf(file, "Current Dungeon: %s\n", player->currentDungeon->name);
        fprintf(file, "Current Dungeon Code: %d\n", player->currentDungeon->code);
        fprintf(file, "Item Count: %d\n", player->currentDungeon->itemCount);
        fprintf(file, "Creature Count: %d\n", player->currentDungeon->creatureCount);
        fprintf(file, "Items in Dungeon:\n");
        for (int i = 0; i < player->currentDungeon->itemCount; i++) {
            fprintf(file, "Item %d:\n", i);
            fprintf(file, "Name: %s\n", player->currentDungeon->items[i].template->name);
            fprintf(file, "Count: %d\n", player->currentDungeon->items[i].count);
            fprintf(file, "Type: %d\n", player->currentDungeon->items[i].template->type);
            fprintf(file, "Value: %d\n", player->currentDungeon->items[i].template->damage);
        }
        fprintf(file, "Creatures:\n");
        for (int i = 0; i < player->currentDungeon->creatureCount; i++) {
            fprintf(file, "%s, %d, %d\n", 
            player->currentDungeon->creatures[i].name,
            player->currentDungeon->creatures[i].health,
            player->currentDungeon->creatures[i].strength
            );
        }
        fprintf(file, "Dungeon Connections:\n");
        fprintf(file, "Up: %d\n", player->currentDungeon->up ? player->currentDungeon->up->code : -1);
        fprintf(file, "Down: %d\n", player->currentDungeon->down ? player->currentDungeon->down->code : -1);
        fprintf(file, "Left: %d\n", player->currentDungeon->left ? player->currentDungeon->left->code : -1);
        fprintf(file, "Right: %d\n", player->currentDungeon->right ? player->currentDungeon->right->code : -1);
    }
    // All Dungeon Creature States:
    fprintf(file, "Dungeon States:\n");
    for (int i = 0; i < 16; i++) {  // total 16 dungeon
        fprintf(file, "Dungeon_%d_Creatures:%d\n", i, (i == player->currentDungeon->code) ? player->currentDungeon->creatureCount : -1);
    }

    printf("Game is saved!\n");
    fclose(file);
}

void loadGame(Player *player, const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("ERROR!!: Failed to load game.\n");
        return;
    }

    // Skip the "Player:" line
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("ERROR!!: Failed to read player section.\n");
        fclose(file);
        return;
    }

    // Cleanup old player data if exists
    if (player->nickName != NULL) {
        free(player->nickName);
    }
    player->nickName = (char*)malloc(100 * sizeof(char));
    if (player->nickName == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        fclose(file);
        exit(1);
    }

    // Read player data
    if (fscanf(file, "Nickname: %[^\n]\n", player->nickName) != 1 ||
        fscanf(file, "Health: %d\n", &player->health) != 1 ||
        fscanf(file, "Strength: %d\n", &player->strength) != 1 ||
        fscanf(file, "Inventory Capacity: %d\n", &player->inventoryCapacity) != 1) {
        
        printf("ERROR!!: Failed to read player data.\n");
        free(player->nickName);
        fclose(file);
        return;
    }

    // Initialize inventory
    if (player->inventory != NULL) {
        free(player->inventory);
    }
    player->inventory = (Item*)malloc(player->inventoryCapacity * sizeof(Item));
    if (player->inventory == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        free(player->nickName);
        fclose(file);
        exit(1);
    }
    for (int i = 0; i < player->inventoryCapacity; i++) { // Initialize inventory slots
        player->inventory[i].template = NULL;
        player->inventory[i].count = 0;
    }

    // Skip "Inventory:" line
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("ERROR!!: Failed to read inventory section.\n");
        free(player->nickName);
        free(player->inventory);
        fclose(file);
        return;
    }

    // Read inventory items
    char itemName[256];
    int inventoryItemCount = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "Current Dungeon:", 16) == 0) {
            break;
        }
        // Parse item name (before the colon)
        char *colon = strchr(buffer, ':');
        if (colon != NULL) {
            size_t nameLen = colon - buffer;
            strncpy(itemName, buffer, nameLen);
            itemName[nameLen] = '\0';
            
            // Find matching template and add to inventory
            for (int i = 0; i < ITEM_COUNT + 1; i++) {
                if (strcmp(ITEM_TEMPLATES[i].name, itemName) == 0) {
                    if (inventoryItemCount < player->inventoryCapacity) {
                        player->inventory[inventoryItemCount].template = &ITEM_TEMPLATES[i];
                        player->inventory[inventoryItemCount].count = 1;
                        inventoryItemCount++;
                    }
                    break;
                }
            }
        }
    }

    // Read current dungeon
    char dungeonName[256];
    int dungeonCode, dungeonItemCount, dungeonCreatureCount;
    if (sscanf(buffer, "Current Dungeon: %[^\n]", dungeonName) != 1) {
        printf("ERROR!!: Failed to read current dungeon.\n");
        free(player->nickName);
        free(player->inventory);
        fclose(file);
        return;
    }

    if (player->currentDungeon != NULL && player->currentDungeon->creatures != NULL) {
    for (int i = 0; i < player->currentDungeon->creatureCount; i++) {
        if (player->currentDungeon->creatures[i].name != NULL) {
            free(player->currentDungeon->creatures[i].name);
            }
        }
        free(player->currentDungeon->creatures);
        player->currentDungeon->creatures = NULL;
        player->currentDungeon->creatureCount = 0;
    }

    if (strcmp(dungeonName, "None") == 0) {
        player->currentDungeon = NULL;
    } else {
        // Read dungeon properties
        if (fscanf(file, "Current Dungeon Code: %d\n", &dungeonCode) != 1 ||
        fscanf(file, "Item Count: %d\n", &dungeonItemCount) != 1 ||
        fscanf(file, "Creature Count: %d\n", &dungeonCreatureCount) != 1) {
        
        printf("ERROR!!: Failed to read dungeon properties.\n");
        free(player->nickName);
        free(player->inventory);
        fclose(file);
        return;
    }
        
        // Allocate and read items if exists
        Item *items = NULL;
        if (dungeonItemCount > 0) {
            items = (Item*)malloc(dungeonItemCount * sizeof(Item));
            if (items == NULL) {
                printf("ERROR!!: Memory allocation failed.\n");
                free(player->nickName);
                free(player->inventory);
                fclose(file);
                exit(1);
            }
            
            // Skip "Items:" line
            if (fgets(buffer, sizeof(buffer), file) == NULL) {
                printf("ERROR!!: Failed to read items section.\n");
                // free(items); // TESTING 55
                free(player->nickName);
                free(player->inventory);
                fclose(file);
                return;
            }
            
            // Read each item
            for (int i = 0; i < dungeonItemCount; i++) {
                if (fgets(buffer, sizeof(buffer), file) == NULL) {
                    printf("ERROR!!: Failed to read item header.\n");
                    // free(items); // TESTING 55
                    free(player->nickName);
                    free(player->inventory);
                    fclose(file);
                    return;
                }
                char itemName[256];
                int count, type, value;
                if (fgets(buffer, sizeof(buffer), file) == NULL ||
                    sscanf(buffer, "Name: %[^\n]", itemName) != 1 ||
                    fgets(buffer, sizeof(buffer), file) == NULL ||
                    sscanf(buffer, "Count: %d", &count) != 1 ||
                    fgets(buffer, sizeof(buffer), file) == NULL ||
                    sscanf(buffer, "Type: %d", &type) != 1 ||
                    fgets(buffer, sizeof(buffer), file) == NULL ||
                    sscanf(buffer, "Value: %d", &value) != 1) {
        
                    printf("ERROR!!: Failed to read item details.\n");
                    // free(items); // TESTING 55
                    free(player->nickName);
                    free(player->inventory);
                    fclose(file);
                    return;
                }
                
                // Find matching template
                items[i].template = NULL;
                for (int j = 0; j < ITEM_COUNT + 1; j++) {
                    if (strcmp(ITEM_TEMPLATES[j].name, itemName) == 0) {
                        items[i].template = &ITEM_TEMPLATES[j];
                        items[i].count = count;
                        break;
                    }
                }
                if (items[i].template == NULL) {
                    printf("ERROR!!: Unknown item '%s' found in save file.\n", itemName);
                }
            }
        }
        
        // Allocate and read creatures if exists
        Creature *creatures = NULL;
        if (dungeonCreatureCount > 0) {
            creatures = (Creature*)malloc(dungeonCreatureCount * sizeof(Creature));
            if (creatures == NULL) {
                if (items) free(items);
                printf("ERROR!!: Memory allocation failed.\n");
                free(player->nickName);
                free(player->inventory);
                fclose(file);
                exit(1);
            }
            
            // Skip "Creatures:" line -> HATALI DÜZELT
            if (fgets(buffer, sizeof(buffer), file) == NULL) {
                printf("ERROR!!: Failed to read creature section.\n");
                if (items) free(items);
                free(player->nickName);
                free(player->inventory);
                fclose(file);
                return;
            }
            
            // Read each creature
            for (int i = 0; i < dungeonCreatureCount; i++) {
                char creatureName[100];
                int health, strength;
                if (fscanf(file, "%[^,], %d, %d\n", creatureName, &health, &strength) != 3) {
                    printf("ERROR!!: Failed to read creature data.\n");
                    if (items) free(items);
                    free(creatures);
                    free(player->nickName);
                    free(player->inventory);
                    fclose(file);
                    return;
                }

                creatures[i].name = strdup(creatureName);
                if (creatures[i].name == NULL) {
                    printf("ERROR!!: Memory allocation failed.\n");
                    if (items) free(items);
                    free(creatures);
                    free(player->nickName);
                    free(player->inventory);
                    fclose(file);
                    exit(1);
                }
                creatures[i].health = health;
                creatures[i].strength = strength;
            }
        }
        
        // Create the dungeon
        player->currentDungeon = createDungeon(dungeonName, dungeonCode, dungeonItemCount, dungeonCreatureCount, items, creatures);
        restoreConnections(player->currentDungeon); // TESTING 55
        if (player->currentDungeon == NULL) {
            printf("ERROR!!: Failed to create dungeon.\n");
            if (items) free(items);
            if (creatures) {
                for (int i = 0; i < dungeonCreatureCount; i++) {
                    free(creatures[i].name);
                }
                free(creatures);
            }
            free(player->nickName);
            free(player->inventory);
            fclose(file);
            return;
        }
        // Read dungeon connections
        int upCode, downCode, leftCode, rightCode;
        if (fscanf(file, "Dungeon Connections:\n") == 0 &&
            fscanf(file, "Up: %d\n", &upCode) == 1 &&
            fscanf(file, "Down: %d\n", &downCode) == 1 &&
            fscanf(file, "Left: %d\n", &leftCode) == 1 &&
            fscanf(file, "Right: %d\n", &rightCode) == 1) {

            // Restore connections
            restoreConnections(player->currentDungeon);
        }
    }

    static int dungeonCreatureStates[16];  // -1: Default, 0: ALl Creatures Dead, >0: Creature Count
    
    for (int i = 0; i < 16; i++) {
        dungeonCreatureStates[i] = -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int dungeonID, creatureCount;
        if (sscanf(line, "Dungeon_%d_Creatures:%d", &dungeonID, &creatureCount) == 2) {
            dungeonCreatureStates[dungeonID] = creatureCount;
        }
    }

    printf("Game loaded successfully! Welcome back, %s! You are in %s\n", player->nickName, player->currentDungeon ? player->currentDungeon->name : "unknown");
    fclose(file);
}

void move(Player *player, const char *direction) {

    Dungeon *nextDungeon = NULL;
    int validMove = 0;

    if (player->currentDungeon == NULL) {
        printf("ERROR!!: You are not in a dungeon to move.\n");
        return;
    }

    // OYUNU BİTİRDİKTEN SONRA YORUM SATIRINI KALDIR!
    if (player->currentDungeon->creatureCount > 0) {
        printf("\nYou cannot move in that direction because there are creatures in this area.\n");
        printf("Defeat them all creatures first to move.\n");
        printf("\n");
        return;
    }

    while (!validMove) {
        printf("Enter the direction to move (up, down, left, right) or '0' to cancel: ");
        scanf("%s", direction);

        if (strcmp(direction, "0") == 0) {
            printf("Movement cancelled!\n");
            return;
        }

        if (strcmp(direction, "up") == 0) {
            nextDungeon = player->currentDungeon->up;
            validMove = 1;
        } else if (strcmp(direction, "down") == 0) {
            nextDungeon = player->currentDungeon->down;
            validMove = 1;
        } else if (strcmp(direction, "left") == 0) {
            nextDungeon = player->currentDungeon->left; 
            validMove = 1;
        } else if (strcmp(direction, "right") == 0) {
            nextDungeon = player->currentDungeon->right;
            validMove = 1;
        } else {
            printf("Invalid direction. Please enter up, down, left, or right.\n");
        }
    }

    if (nextDungeon) {
        if (strcmp(nextDungeon->name, "The King's Palace") == 0) {
            int hasKey = 0;
            for (int i = 0; i < player->inventoryCapacity; i++) {
                if (player->inventory[i].template != NULL && player->inventory[i].template->type == KEY_ITEM) {
                    hasKey = 1;
                    break;
                }
            }
            if (!hasKey) {
                printf("\nYou need a key to enter the King's Palace.\n");
                return;
            }
        }
    
        player->currentDungeon = nextDungeon;
        restoreConnections(player->currentDungeon);

        if (strcmp(player->currentDungeon->name, "Escape Dungeon") == 0) {
            printf("\n\n");
            printf("********************************\n");
            printf("*       CONGRATULATIONS!       *\n");
            printf("********************************\n\n");
            printf("   You have escaped the Abyss!\n\n");
            printf("   Hero: %s\n", player->nickName);
            printf("   Final Health: %d\n", player->health);
            printf("   Final Strength: %d\n\n", player->strength);
            printf("   You have proven yourself\n");
            printf("   worthy and your name will be\n");
            printf("   remembered for generations!\n\n");
            printf("********************************\n");
            printf("Press ENTER to exit the game...\n");
            getchar();
            getchar(); // Consume the newline and wait for key press
            exit(0);
}

        if (!nextDungeon->up && !nextDungeon->down && !nextDungeon->left && !nextDungeon->right) {
            restoreConnections(player->currentDungeon);
        }
        printf("You moved up to the %s.\n", nextDungeon->name);
    } else {
        printf("You cannot move in that direction.\n");
    }
}

Dungeon *createDungeon(const char *name,int code, int itemCount, int creatureCount, Item *items, Creature *creatures) {
    Dungeon *newDungeon = (Dungeon *)malloc(sizeof(Dungeon));
    if (newDungeon == NULL) {
        printf("ERROR!!: Memory allocation failed for dungeon.\n");
        return NULL;
    }
    newDungeon->name = strdup(name);
    if (newDungeon->name == NULL) {
        printf("ERROR!!: Memory allocation failed for dungeon name.\n");
        free(newDungeon);
        return NULL;
    }
    newDungeon->code = code;
    newDungeon->itemCount = itemCount;
    newDungeon->creatureCount = creatureCount;

    if (itemCount > 0 && items != NULL) {
        newDungeon->items = (Item *)malloc(itemCount * sizeof(Item));
        if (newDungeon->items == NULL) {
            free(newDungeon);
            free(newDungeon->name);
            printf("ERROR!!: Memory allocation failed for items.\n");
            return NULL;
        }
        for (int i = 0; i < itemCount; i++) {
            newDungeon->items[i] = items[i];
        }
    } else {
        newDungeon->items = NULL;
    }

    if (creatureCount > 0 && creatures != NULL) {
        newDungeon->creatures = (Creature *)malloc(creatureCount * sizeof(Creature));
        if (newDungeon->creatures == NULL) {
            if (newDungeon->items) free(newDungeon->items);
            free(newDungeon);
            free(newDungeon->name);
            printf("ERROR!!: Memory allocation failed for creatures.\n");
            return NULL;
        }
        for (int i = 0; i < creatureCount; i++) {
            newDungeon->creatures[i] = creatures[i];
        }
    } else {
        newDungeon->creatures = NULL;
    }
    newDungeon->up = newDungeon->down = newDungeon->left = newDungeon->right = NULL;
    return newDungeon;
}

void printDungeon(Dungeon *dungeon) {
    printf("\nYou are in %s right now! You can see the creatures and items in this dungeon bellow.\n", dungeon->name);
    if (dungeon->code != 0) {
        printf("\nBrief:\n%s\n\n", DUNGEON_DESCRIPTIONS[dungeon->code]);
    }

    if (dungeon->itemCount > 0) {
        printf("Items:\n");
        for (int i = 0; i < dungeon->itemCount; i++) {
            printf("- %s: %s\n", dungeon->items[i].template->name, dungeon->items[i].template->description);
        }
    } else {
        printf("Items: NONE\n");
    }
    if (dungeon->creatureCount > 0) {
        printf("Creatures:\n");
        for (int i = 0; i < dungeon->creatureCount; i++) {
            printf("- %s (Health: %d, Strength: %d)\n", dungeon->creatures[i].name, dungeon->creatures[i].health, dungeon->creatures[i].strength);
        }
    } else {
        printf("Creatures: NONE\n");
    }
    printf("Connections:\n");
    if (dungeon->up) printf("  Up: %s\n", dungeon->up->name);
    if (dungeon->down) printf("  Down: %s\n", dungeon->down->name);
    if (dungeon->left) printf("  Left: %s\n", dungeon->left->name);
    if (dungeon->right) printf("  Right: %s\n", dungeon->right->name);
    printf("\n");
}

void connectDungeons(Dungeon *from, Dungeon *to, const char *direction) {
    if (strcmp(direction, "up") == 0) {
        from->up = to;
    } else if (strcmp(direction, "down") == 0) {
        from->down = to;
    } else if (strcmp(direction, "left") == 0) {
        from->left = to;
    } else if (strcmp(direction, "right") == 0) {
        from->right = to;
    }
}


// Helper function for Dungeons
Item *createItems(const ItemTemplate **templates, const int *counts, int numItems) {
    Item *items = (Item*)malloc(numItems * sizeof(Item));
    if (items == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        exit(1);
    }
    for (int i = 0; i < numItems; i++) {
        items[i].template = templates[i];
        items[i].count = counts[i];
    }
    return items;
}

void setupDungeons(Dungeon **startingDungeon) {
    // Dead Valley:
    const ItemTemplate *deadValleyTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int deadValleyCounts[] = {1};
    Item *itemsDeadValley = createItems(deadValleyTemplates, deadValleyCounts, 1);

    // King's Palace:
    const ItemTemplate *kingsPalaceTemplates[] = {
        &ITEM_TEMPLATES[8], // King's Amulet
    };
    const int kingsPalaceCounts[] = {1};
    Item *itemsKingsPalace = createItems(kingsPalaceTemplates, kingsPalaceCounts, 1);

    // Spider Nest:
    const ItemTemplate *spiderNestTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
        &ITEM_TEMPLATES[3], // Big Healing Potion
        &ITEM_TEMPLATES[7], // Torch of the Abyss (effective against darkness)
    };
    const int spiderNestCounts[] = {1, 1, 1};
    Item *itemsSpiderNest = createItems(spiderNestTemplates, spiderNestCounts, 3);

    // Skeleton King:
    const ItemTemplate *skeletonKingTemplates[] = {
        &ITEM_TEMPLATES[4], // Key of King's Palace
    };
    const int skeletonKingCounts[] = {1};
    Item *itemsSkeletonKing = createItems(skeletonKingTemplates, skeletonKingCounts, 1);

    // Path of the Ronin:
    const ItemTemplate *pathOfTheRoninTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int pathOfTheRoninCounts[] = {1};
    Item *itemsPathOfTheRonin = createItems(pathOfTheRoninTemplates, pathOfTheRoninCounts, 1);

    // Hall of Blades:
    const ItemTemplate *hallOfBladesTemplates[] = {
        &ITEM_TEMPLATES[6], // Katana
    };
    const int hallOfBladesCounts[] = {1};
    Item *itemsHallOfBlades = createItems(hallOfBladesTemplates, hallOfBladesCounts, 1);

    // Crimson Abyss:
    const ItemTemplate *crimsonAbyssTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int crimsonAbyssCounts[] = {1, 1};
    Item *itemsCrimsonAbyss = createItems(crimsonAbyssTemplates, crimsonAbyssCounts, 2);

    // Mount of Abyss:
    const ItemTemplate *mountOfAbyssTemplates[] = {
        &ITEM_TEMPLATES[5], // Dragon's Scale
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int mountOfAbyssCounts[] = {1, 1};
    Item *itemsMountOfAbyss = createItems(mountOfAbyssTemplates, mountOfAbyssCounts, 2);

    // Zombie Cave:
    const ItemTemplate *zombieCaveTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int zombieCaveCounts[] = {1};
    Item *itemsZombieCave = createItems(zombieCaveTemplates, zombieCaveCounts, 1);

    // Top of the Mount of Abyss:
    const ItemTemplate *topOfTheMountOfAbyssTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int topOfTheMountOfAbyssCounts[] = {1};
    Item *itemsTopOfTheMountOfAbyss = createItems(topOfTheMountOfAbyssTemplates, topOfTheMountOfAbyssCounts, 1);

    // Create dungeons
    Dungeon *dungeon1 = createDungeon("Dead Valley",1, 1, 0, itemsDeadValley, NULL);
    Dungeon *dungeon2 = createDungeon("Route of the King's Palace",2, 0, 0, NULL, NULL);
    Dungeon *dungeon3 = createDungeon("The King's Palace", 3, 1, 0, itemsKingsPalace, NULL); // Girebilmek için key lazım!-> Special Item: King's Amulet
    Dungeon *dungeon4 = createDungeon("Crooked Forest: Spider Nest", 4, 3, 0, itemsSpiderNest, NULL);
    Dungeon *dungeon5 = createDungeon("Crooked Forest: Skeleton King", 5, 1, 0, itemsSkeletonKing, NULL); // Skeleton King -> drop Key
    Dungeon *dungeon6 = createDungeon("Exit of the Crooked Forest", 6, 0, 0, NULL, NULL);
    Dungeon *dungeon7 = createDungeon("Road to the Dragon's Lair", 7, 0, 0, NULL, NULL);
    Dungeon *dungeon8 = createDungeon("Path of the Ronin", 8, 1, 0, itemsPathOfTheRonin, NULL);
    Dungeon *dungeon9 = createDungeon("Hall of Blades", 9, 1, 0, itemsHallOfBlades, NULL); // Special Item: Katana
    Dungeon *dungeon10 = createDungeon("Crimson Abyss", 10, 2, 0, itemsCrimsonAbyss, NULL); // Trap: Lava
    Dungeon *dungeon11 = createDungeon("Entrance to Mount of Abyss", 11, 2, 0, itemsMountOfAbyss, NULL); // Dragon -> drop Dragon's Scale
    Dungeon *dungeon12 = createDungeon("Mount of Abyss: Zombie Cave", 12, 1, 0, itemsZombieCave, NULL);
    Dungeon *dungeon13 = createDungeon("Mount of Abyss: Dead End", 13, 0, 0, NULL, NULL);
    Dungeon *dungeon14 = createDungeon("Top of the Mount of Abyss", 14, 1, 0, itemsTopOfTheMountOfAbyss, NULL); // Demon of the Abyss
    Dungeon *dungeonEscape = createDungeon("Escape Dungeon", 15, 0, 0, NULL, NULL);

    //Generate Creatures for each dungeon
    generateCreatures(&dungeon1, 0);
    generateCreatures(&dungeon2, 1);
    generateCreatures(&dungeon3, -1);
    generateCreatures(&dungeon4, 2);
    generateCreatures(&dungeon5, 3); // SKELETON KING and WARRIORS
    generateCreatures(&dungeon6, 1);
    generateCreatures(&dungeon7, 0);
    generateCreatures(&dungeon8, 2);
    generateCreatures(&dungeon9, -1);
    generateCreatures(&dungeon10, 1);
    generateCreatures(&dungeon11, 4); // DRAGON
    generateCreatures(&dungeon12, 0);
    generateCreatures(&dungeon13, 2);
    generateCreatures(&dungeon14, 5); // DEMON OF THE ABYSS

    // Connect dungeons
    connectDungeons(*startingDungeon, dungeon1, "right");
    connectDungeons(dungeon1, dungeon2, "right");
    connectDungeons(dungeon1, dungeon4, "down");
    connectDungeons(dungeon2, dungeon1, "left");
    connectDungeons(dungeon2, dungeon5, "down");
    connectDungeons(dungeon2, dungeon3, "right");
    connectDungeons(dungeon3, dungeon2, "left");
    connectDungeons(dungeon4, dungeon1, "up");
    connectDungeons(dungeon4, dungeon5, "right");
    connectDungeons(dungeon4, dungeon7, "down");
    connectDungeons(dungeon4, dungeon6, "left");
    connectDungeons(dungeon5, dungeon4, "left");
    connectDungeons(dungeon5, dungeon2, "up");
    connectDungeons(dungeon6, dungeon4, "right");
    connectDungeons(dungeon6, dungeon8, "down");
    connectDungeons(dungeon7, dungeon4, "up");
    connectDungeons(dungeon7, dungeon8, "left");
    connectDungeons(dungeon7, dungeon10, "down");
    connectDungeons(dungeon7, dungeon11, "right");
    connectDungeons(dungeon8, dungeon6, "up");
    connectDungeons(dungeon8, dungeon7, "right");
    connectDungeons(dungeon8, dungeon9, "down");
    connectDungeons(dungeon9, dungeon8, "up");
    connectDungeons(dungeon10, dungeon7, "up");
    connectDungeons(dungeon11, dungeon7, "left");
    connectDungeons(dungeon11, dungeon12, "right");
    connectDungeons(dungeon12, dungeon13, "up");
    connectDungeons(dungeon12, dungeon14, "down");
    connectDungeons(dungeon13, dungeon12, "down");
    connectDungeons(dungeon14, dungeon12, "up");
    connectDungeons(dungeon14, dungeonEscape, "right");
}

void cleanupDungeon(Dungeon *dungeon) {

    if (dungeon == NULL) {
        return;
    }

    if (dungeon->name != NULL) {
        free(dungeon->name);
    }

    if (dungeon->items != NULL) {
        free(dungeon->items);
    }

    if (dungeon->creatures != NULL) {
        for (int i = 0; i < dungeon->creatureCount; i++) {
            if (dungeon->creatures[i].name != NULL) {
                free(dungeon->creatures[i].name);
                dungeon->creatures[i].name = NULL;
            }
        }
        free(dungeon->creatures);
    }

    dungeon->name = NULL;
    dungeon->items = NULL;
    dungeon->creatures = NULL;
    dungeon->itemCount = 0;
    dungeon->creatureCount = 0;
    dungeon->up = dungeon->down = dungeon->left = dungeon->right = NULL;
    free(dungeon);
}

void generateCreatures(Dungeon **dungeon, int forcedCreatureType) {

    // Creatures Group List
    // 1. Zombies    
    // 2. Werewolf and Wolves
    // 3. Spiders
    // 4. Skeleton King + Skeletons [BOSS]
    // 5. Dragon [BOSS]
    // 6. Demon of the Abyss [BOSS]

    if (forcedCreatureType == -1) {
        (*dungeon)->creatureCount = 0;
        (*dungeon)->creatures = NULL;
        return;
    }

    srand(time(NULL));

    (*dungeon)->creatures = (Creature*)malloc(sizeof(Creature) * 10); // MAX 10 CREATURES
    if ((*dungeon)->creatures == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        exit(1);
    }

    int creatureType = (forcedCreatureType >= 0) ? forcedCreatureType : rand() % 6;

    switch(creatureType) {
        case 0: // Zombies 3-5
            (*dungeon)->creatureCount = 3 + (rand() % 3);
            for (int i = 0; i < (*dungeon)->creatureCount; i++) {   
                (*dungeon)->creatures[i].name = strdup("Zombie");
                (*dungeon)->creatures[i].health = 30;
                (*dungeon)->creatures[i].strength = 5;
            }
            break;
        case 1: // Werewolf and Wolves (1 Werewolf, 2-3 Wolves)
            (*dungeon)->creatureCount = 3 + (rand() % 2);
            (*dungeon)->creatures[0].name = strdup("Werewolf");
            (*dungeon)->creatures[0].health = 50;
            (*dungeon)->creatures[0].strength = 15;

            for (int i = 1; i < (*dungeon)->creatureCount; i++) {
                (*dungeon)->creatures[i].name = strdup("Wolf");
                (*dungeon)->creatures[i].health = 20;
                (*dungeon)->creatures[i].strength = 10;
            }
            break;
        case 2: // Spiders 4-6
            (*dungeon)->creatureCount = 4 + (rand() % 3);
            for (int i = 0; i < (*dungeon)->creatureCount; i++) {
                (*dungeon)->creatures[i].name = strdup("Spider");
                (*dungeon)->creatures[i].health = 15;
                (*dungeon)->creatures[i].strength = 8;
            }
            break;
        case 3: // Skeleton King + Skeletons (1 Skeleton King, 2-3 Skeletons) [BOSS]
            (*dungeon)->creatureCount = 3 + (rand() % 2);
            (*dungeon)->creatures[0].name = strdup("Skeleton King");
            (*dungeon)->creatures[0].health = 100;
            (*dungeon)->creatures[0].strength = 20;
            for (int i = 1; i < (*dungeon)->creatureCount; i++) {
                (*dungeon)->creatures[i].name = strdup("Skeleton Warrior");
                (*dungeon)->creatures[i].health = 25;
                (*dungeon)->creatures[i].strength = 10;
            }
            break;  
        case 4: // Dragon [BOSS]
            (*dungeon)->creatureCount = 1;
            (*dungeon)->creatures[0].name = strdup("Dragon");
            (*dungeon)->creatures[0].health = 150;
            (*dungeon)->creatures[0].strength = 30;
            break;  
        case 5: // Demon of the Abyss [FINAL BOSS]
            (*dungeon)->creatureCount = 1;
            (*dungeon)->creatures[0].name = strdup("Demon of the Abyss");
            (*dungeon)->creatures[0].health = 200;
            (*dungeon)->creatures[0].strength = 40;
            break;
    }
}


void drop(Player *player) {
    //Check player is in a dungeon
    if (player->currentDungeon == NULL) {
        printf("ERROR!!: You are not in a dungeon to drop an item.\n");
        return;
    }

    //Check player iventory is empty
    int hasItem = 0;
    for (int i = 0; i < player->inventoryCapacity; i++) {
        if (player->inventory[i].template != NULL) {
            hasItem = 1;
            break;
        }
    }
    if (!hasItem) {
        printf("ERROR!!: You have no items to drop.\n");
        return;
    }

    //Display Inventory
    printf("\n---- YOUR INVENTORY ----\n");
    for (int i = 0; i < player->inventoryCapacity; i++) {
        if (player->inventory[i].template != NULL) {
            printf("%d. - %s: %s\n", i+1, player->inventory[i].template->name, player->inventory[i].template->description);
        }
    }

    int choice;
    printf("\nEnter the number of the item you want to drop (0 to cancel): ");
    scanf("%d", &choice);

    if (choice == 0) {
        printf("Item drop cancelled!\n");
        return;
    }

    if (choice < 1 || choice > player->inventoryCapacity) {
        printf("ERROR!!: Invalid item number!\n");
        return;
    }

    int itemIndex = choice - 1;
    if (player->inventory[itemIndex].template == NULL) {
        printf("ERROR!!: No item in that slot. Please try again.\n");
        return;
    }

    // Allocate space for new items
    Item *newItems = (Item*)realloc(player->currentDungeon->items, (sizeof(Item) * (player->currentDungeon->itemCount + 1)));
    if (newItems == NULL) {
        printf("ERROR!!: Memory allocation failed.\n");
        return;
    }

    // Add item to dungeon
    player->currentDungeon->items = newItems;
    int dungeonIndex = player->currentDungeon->itemCount;
    player->currentDungeon->items[dungeonIndex] = player->inventory[itemIndex];
    player->currentDungeon->itemCount++;

    // Remove item from player inventory
    printf("You dropped %s!\n", player->inventory[itemIndex].template->name);
    player->inventory[itemIndex].template = NULL;
    player->inventory[itemIndex].count = 0;
}

void inventory(Player *player) {
    int hasItem = 0;
    printf("\n---- YOUR INVENTORY ----\n");
    printf("Name: %s\n", player->nickName);
    printf("Health: %d\n", player->health);
    printf("Strength: %d\n", player->strength);
    printf("Inventory Capacity: %d\n", player->inventoryCapacity);
    printf("\nItems:\n");
    for (int i = 0; i < player->inventoryCapacity; i++) {
        if (player->inventory[i].template != NULL) {
            printf("%d. - %s: %s\n", i+1, player->inventory[i].template->name, player->inventory[i].template->description);
            hasItem = 1;
        }
    }

    if (!hasItem) {
        printf("Your inventory is empty right now...\n");
        printf("------------------------\n\n");
        return;
    }

    printf("------------------------\n\n");
    printf("Would you like to use an item? (1: Yes, 0: No): ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter the number of the item you want to use (0 to cancel): ");
        int itemChoice;
        scanf("%d", &itemChoice);

        if (itemChoice == 0) {
            printf("Item usage cancelled!\n");
            return;
        }
        if (itemChoice < 1 || itemChoice > player->inventoryCapacity || player->inventory[itemChoice-1].template == NULL) {
            printf("ERROR!!: Invalid item number. Please try again.\n");
            return;
        }
        Item *selectedItem = &player->inventory[itemChoice-1];

        if (strcmp(selectedItem->template->name, "Dragon's Scale") == 0) {
            int shieldUpgrade = 0;
            for (int i = 0; i < player->inventoryCapacity; i++) {
                if (player->inventory[i].template != NULL && strcmp(player->inventory[i].template->name, "Shield") == 0) {
                    ItemTemplate *upgradedShield = malloc(sizeof(ItemTemplate));
                    if (upgradedShield == NULL) {
                        printf("ERROR!!: Memory allocation failed.\n");
                        return;
                    }

                    if (player->inventory[i].template != &ITEM_TEMPLATES[1]) {
                        free((void*)player->inventory[i].template);
                    }

                    *upgradedShield = *player->inventory[i].template;
                    upgradedShield->name = strdup("Dragon's Shield");
                    upgradedShield->description = strdup("A shield made from the Dragon's Scale. Reduces incoming damage by 45%..");
                    upgradedShield->defense = 45;

                    player->inventory[i].template = upgradedShield;

                    selectedItem->template = NULL;
                    selectedItem->count = 0;
                    
                    printf("You upgraded your shield to %d damage!\n", upgradedShield->damage);
                    shieldUpgrade = 1;
                    break;
                }
            }
            if (!shieldUpgrade) {
                printf("You don't have a shield to upgrade!\n");
            }
        }
        else if (selectedItem->template->type == CONSUMABLE) {
            if (player->health >= 100) {
                printf("Your health already full! You can't use this item.\n");
                return;
            }
            printf("You used %s!\n", selectedItem->template->name);
            int healAmount = selectedItem->template->healing;
            int actualHeal = 0;
            if (player->health + healAmount > 100) {
                actualHeal = 100 - player->health;
                player->health = 100;
            } else {
                actualHeal = healAmount;
                player->health += healAmount;
            }
            printf("You healed %d HP! Current Health: %d\n", actualHeal, player->health );
            selectedItem->count--;
            if (selectedItem->count == 0) {
                selectedItem->template = NULL;
            }
        } else {
            printf("You cannot use this item!\n");
        }
    }
    printf("------------------------\n\n");
}

void attack(Player *player) {
    extern int dungeonCreatureStates[16]; // access global array
    if (player->currentDungeon->creatureCount == 0) {
        printf("There are no creatures in this dungeon.\n");
        return;
    }
    printf("Available creatures to attack:\n");
    for (int i = 0; i < player->currentDungeon->creatureCount; i++) {
        printf("%d. - %s (Health: %d, Strength: %d)\n", i+1, player->currentDungeon->creatures[i].name, player->currentDungeon->creatures[i].health, player->currentDungeon->creatures[i].strength);
    }

    int choice;
    printf("Enter the number of the creature you want to attack (0 to cancel): ");
    scanf("%d", &choice);

    if (choice == 0) {
        printf("Creature attack cancelled!\n");
        return;
    }

    if (choice < 1 || choice > player->currentDungeon->creatureCount) {
        printf("ERROR!!: Invalid creature number. Please try again.\n");
        return;
    }
    // Get the chosen creature
    Creature *targetCreature = &player->currentDungeon->creatures[choice - 1];

    // Let player choose weapon
    int hasWeapon = 0;
    printf("\nChoose a weapon to attack with:\n");
    for (int i = 0; i < player->inventoryCapacity; i++) {
        if (player->inventory[i].template != NULL && player->inventory[i].template->type == WEAPON) {
            printf("%d. %s (Attack: %d)\n", i+1, player->inventory[i].template->name, player->inventory[i].template->damage);
            hasWeapon = 1;
        }
    }

    if (!hasWeapon) {
        printf("You have no weapons! Using bare hands (1 damage)\n");
        int playerDamage = 1;
        targetCreature->health -= playerDamage;
        printf("You attacked %s and dealt %d damage!\n", targetCreature->name, playerDamage);
    } else {
        int weaponChoice;
        printf("Choose weapon number (0 to cancel): ");
        scanf("%d", &weaponChoice);

        if (weaponChoice == 0) {
            printf("Attack cancelled!\n");
            return;
        }

        if (weaponChoice < 1 || 
            weaponChoice > player->inventoryCapacity ||
            player->inventory[weaponChoice-1].template == NULL ||
            player->inventory[weaponChoice-1].template->type != WEAPON) {
            printf("Invalid weapon choice!\n");
            return;
        }

        int playerDamage = player->inventory[weaponChoice-1].template->damage;
        targetCreature->health -= playerDamage;
        printf("You attacked %s with %s and dealt %d damage!\n",
               targetCreature->name,
               player->inventory[weaponChoice-1].template->name,
               playerDamage);
    }

    // Helper Function to calculate player defense
    int calculatePlayerDefense(Player *player) {
        int totalDefense = 0;
        for (int i = 0; i < player->inventoryCapacity; i++) {
            if (player->inventory[i].template != NULL && player->inventory[i].template->type == ARMOR) {
                totalDefense += player->inventory[i].template->defense;
            }
        }
        return totalDefense;
    }

    // Creature counter-attacks if still alive
    if (targetCreature->health > 0) {
        int playerDefense = calculatePlayerDefense(player);
        int creatureDamage = targetCreature->strength;
        float damageReduction = (float)playerDefense / 100.0f; // Convert to percentage
        int reducedDamage = creatureDamage * (1.0f - damageReduction);
        
        reducedDamage = (reducedDamage < 1) ? 1 : reducedDamage; // Minimum 1 damage

        player->health -= reducedDamage;
        printf("%s attacked and dealt %d damage to you! (Reduced from %d by your armor)\n", 
               targetCreature->name, reducedDamage, creatureDamage);

        // Check if player died
        if (player->health <= 0) {
            displayDeathScreen(player);
        }

    } else {
        printf("%s has been defeated!\n", targetCreature->name);
        
        // Remove defeated creature
        free(targetCreature->name);
        for (int i = choice - 1; i < player->currentDungeon->creatureCount - 1; i++) {
            player->currentDungeon->creatures[i] = player->currentDungeon->creatures[i + 1];
        }
        player->currentDungeon->creatureCount--;

        // Update dungeon creature state
        if (player->currentDungeon->creatureCount == 0) {
            dungeonCreatureStates[player->currentDungeon->code] = 0;
        } else {
            dungeonCreatureStates[player->currentDungeon->code] = player->currentDungeon->creatureCount;
        }
    }
}

void displayDeathScreen(Player *player) {
    printf("\n\n");
    printf("********************************\n");
    printf("*          GAME OVER           *\n");
    printf("********************************\n\n");
    printf("   %s's journey has ended...\n\n", player->nickName);
    printf("   Fallen in: %s\n\n", player->currentDungeon->name);
    printf("   Final Stats:\n");
    printf("   ============\n");
    printf("   Health: 0\n");
    printf("   Strength: %d\n\n", player->strength);
    printf("   Your legend will be remembered\n");
    printf("   in the halls of fallen heroes\n\n");
    printf("********************************\n");
    printf("Press ENTER to exit the game...\n");
    getchar();
    getchar(); // Consume the newline and wait for key press
    exit(0);
}

void pickup(Player *player) {
    if (player->currentDungeon->itemCount == 0) {
        printf("There are no items in this dungeon.\n");
        return;
    }
    printf("Available items to pickup:\n");
    for (int i = 0; i < player->currentDungeon->itemCount; i++) {
        printf("%d. - %s: %s\n", i+1, player->currentDungeon->items[i].template->name, player->currentDungeon->items[i].template->description);
    }

    int choice;
    printf("Enter the number of the item you want to pickup (0 to cancel): ");
    scanf("%d", &choice);

    if (choice == 0) {
        printf("Item pickup cancelled!\n");
        return;
    }

    if (choice < 1 || choice > player->currentDungeon->itemCount) {
        printf("ERROR!!: Invalid item number. Please try again.\n");
        return;
    }

    int emptySlot = -1; // For finding empty slot
    for (int i = 0; i < player->inventoryCapacity; i++) {
        if (player->inventory[i].template == NULL) {
            emptySlot = i;
            break;
        }
    }

    if (emptySlot == -1) {
        printf("ERROR!!: Your inventory is full. Cannot pickup item!\n");
        return;
    }

    int itemIndex = choice - 1;
    player->inventory[emptySlot] = player->currentDungeon->items[itemIndex];

    for (int i = itemIndex; i < player->currentDungeon->itemCount - 1; i++) {
        player->currentDungeon->items[i] = player->currentDungeon->items[i + 1];
    }

    player->currentDungeon->itemCount--;
    printf("You picked up %s!\n",player->inventory[emptySlot].template->name);
    printf("\n");
}

void cleanupGame(Player *player, Dungeon **dungeon) {
    if (*dungeon != NULL) {
        Dungeon *visitedDungeon[100] = {NULL}; // For avoid double free 
        int visitedCount = 0;

        void cleanupDungeonRecursive(Dungeon *dungeon) { // Helper Function
            if (dungeon == NULL) return;
            for (int i = 0; i < visitedCount; i++) {
                if (visitedDungeon[i] == dungeon) return;
            }
            visitedDungeon[visitedCount++] = dungeon;
            cleanupDungeonRecursive(dungeon->up);
            cleanupDungeonRecursive(dungeon->down);
            cleanupDungeonRecursive(dungeon->left);
            cleanupDungeonRecursive(dungeon->right);
            cleanupDungeon(dungeon);
        }
        cleanupDungeonRecursive(*dungeon);
        *dungeon = NULL;
    }
    cleanupPlayer(player);
}

int getCreatureTypeForDungeon(int dungeonCode) {
    switch (dungeonCode) {
        case 1: return 0;  // Dead Valley - Zombies
        case 2: return 1;  // Route of King's Palace
        case 3: return -1; // King's Palace
        case 4: return 2;  // Spider Nest
        case 5: return 3;  // Skeleton King
        case 6: return 1;  // Exit of Forest
        case 7: return 0;  // Road to Dragon's Lair
        case 8: return 2;  // Path of Ronin
        case 9: return -1; // Hall of Blades
        case 10: return 1; // Crimson Abyss
        case 11: return 4; // Dragon's Lair
        case 12: return 0; // Zombie Cave
        case 13: return 2; // Dead End
        case 14: return 5; // Demon of Abyss
        default: return -1;
    }
}

Dungeon *createConnectedDungeon(Dungeon *templateDungeon) {
    if (!templateDungeon) return NULL;
    int code = templateDungeon->code;

    Item *newItems = NULL;
    if (templateDungeon->itemCount > 0) {
        newItems = (Item *)malloc(templateDungeon->itemCount * sizeof(Item));
        if (newItems == NULL) {
            printf("ERROR!!: Memory allocation failed for items.\n");
            return NULL;
        }
        for (int i = 0; i < templateDungeon->itemCount; i++) {
            newItems[i] = templateDungeon->items[i];
        }
    }
        

    Dungeon *newDungeon = createDungeon(
        templateDungeon->name, 
        code, 
        templateDungeon->itemCount, 
        templateDungeon->creatureCount, 
        newItems, 
        NULL
    );
    if (newDungeon == NULL) {
        if (newItems) free(newItems);
        return NULL;
    }
    // Check saved state
    if (dungeonCreatureStates[code] != -1) {
        if (dungeonCreatureStates[code] == 0) {
            newDungeon->creatureCount = 0;
            newDungeon->creatures = NULL;
        } else {
            generateCreatures(&newDungeon, getCreatureTypeForDungeon(code));
            newDungeon->creatureCount = dungeonCreatureStates[code];
        }
    } else {
        generateCreatures(&newDungeon, getCreatureTypeForDungeon(code));
    }
    return newDungeon;
}

void restoreConnections(Dungeon *currentDungeon) {
    extern int dungeonCreatureStates[16];  // Global array'e erişim

    Dungeon *tempDungeon[16];
    for (int i = 0; i < 16; i++) {
        tempDungeon[i] = NULL;
    }
    // Create Temp Dungeons with their respective items
    const ItemTemplate *deadValleyTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int deadValleyCounts[] = {1};
    Item *itemsDeadValley = createItems(deadValleyTemplates, deadValleyCounts, 1);

    const ItemTemplate *kingsPalaceTemplates[] = {
        &ITEM_TEMPLATES[8], // King's Amulet
    };
    const int kingsPalaceCounts[] = {1};
    Item *itemsKingsPalace = createItems(kingsPalaceTemplates, kingsPalaceCounts, 1);

    const ItemTemplate *spiderNestTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
        &ITEM_TEMPLATES[3], // Big Healing Potion
        &ITEM_TEMPLATES[7], // Torch of the Abyss
    };
    const int spiderNestCounts[] = {1, 1, 1};
    Item *itemsSpiderNest = createItems(spiderNestTemplates, spiderNestCounts, 3);

    const ItemTemplate *skeletonKingTemplates[] = {
        &ITEM_TEMPLATES[4], // Key of King's Palace
    };
    const int skeletonKingCounts[] = {1};
    Item *itemsSkeletonKing = createItems(skeletonKingTemplates, skeletonKingCounts, 1);

    const ItemTemplate *pathOfTheRoninTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int pathOfTheRoninCounts[] = {1};
    Item *itemsPathOfTheRonin = createItems(pathOfTheRoninTemplates, pathOfTheRoninCounts, 1);

    const ItemTemplate *hallOfBladesTemplates[] = {
        &ITEM_TEMPLATES[6], // Katana
    };
    const int hallOfBladesCounts[] = {1};
    Item *itemsHallOfBlades = createItems(hallOfBladesTemplates, hallOfBladesCounts, 1);

    const ItemTemplate *crimsonAbyssTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int crimsonAbyssCounts[] = {1, 1};
    Item *itemsCrimsonAbyss = createItems(crimsonAbyssTemplates, crimsonAbyssCounts, 2);

    const ItemTemplate *mountOfAbyssTemplates[] = {
        &ITEM_TEMPLATES[5], // Dragon's Scale
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int mountOfAbyssCounts[] = {1, 1};
    Item *itemsMountOfAbyss = createItems(mountOfAbyssTemplates, mountOfAbyssCounts, 2);

    const ItemTemplate *zombieCaveTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int zombieCaveCounts[] = {1};
    Item *itemsZombieCave = createItems(zombieCaveTemplates, zombieCaveCounts, 1);

    const ItemTemplate *topOfTheMountTemplates[] = {
        &ITEM_TEMPLATES[2], // Healing Potion
    };
    const int topOfTheMountCounts[] = {1};
    Item *itemsTopOfTheMountOfAbyss = createItems(topOfTheMountTemplates, topOfTheMountCounts, 1);

    // Create temp dungeons with their items
    tempDungeon[0] = createDungeon("Beginning of The Road", 0, 0, 0, NULL, NULL);
    tempDungeon[1] = createDungeon("Dead Valley", 1, 1, 0, itemsDeadValley, NULL);
    tempDungeon[2] = createDungeon("Route of the King's Palace", 2, 0, 0, NULL, NULL);
    tempDungeon[3] = createDungeon("The King's Palace", 3, 1, 0, itemsKingsPalace, NULL);
    tempDungeon[4] = createDungeon("Crooked Forest: Spider Nest", 4, 3, 0, itemsSpiderNest, NULL);
    tempDungeon[5] = createDungeon("Crooked Forest: Skeleton King", 5, 1, 0, itemsSkeletonKing, NULL);
    tempDungeon[6] = createDungeon("Exit of the Crooked Forest", 6, 0, 0, NULL, NULL);
    tempDungeon[7] = createDungeon("Road to the Dragon's Lair", 7, 0, 0, NULL, NULL);
    tempDungeon[8] = createDungeon("Path of the Ronin", 8, 1, 0, itemsPathOfTheRonin, NULL);
    tempDungeon[9] = createDungeon("Hall of Blades", 9, 1, 0, itemsHallOfBlades, NULL);
    tempDungeon[10] = createDungeon("Crimson Abyss", 10, 2, 0, itemsCrimsonAbyss, NULL);
    tempDungeon[11] = createDungeon("Entrance to Mount of Abyss", 11, 2, 0, itemsMountOfAbyss, NULL);
    tempDungeon[12] = createDungeon("Mount of Abyss: Zombie Cave", 12, 1, 0, itemsZombieCave, NULL);
    tempDungeon[13] = createDungeon("Mount of Abyss: Dead End", 13, 0, 0, NULL, NULL);
    tempDungeon[14] = createDungeon("Top of the Mount of Abyss", 14, 1, 0, itemsTopOfTheMountOfAbyss, NULL);
    tempDungeon[15] = createDungeon("Escape Dungeon", 15, 0, 0, NULL, NULL);

    // Connect Temp Dungeons
    connectDungeons(tempDungeon[0], tempDungeon[1], "right");
    connectDungeons(tempDungeon[1], tempDungeon[2], "right");
    connectDungeons(tempDungeon[1], tempDungeon[4], "down");
    connectDungeons(tempDungeon[2], tempDungeon[1], "left");
    connectDungeons(tempDungeon[2], tempDungeon[5], "down");
    connectDungeons(tempDungeon[2], tempDungeon[3], "right");
    connectDungeons(tempDungeon[3], tempDungeon[2], "left");
    connectDungeons(tempDungeon[4], tempDungeon[1], "up");
    connectDungeons(tempDungeon[4], tempDungeon[5], "right");
    connectDungeons(tempDungeon[4], tempDungeon[7], "down");
    connectDungeons(tempDungeon[4], tempDungeon[6], "left");
    connectDungeons(tempDungeon[5], tempDungeon[4], "left");
    connectDungeons(tempDungeon[5], tempDungeon[2], "up");
    connectDungeons(tempDungeon[6], tempDungeon[4], "right");
    connectDungeons(tempDungeon[6], tempDungeon[8], "down");
    connectDungeons(tempDungeon[7], tempDungeon[4], "up");
    connectDungeons(tempDungeon[7], tempDungeon[8], "left");
    connectDungeons(tempDungeon[7], tempDungeon[10], "down");
    connectDungeons(tempDungeon[7], tempDungeon[11], "right");
    connectDungeons(tempDungeon[8], tempDungeon[6], "up");
    connectDungeons(tempDungeon[8], tempDungeon[7], "right");
    connectDungeons(tempDungeon[8], tempDungeon[9], "down");
    connectDungeons(tempDungeon[9], tempDungeon[8], "up");
    connectDungeons(tempDungeon[10], tempDungeon[7], "up");
    connectDungeons(tempDungeon[11], tempDungeon[7], "left");
    connectDungeons(tempDungeon[11], tempDungeon[12], "right");
    connectDungeons(tempDungeon[12], tempDungeon[13], "up");
    connectDungeons(tempDungeon[12], tempDungeon[14], "down");
    connectDungeons(tempDungeon[13], tempDungeon[12], "down");
    connectDungeons(tempDungeon[14], tempDungeon[12], "up");
    connectDungeons(tempDungeon[14], tempDungeon[15], "right");

    // Connect current dungeon to temp dungeons
    int currentCode = currentDungeon->code;
    if (currentCode >= 0 && currentCode < 16) {
        currentDungeon->up = NULL;
        currentDungeon->down = NULL;
        currentDungeon->left = NULL;
        currentDungeon->right = NULL;

        // Create Connections
        if (tempDungeon[currentCode]->up) {
            currentDungeon->up = createConnectedDungeon(tempDungeon[currentCode]->up);
        }
        if (tempDungeon[currentCode]->down) {
            currentDungeon->down = createConnectedDungeon(tempDungeon[currentCode]->down);
        }
        if (tempDungeon[currentCode]->left) {
            currentDungeon->left = createConnectedDungeon(tempDungeon[currentCode]->left);
        }
        if (tempDungeon[currentCode]->right) {
            currentDungeon->right = createConnectedDungeon(tempDungeon[currentCode]->right);
        }
    }

    // Free Temp Items
    if (itemsDeadValley) free(itemsDeadValley);
    if (itemsKingsPalace) free(itemsKingsPalace);
    if (itemsSpiderNest) free(itemsSpiderNest);
    if (itemsSkeletonKing) free(itemsSkeletonKing);
    if (itemsPathOfTheRonin) free(itemsPathOfTheRonin);
    if (itemsHallOfBlades) free(itemsHallOfBlades);
    if (itemsCrimsonAbyss) free(itemsCrimsonAbyss);
    if (itemsMountOfAbyss) free(itemsMountOfAbyss);
    if (itemsZombieCave) free(itemsZombieCave);
    if (itemsTopOfTheMountOfAbyss) free(itemsTopOfTheMountOfAbyss);
    // Free Temp Dungeons
    for (int i = 0; i < 16; i++) {
        if (tempDungeon[i] != NULL) {
            if (tempDungeon[i]->name != NULL) {
                free(tempDungeon[i]->name);
            }
            if (tempDungeon[i]->items != NULL) {
                free(tempDungeon[i]->items);
            }
            if (tempDungeon[i]->creatures != NULL) {
                for (int j = 0; j < tempDungeon[i]->creatureCount; j++) {
                    if (tempDungeon[i]->creatures[j].name != NULL) {
                    free(tempDungeon[i]->creatures[j].name);
                }
            }
            free(tempDungeon[i]->creatures);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    Player player = {NULL, 0, 0, 0, NULL, NULL};
    Dungeon *dungeon = NULL;
    char commandName[100];
    menu(&player, &dungeon);

    while (1) {
        printf("Enter a command (type 'menu' to see the commands): ");
        scanf("%s", commandName);
        if (strcmp(commandName, "exit") == 0) {
            printf("Exiting game...\n");
            cleanupGame(&player, &dungeon);
            break;
        }
        commands(commandName, &player, &dungeon);
    }
    cleanupGame(&player, &dungeon);
    return 0;
}
