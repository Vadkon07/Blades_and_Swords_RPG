#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"


/* CURRENT TASKS:
 *
 * 1. DESCRIBE WHAT IS THIS RPG AND WHERE IS IT ALL HAPPENS, ADD HELP AND GUIDE COMMANDS.
 * 2. MAKE CLEAR SCREEN COMMANDS NORMALY, AND IF SOMEONE FINISHED TASK - ENTER TO GO TO MAIN MENU
 * 3. ADD OTHER CHARACTERS AND SYSTEM OF RELATIONSHIPS WITH THEM (WIFE, FRIEND, GIRLFRIEND, ENEMY ETC.)
 * 4. IN LOADGAME ADD POSSIBILITY TO NAME SAVES DIFFERENTLY AND CREATE A MENU TO CHOOSE WHICH ONE OF THEM WE HAVE TO OPEN
 */

int hp = 100;
int lvl = 1;
int xp = 0;
int reqXp = 50;
char location[20] = "Home";
int balance = 0;
char heroName[20] = "NONE";
char heroSurname[20] = "NONE";
char heroClass[20] = "NONE";
int days = 0;
bool relationships = false;

Character characters[] = {
	{"Alice","Wife", 50},
	{"Bob","Friend", 70},
	{"Eve","Enemy", -50}
};

Quest quests[] = {
	{"Defeat 5 enemies", 50, false},
	{"Find a rare gem", 100, false}
};

void theEnd() {
	printf("Thank you for playing Blades and Swords RPG!\n");
	exit(0);
}

void clearScreen(){
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void waitForUser(){
	printf("Press Enter to continue...\n");
	getchar();
}

void canteen(){
	int choiceCanteen;
	printf("Welcome to the canteen! \n\n");
	printf("--- MENU --- \n");
	printf("1. Bread - 5 coins - 5 hp\n");
	choiceCanteen = getchar();

	switch(choiceCanteen) {
		case '1':
			balance = balance - 5;
			hp = hp + 5;
		default:
			waitForUser();

	}
}

void decreaseCoins(int amount){
	balance -= amount;
}

void increaseHp(int amount) {
	hp += amount;
	if (hp > 100) {
		hp = 100;
	}
}

void heroInfo() {
	printf("----------+ HERO INFO +----------\n\n");
	printf("---> Name: %s\n", heroName);
	printf("---> Surname: %s\n", heroSurname);
	printf("---> Class: %s\n", heroClass);
	printf("---> HP: %d\n", hp);
	printf("---> LVL: %d\n", lvl);
	printf("---> XP: %d / %d\n", xp, reqXp);
	printf("---> Current location: %s\n", location);
	printf("---> Balance: %d coins\n", balance);
	printf("---> Days survived: %d\n", days);
	printf("---> Relationships: %s\n", relationships ? "Yes" : "No");
	printf("---------------------------------\n\n");
	waitForUser();
}

void heroInventory() {
	printf("------ Your Inventory ------\n");
	listInventory();
	waitForUser();
}

void addItemToInventory(const char *name, const char *type, int value) {
	if (inventoryCount < 10) {
		strcpy(inventory[inventoryCount].name, name);
		strcpy(inventory[inventoryCount].type, type);
		inventory[inventoryCount].value = value;
		inventoryCount++;
		printf("%s added to your inventory.\n", name);
	} else {
		printf("Inventory full! Cannot add more items. Buy a bigger bag to be able to care with yourself more items\n");
	}
	waitForUser();
}

void useItem(int index) {
	if (index < 0 || index >= inventoryCount) {
		printf("Invalid inventory index.\n");
		return;
	}

	Item item = inventory[index];
	if (strcmp(item.type, "potion") == 0) {
		increaseHp(item.value);
		printf("You used %s and restored %d HP.\n", item.name, item.value);
	} else if (strcmp(item.type, "weapon") == 0) {
		printf("You equipped %s, increasing your attack power!\n", item.name);
	}
	// Remove the item from inventory
	for (int i = index; i < inventoryCount - 1; i++) {
		inventory[i] = inventory[i + 1];
	}
	inventoryCount--;
	waitForUser();
}

void listInventory() {
	if (inventoryCount == 0) {
		printf("Inventory is empty.\n");
	} else {
		for (int i = 0; i < inventoryCount; i++) {
			printf("%d. %s (%s) - Value: %d\n", i + 1, inventory[i].name, inventory[i].type, inventory[i].value);
		}
	}
}

void killEnemies() {
	int damage = rand() % 8 + 1;
	int coins = rand() % 6 + 3;
	hp -= damage;
	balance += coins;
	xp += 10;

	printf("You killed some enemies and earned %d coins, but took %d damage. +10 XP\n", coins, damage);

	if (hp <= 0) {
		printf("Your hero %s was killed. THE END.\n", heroName);
		theEnd();
	}

	checkQuestCompletion();
	waitForUser();
}

void changeLocation() {
	int askLocation;
	printf("Where would you like to go (current location is (%s))?\n", location);
	printf("1. Village House (Sleep)\n");
	printf("2. Mountains\n");
	printf("3. Work\n");
	printf("4. Lake\n");
	printf("5. Shop\n");
	printf("6. Canteen\n");
	printf("Enter your choice: ");

	askLocation = getchar();
	getchar(); // Consume newline character
	switch (askLocation) {
		case '1':
			strcpy(location, "Village House");
			printf("You decided to sleep and restore your HP.\n");
			hp = 100;
			break;
		case '2':
			strcpy(location, "Mountains");
			printf("You are now in the Mountains. Watch out for enemies!\n");
			break;
		case '3':
			strcpy(location, "Work");
			int income = rand() % 6 + 1;
			balance += income;
			printf("You worked as a miner and earned %d coins. Current balance: %d coins\n", income, balance);
			break;
		case '4':
			strcpy(location, "Lake");
			printf("You are at the Lake. You feel relaxed. +5 HP\n");
			increaseHp(5);
			break;
		case '5':
			strcpy(location, "Shop");
			printf("Welcome to the Shop! (Coming soon)\n");
			break;
		case '6':
			if (balance > 0) {
				canteen();
			} else {
				printf("You need more coins to visit the canteen.\n");
			}
			break;
		default:
			printf("Invalid choice. Returning to main menu.\n");
	}

	waitForUser();
}

void manageRelationships() {
	printf("------ Character Relationships ------\n");
	for (int i = 0; i < characterCount; i++) {
		printf("%s (%s): Affection Level %d\n", characters[i].name, characters[i].type, characters[i].affectionLevel);
	}
	waitForUser();
}

void displayAbout() {
	printf("--- About Blades and Swords RPG ---\n");
	printf("This is a text-based RPG game where you explore different locations, fight enemies, complete, quest, and manage relationships.\n");
	printf("More features will be added soon!\n");
	printf("This videogame is Open Source, and we will be very happy if you will help us with development of her!\n");
	waitForUser();
}

void displayHelp() {
	printf("--- Help and Guide ---\n");
	printf("1. Use the main menu to navigate through the game.\n");
	printf("2. Earn coins by working and killing enemies.\n");
	printf("3. Manage your health and relationships to survive.\n");
	printf("4. Type the number of the menu option to select it.\n");
	waitForUser();
}

void addQuest(const char *description, int reward) {
	if (questCount < 10) {
		strcpy(quests[questCount].description, description);
		quests[questCount].reward = reward;
		quests[questCount].isCompleted = false;
		questCount++;
	} else {
		printf("Maximum number of quests reached.\n");
	}
}

void checkQuestCompletion() {
	for (int i = 0; i < questCount; i++) {
		if (!quests[i].isCompleted) {
			if (strcmp(quests[i].description, "Defeat 5 enemies") == 0 && xp >= 50) {
				quests[i].isCompleted = true;
				balance += quests[i].reward;
				printf("Quest completed: %s! You received %d coins.\n", quests[i].description, quests[i].reward);
			}
		}
	}
}

void saveGame() {
	FILE *file = fopen("savegame.dat", "wb");
	if (!file) {
		printf("Failed to save game.\n");
		return;
	}

	fwrite(&hp, sizeof(int), 1, file);
	fwrite(&lvl, sizeof(int), 1, file);
	fwrite(&xp, sizeof(int), 1, file);
	fwrite(&reqXp, sizeof(int), 1, file);
	fwrite(location, sizeof(char), 20, file);
	fwrite(&balance, sizeof(int), 1, file);
	fwrite(heroName, sizeof(char), 20, file);
	fwrite(heroSurname, sizeof(char), 20, file);
	fwrite(heroClass, sizeof(char), 20, file);
	fwrite(&days, sizeof(int), 1, file);
	fwrite(&relationships, sizeof(bool), 1, file);
	fwrite(&inventoryCount, sizeof(int), 1, file);
	fwrite(inventory, sizeof(Item), inventoryCount, file);
	fwrite(&questCount, sizeof(int), 1, file);
	fwrite(quests, sizeof(Quest), questCount, file);

	fclose(file);
	printf("Game saved successfully!\n");
	waitForUser();
}

void loadGame() {
	FILE *file = fopen("savegame.dat", "rb");
	if (!file) {
		printf("Failed to load game.\n");
		return;
	}

	fread(&hp, sizeof(int), 1, file);
	fread(&lvl, sizeof(int), 1, file);
	fread(&xp, sizeof(int), 1, file);
	fread(&reqXp, sizeof(int), 1, file);
	fread(location, sizeof(char), 20, file);
	fread(&balance, sizeof(int), 1, file);
	fread(heroName, sizeof(char), 20, file);
	fread(heroSurname, sizeof(char), 20, file);
	fread(heroClass, sizeof(char), 20, file);
	fread(&days, sizeof(int), 1, file);
	fread(&relationships, sizeof(bool), 1, file);
	fread(&inventoryCount, sizeof(int), 1, file);
	fread(inventory, sizeof(Item), inventoryCount, file);
	fread(&questCount, sizeof(int), 1, file);
	fread(quests, sizeof(Quest), questCount, file);

	fclose(file);
	printf("Game loaded successfully!\n");
	waitForUser();
}

void newGame() {
		printf("Enter the name of your hero: ");
		fgets(heroName, sizeof(heroName), stdin);
		heroName[strcspn(heroName, "\n")] = 0; 
		
		printf("Enter the surname of your hero: ");
		fgets(heroSurname, sizeof(heroSurname), stdin);
		heroSurname[strcspn(heroSurname, "\n")] = 0; 

		printf("Enter the class of your hero: ");
		fgets(heroClass, sizeof(heroClass), stdin);
		heroClass[strcspn(heroClass, "\n")] = 0; 
		
		clearScreen();
		mainMenu();
}
		



void mainMenu() {


	clearScreen();


	char ask;

	if (xp >= reqXp) {
		xp = 0;
		lvl++;
		reqXp += 50;
		printf("NOTIFICATION: Level UP! Your level has increased to %d \n\n", lvl);

	}

	printf("--- Welcome to the Blades and Swords RPG! ---\n\n");
	printf("1. Hero info\n");
	printf("2. Inventory\n");
	printf("3. Work\n");
	printf("4. Change location\n");
	printf("5. Manage Relationships\n");
	printf("6. Save Game\n");
	printf("7. Load Game\n");
	printf("9. About RPG\n");
	printf("H. Help\n");
	printf("0. Exit\n\n");
	printf("Enter your choice: ");

	ask = getchar();
	getchar(); 
	switch (ask) {
		case '1':
			heroInfo();
			break;
		case '2':
			heroInventory();
			break;
		case '3':
			killEnemies();
			break;
		case '4':
			changeLocation();
			break;
		case '5':
			manageRelationships();
			break;
		case '6':
			saveGame();
			break;
		case '7':
			loadGame();
			break;
		case '9':
			displayAbout();
			break;
		case 'H':
		case 'h':
			displayHelp();
			break;
		case '0':
			printf("Goodbye!\n");
			exit(0);
		default:
			printf("Invalid choice, try again.\n");
	}

	mainMenu(); 
}

int main() {
	char newOrSave;

	clearScreen();

	printf("1. Start New Game\n");
	printf("(SOON) 2. Load Saved Game\n");
	printf("Enter your choice: ");
	newOrSave = getchar();
	getchar();
	switch (newOrSave) {
		case '1':
			newGame();
			break;
		case '2':
			loadGame();
			mainMenu();
			break;
		default:
			printf("Invalid choice, try again. Please, check that you actually have a saved games. If not - instead start a new game\n");
	}
}

