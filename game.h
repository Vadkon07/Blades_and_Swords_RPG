#ifndef GAME_H
#define GAME_H





typedef struct {
    char name[20];
    char type[20]; // e.g., wife, friend, girlfriend, enemy
    int affectionLevel; // Could range from -100 to 100
} Character;

typedef struct {
    char description[100];
    int reward; // XP or coins
    bool isCompleted;
} Quest;

typedef struct {
    char name[20];
    char type[20]; // e.g., weapon, potion
    int value;
} Item;

int questCount = 2;
int characterCount = 3;
Item inventory[10];
int inventoryCount = 0;

// Prototypes

void theEnd();
void clearScreen();
void waitForUser();
void canteen();
void decreaseCoints(int amount);
void increaseHp(int amount);
void heroInfo();
void heroInventory();
void addItemToInventory(const char *name, const char *type, int value);
void useItem(int index);
void listInventory();
void killEnemies();
void changeLocation();
void manageRelationships();
void displayAbout();
void displayHelp();
void addQuest(const char *description, int reward);
void checkQuestCompletion();
void interactWithCharacter();
void mainMenu();
void saveGame();
void loadGame();

#endif 
