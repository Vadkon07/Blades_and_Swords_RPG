#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "game.h"


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
int enemiesKilled = 0;
bool relationships = false;

Character characters[] = {
	{"Alice", "Wife", 50},
	{"Bob", "Friend", 70},
	{"Eve", "Enemy", -50},
	{"Charlie", "Mentor", 80},
	{"Diana", "Girlfriend", 60},
	{"Frank", "Rival", -30}
};


Quest quests[] = {
	{"Defeat 5 enemies", 10, 25, false},
	{"Defeat 10 enemies", 20, 50, false},
	{"Find a rare gem", 50, 100, false}
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
	printf("---> Enemies killed: %d\n", enemiesKilled);
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

	enemiesKilled +=1;

	if (hp <= 0) {
		printf("Your hero %s was killed. THE END.\n", heroName);
		theEnd();
	}

	checkQuestCompletion();
	waitForUser();
}

void printWithAnimation(const char* text) {
	for (int i = 0; text[i] != '\0'; i++) {
		printf("%c", text[i]);
		fflush(stdout); 
		usleep(100000); 
	}
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
	getchar(); 
	switch (askLocation) {
		case '1':
			strcpy(location, "Village House");
            printf("You decided to sleep and restore your HP.\n");

			char sleepAnimation[] = "Z . . .\nZz . . .\nZzz . . .\n";
			printWithAnimation(sleepAnimation);

			hp = 100;
			days += 1;
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

void interactWithCharacter() {
    int choice;
    printf("------ Interact with Characters ------\n");
    for (int i = 0; i < characterCount; i++) {
        printf("%d. %s (%s): Affection Level %d\n", i + 1, characters[i].name, characters[i].type, characters[i].affectionLevel);
    }
    printf("Choose a character to interact with (Enter the number): ");
    scanf("%d", &choice);
    getchar(); 

    if (choice > 0 && choice <= characterCount) {
        char interaction;
        printf("Interacting with %s. Choose action: (F)ight, (T)alk, (G)ift (SOON): ", characters[choice - 1].name);
        interaction = getchar();
        getchar(); 

        switch (toupper(interaction)) {
            case 'F':
                printf("You chose to fight with %s!\n", characters[choice - 1].name);
                characters[choice - 1].affectionLevel -= 20;
                printf("%s's affection level is now %d.\n", characters[choice - 1].name, characters[choice - 1].affectionLevel);
                break;
            case 'T':
                printf("You have a friendly conversation with %s.\n", characters[choice - 1].name);
                characters[choice - 1].affectionLevel += 10;
                printf("%s's affection level is now %d.\n", characters[choice - 1].name, characters[choice - 1].affectionLevel);
                break;
            case 'G':
		int askAboutGift;

		printf("Gift costs 20 coins, are you sure? (Y/N)");

		askAboutGift = getchar();
		getchar();

		switch (toupper(askAboutGift)) {

			case 'Y':
				printf("You give a gift to %s.\n", characters[choice - 1].name);
				printf("%s: Oh, thank you so much for this gift!\n", characters[choice - 1].name);
				characters[choice - 1].affectionLevel += 20;
				printf("%s's affection level is now %d.\n", characters[choice - 1].name, characters[choice - 1].affectionLevel);
				balance -= 20;
				break;


			case 'N':
				printf("No gifts today");
			default:
				printf("You decided not to gift anything to %s.", characters[choice - 1].name);
	}

            default:
                printf("Invalid action. Try again.\n");
                break;
        }
    } else {
        printf("Invalid choice. Returning to main menu.\n");
    }

    waitForUser();
}

void displayAbout() {
	printf("--- About Blades and Swords RPG ---\n");
	printf("Blades and Swords RPG is a text-based adventure game set in a medieval fantasy world. You take on the role of a hero who explores different locations, completes quests, battles enemies, and manages relationships with various characters.\n");
	printf("The game is set in a vast open world that includes diverse terrains such as mountains, lakes, villages, and more. Your decisions and actions will affect the story, relationships with other characters, and your hero's growth.\n");
	printf("As you progress, you will face challenges, make choices that impact the narrative, and experience the consequences of your actions. Build your strength, forge alliances, and seek to become the most renowned hero in the realm.\n");
	printf("More features and expansions are planned, including new locations, enemies, and story arcs.\n");
	printf("This video game is Open Source, and we would be very happy if you help us with its development!\n");
	waitForUser();
}

void displayHelp() {
	printf("--- Help and Guide ---\n");
	printf("1. Use the main menu to navigate through different sections of the game.\n");
	printf("2. Earn coins by working in various locations or defeating enemies. Use coins to buy items or services like healing at the canteen.\n");
	printf("3. Manage your hero's health carefully. Rest in safe places or use potions to recover lost HP.\n");
	printf("4. Build relationships with different characters in the game. Your interactions can turn them into friends, allies, or enemies.\n");
	printf("5. Complete quests to earn experience points (XP) and level up. Some quests have specific requirements or hidden rewards.\n");
	printf("6. Type the corresponding number of the menu option to select it. Use 'Enter' to confirm your choice.\n");
	printf("7. Always save your progress frequently to avoid losing your achievements.\n");
	printf("8. Check your inventory regularly to manage items effectively and use them when needed.\n");
	printf("9. Explore different locations to uncover secrets and new quests. Each location may have unique opportunities or dangers.\n");
	printf("For more detailed information, refer to the game's user manual or the in-game tutorial.\n");
	waitForUser();
}

void addQuest(const char *description, int xp, int coins) {
	if (questCount < 10) {
		strcpy(quests[questCount].description, description);
		quests[questCount].xp = xp;
		quests[questCount].coins = coins;
		quests[questCount].isCompleted = false;
		questCount++;
	} else {
		printf("Maximum number of quests reached.\n");
	}
}

void checkQuestCompletion() {
	for (int i = 0; i < questCount; i++) {
		if (!quests[i].isCompleted) {
			if (strcmp(quests[i].description, "Defeat 5 enemies") == 0 && enemiesKilled >= 5) {
				quests[i].isCompleted = true;
				xp += quests[i].xp;
				balance += quests[i].coins;
				printf("Quest completed: %s! You received %d xp and %d coins.\n", quests[i].description, quests[i].xp, quests[1].coins);
			}
			if (strcmp(quests[i].description, "Defeat 10 enemies") == 0 && enemiesKilled >= 10) {
				quests[i].isCompleted = true;
				xp += quests[i].xp;					
				balance += quests[i].coins;
				printf("Quest completed: %s! You received %d coins.\n", quests[i].description, quests[i].coins);
			}
		}
	}
}

void saveGame() {
	char filename[100];
	printf("Enter a name for your save file: ");
	fgets(filename, sizeof(filename), stdin);
	filename[strcspn(filename, "\n")] = 0; 

	strcat(filename, ".dat");
	FILE *file = fopen(filename, "wb");
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
	printf("Game saved successfully as '%s'!\n", filename);
	waitForUser();
}

void loadGame() {
	char filename[100];
	printf("Enter the name of the save file to load: ");
	fgets(filename, sizeof(filename), stdin);
	filename[strcspn(filename, "\n")] = 0; 

	strcat(filename, ".dat");
	FILE *file = fopen(filename, "rb");
	if (!file) {
		printf("Failed to load game. Check if the file exists.\n");
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
	printf("Game loaded successfully from '%s'!\n", filename);
	waitForUser();
}

void newGame() {
		int chooseClass;
		printf("Enter the name of your hero: ");
		fgets(heroName, sizeof(heroName), stdin);
		heroName[strcspn(heroName, "\n")] = 0; 
		
		printf("Enter the surname of your hero: ");
		fgets(heroSurname, sizeof(heroSurname), stdin);
		heroSurname[strcspn(heroSurname, "\n")] = 0; 

		printf("Choose the class of your hero:\n");
		printf("1. Knight\n");
		printf("2. Orc\n");
		printf("3. Elf\n");
		printf("4. Alchemist\n");
		printf("Enter your choice: ");
		scanf("%d", &chooseClass);

		if (chooseClass == 1) {
			strcpy(heroClass, "Knight");
		}

		if (chooseClass == 2) {
			strcpy(heroClass, "Orc");
		}

		if (chooseClass == 3) {
			strcpy(heroClass, "Elf");
		}

		if (chooseClass == 4) {
			strcpy(heroClass, "Alchemist");
		}

		if (chooseClass == 666) {
			strcpy(heroClass, "Devil"); //Easter egg ;)
		}



		else {
			printf("Error");
		}
		
		clearScreen();


		srand(time(0));
		int random_number = (rand() % 5) + 1;
		//printf("TEST: %d\n", random_number); //for debugm and will be deleted soon, but before we have to add +-10 different stories

		if (random_number == 1) {
			printf("Your character was born in a poor family, you don't have anything and have to start from zero.\n\n");
		}

		if (random_number == 2) {
			printf("Your character was born in a decent family, so he has 5 coins in his pocket.\n\n");
			balance += 5;
		}

		if (random_number == 3) {
			printf("Your character was born in a very rich family, and he has 50 coins!\n\n");
			balance += 50;
		}

		if (random_number == 4) {
			printf("Your character was born in Mountains!\n\n");
			strcpy(location, "Mountains");
			printf("You are now in the Mountains. Watch out for enemies!\n");
		}

		if (random_number == 5) {
			printf("Your charecter was for a little sick some time ago, so for now he has only 20 hp!\n\n");
			hp = 20;


		} else {
			printf(" "); //It will print it every time, even if it's not else. Solve it pls someone
		}

		waitForUser();
		mainMenu();
}

void logo() {
	printf("\n ___ _         _                       _   ___                   _    ");
	printf("\n| _ ) |__ _ __| |___ ___  __ _ _ _  __| | / __|_ __ _____ _ _ __| |___");
 	printf("\n| _ \ / _` / _` / -_|_-< / _` | ' \/ _` | \__ \ V  V / _ \ '_/ _` (_-<)");
 	printf("\n|___/_\__,_\__,_\___/__/ \__,_|_||_\__,_| |___/\_/\_/\___/_| \__,_/__/\n\n");
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
	printf("3. Kill enemies\n");
	printf("4. Change location\n");
	printf("5. Manage relationships\n");
	printf("6. Interact with characters\n");
	printf("7. Save Game\n");
	printf("8. Load Game\n");
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
			interactWithCharacter();
			break;
		case '7':
			saveGame();
			break;
		case '8':
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

	logo();
	waitForUser();

	clearScreen();

	printf("1. Start New Game\n");
	printf("2. Load Saved Game\n");
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
