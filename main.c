#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "game.h"
#include <gtk/gtk.h>

void clear_window(GtkWidget *container);
void printWithAnimation(const char* text);
void changeLocation();
void manageRelationships();
void interactWithCharacter();
void displayAbout();
void displayHelp();
void saveGame(GtkWidget *widget, gpointer data);
void loadGame(GtkWidget *widget, gpointer data);
void check_level_up(GtkWidget *widget);
void mainMenu(GtkWidget *widget);
void show_combat_message(GtkWidget *message_area, const char *message);
void killEnemies(GtkWidget *widget, gpointer message_area);
void clearScreen();
void waitForUser();
void show_main_menu();
void heroInfo();
void heroInventory();
void close_window();

GtkWidget *grid;

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
	printf("Thank you for playing Moonshadow RPG!\n");
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
	//getchar();
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

void heroInfo(gpointer data, gpointer message_area) {
    GtkTextView *text_view = GTK_TEXT_VIEW(data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_area));
    gtk_text_buffer_set_text(buffer, "", -1); // Clear previous text

    char message[512];

    snprintf(message, sizeof(message), 
        "----------+ HERO INFO +----------\n\n"
        "---> Name: %s\n"
        "---> Surname: %s\n"
        "---> Class: %s\n"
        "---> HP: %d\n"
        "---> LVL: %d\n"
        "---> XP: %d / %d\n"
        "---> Current location: %s\n"
        "---> Balance: %d coins\n"
        "---> Days survived: %d\n"
        "---> Enemies killed: %d\n"
        "---> Relationships: %s\n"
        "---------------------------------\n\n",
        heroName, heroSurname, heroClass, hp, lvl, xp, reqXp, location, balance, days, enemiesKilled, relationships ? "Yes" : "No");

if (hp < 0) { snprintf(message + strlen(message), sizeof(message) - strlen(message), "Hero is died. You have to start a new game or load a save!\n"); }

    gtk_text_buffer_insert_at_cursor(buffer, message, -1);
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

void show_combat_message(GtkWidget *message_area, const char *message) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_area));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, message, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(message_area), &end, 0.0, FALSE, 0.0, 1.0);
}

void killEnemies(GtkWidget *widget, gpointer message_area) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_area));
    gtk_text_buffer_set_text(buffer, "", -1); // Clear the text area at the start

    const int numEnemies = 10;
    struct Enemy {
        char *name;
        int hpEnemy;
        char *attackPhrases[3];
    };

    struct Enemy enemies[10];

    enemies[0] = (struct Enemy){"Goblin", rand() % 91 + 10, {"Goblin swings its club!", "Goblin bites fiercely!", "Goblin throws a rock!"}};
    enemies[1] = (struct Enemy){"Orc", rand() % 91 + 10, {"Orc slashes with its axe!", "Orc roars and charges!", "Orc throws a spear!"}};
    enemies[2] = (struct Enemy){"Troll", rand() % 91 + 10, {"Troll smashes with a hammer!", "Troll stomps the ground!", "Troll hurls a boulder!"}};
    enemies[3] = (struct Enemy){"Dragon", rand() % 91 + 10, {"Dragon breathes fire!", "Dragon swipes with its claws!", "Dragon bites with sharp teeth!"}};
    enemies[4] = (struct Enemy){"Vampire", rand() % 91 + 10, {"Vampire bites your neck!", "Vampire uses dark magic!", "Vampire slashes with its claws!"}};
    enemies[5] = (struct Enemy){"Werewolf", rand() % 91 + 10, {"Werewolf claws at you!", "Werewolf bites ferociously!", "Werewolf pounces on you!"}};
    enemies[6] = (struct Enemy){"Necromancer", rand() % 91 + 10, {"Necromancer casts a spell!", "Necromancer summons undead!", "Necromancer shoots dark energy!"}};
    enemies[7] = (struct Enemy){"Bandit", rand() % 91 + 10, {"Bandit swings a sword!", "Bandit shoots an arrow!", "Bandit throws a dagger!"}};
    enemies[8] = (struct Enemy){"Giant", rand() % 91 + 10, {"Giant crushes you with a fist!", "Giant stomps!", "Giant throws a rock!"}};
    enemies[9] = (struct Enemy){"Assassin", rand() % 91 + 10, {"Assassin strikes with a dagger!", "Assassin throws a smoke bomb!", "Assassin shoots a poison dart!"}};

    int enemyIndex = rand() % numEnemies;
    int playerAttack, enemyAttack, coins;
    char message[256];

    sprintf(message, "You encounter a %s! His HP is: %d", enemies[enemyIndex].name, enemies[enemyIndex].hpEnemy);
    show_combat_message(message_area, message);

    while (enemies[enemyIndex].hpEnemy > 0 && hp > 0) {
        playerAttack = rand() % 20 + 5;
        enemies[enemyIndex].hpEnemy -= playerAttack;
        sprintf(message, "You attack the %s for %d damage!", enemies[enemyIndex].name, playerAttack);
        show_combat_message(message_area, message);

        if (enemies[enemyIndex].hpEnemy <= 0) {
            coins = rand() % 6 + 3;
            balance += coins;
            xp += 10;
            enemiesKilled += 1;
            sprintf(message, "You killed the %s and earned %d coins. +10 XP", enemies[enemyIndex].name, coins);
            show_combat_message(message_area, message);
            break;
        }

        enemyAttack = rand() % 15 + 5;
        hp -= enemyAttack;
        sprintf(message, "%s %s", enemies[enemyIndex].name, enemies[enemyIndex].attackPhrases[rand() % 3]);
        show_combat_message(message_area, message);
        sprintf(message, "You took %d damage. HP: %d", enemyAttack, hp);
        show_combat_message(message_area, message);

        if (hp <= 0) {
            sprintf(message, "Your hero %s was killed by the %s. THE END.", heroName, enemies[enemyIndex].name);
	    show_combat_message(message_area, message);
            return;
        }
    }
}

void printWithAnimation(const char* text) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        usleep(100000); // 100 milliseconds
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

void saveGame(GtkWidget *widget, gpointer data) {
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

void loadGame(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(widget),
                                         action,
                                         ("_Cancel"),
                                         GTK_RESPONSE_CANCEL,
                                         ("_Open"),
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        FILE *file = fopen(filename, "rb");
        if (!file) {
            printf("Failed to load game. Check if the file exists.\n");
            g_free(filename);
            gtk_widget_destroy(dialog);
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
        g_free(filename);
    }

    gtk_widget_destroy(dialog);

    mainMenu(NULL);
}

GtkWidget *entry_name;
GtkWidget *entry_surname;
GtkWidget *combo_class;
GtkWidget *text_view;

void on_new_game_confirm(GtkWidget *widget, gpointer data) {
    const char *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
    const char *surname = gtk_entry_get_text(GTK_ENTRY(entry_surname));
    int chooseClass = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_class)) + 1;

    strcpy(heroName, name);
    strcpy(heroSurname, surname);

    switch (chooseClass) {
        case 1: strcpy(heroClass, "Knight"); break;
        case 2: strcpy(heroClass, "Orc"); break;
        case 3: strcpy(heroClass, "Elf"); break;
        case 4: strcpy(heroClass, "Alchemist"); break;
	case 5: strcpy(heroClass, "Samurai"); break;
	case 6: strcpy(heroClass, "Monk"); break;
	case 7: strcpy(heroClass, "Hunter"); break;
	case 8: strcpy(heroClass, "Warrior"); break;
        default: g_print("Error, choose a hero class!\n"); return;
    }

    clearScreen();

    srand(time(0));
    int random_number = (rand() % 10) + 1;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, "", -1); // Clear previous text

    char story[256];
    switch (random_number) {
        case 1:
            snprintf(story, sizeof(story), "Your character was born in a poor family, you don't have anything and have to start from zero.\n\n");
            break;
        case 2:
            snprintf(story, sizeof(story), "Your character was born in a decent family, so he has 5 coins in his pocket.\n\n");
            balance += 5;
            break;
        case 3:
            snprintf(story, sizeof(story), "Your character was born in a very rich family, and he has 50 coins!\n\n");
            balance += 50;
            break;
        case 4:
            snprintf(story, sizeof(story), "Your character was born in Mountains!\n\n");
            strcpy(location, "Mountains");
            snprintf(story + strlen(story), sizeof(story) - strlen(story), "You are now in the Mountains. Watch out for enemies!\n");
            break;
        case 5:
            snprintf(story, sizeof(story), "Your character was a little sick some time ago, so for now he has only 20 hp!\n\n");
            hp = 20;
            break;
        case 6:
            snprintf(story, sizeof(story), "Your character did some bad things, and just some days ago he left a prison. It looks like he earned some coins while there!\n\n");
            balance += 5;
            break;
        default:
            snprintf(story, sizeof(story), "Welcome to your adventure!\n\n");
            break;
    }

    gtk_text_buffer_insert_at_cursor(buffer, story, -1);

    waitForUser();
    mainMenu(widget);
}

void newGame(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *content_area, *grid, *label_name, *label_surname, *label_class, *confirm_button;

    dialog = gtk_dialog_new_with_buttons("New Game", GTK_WINDOW(widget), GTK_DIALOG_MODAL, NULL, NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    label_name = gtk_label_new("Enter the name of your hero:");
    gtk_grid_attach(GTK_GRID(grid), label_name, 0, 0, 1, 1);
    entry_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry_name, 1, 0, 1, 1);

    label_surname = gtk_label_new("Enter the surname of your hero:");
    gtk_grid_attach(GTK_GRID(grid), label_surname, 0, 1, 1, 1);
    entry_surname = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry_surname, 1, 1, 1, 1);

    label_class = gtk_label_new("Choose the class of your hero:");
    gtk_grid_attach(GTK_GRID(grid), label_class, 0, 2, 1, 1);
    combo_class = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Knight");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Orc");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Elf");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Alchemist");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Samurai");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Monk");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Hunter");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_class), "Warrior");

    gtk_grid_attach(GTK_GRID(grid), combo_class, 1, 2, 1, 1);

    confirm_button = gtk_button_new_with_label("Confirm");
    g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_new_game_confirm), dialog);
    g_signal_connect(confirm_button, "clicked", G_CALLBACK(close_window), dialog);
    gtk_grid_attach(GTK_GRID(grid), confirm_button, 1, 3, 1, 1);

    gtk_widget_show_all(dialog);
}

void show_notification(GtkWidget *widget, const char *message, const char *title) {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(GTK_WINDOW(widget),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,
			"%s", message);
	gtk_window_set_title(GTK_WINDOW(dialog), title);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void check_level_up(GtkWidget *widget) {
	while (xp >= reqXp) {
		xp -= reqXp;
		lvl++;
		reqXp += 50;
		show_notification(widget, "NOTIFICATION: Level UP! Your level has increased.", "Level Up!");
	}
} // DOESN'T WORK, AS A TEMPORARY SOLUTION CREATE A MANUAL BUTTON TO CHECK IT

void mainMenu(GtkWidget *widget) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *hero_info_button;
    GtkWidget *inventory_button;
    GtkWidget *kill_enemies_button;
    GtkWidget *change_location_button;
    GtkWidget *manage_relationships_button;
    GtkWidget *interact_characters_button;
    GtkWidget *save_game_button;
    GtkWidget *load_game_button;
    GtkWidget *about_rpg_button;
    GtkWidget *help_button;
    GtkWidget *exit_button;
    GtkWidget *message_area;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Moonshadow");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    message_area = gtk_text_view_new();
    gtk_grid_attach(GTK_GRID(grid), message_area, 2, 0, 1, 12);

    hero_info_button = gtk_button_new_with_label("Hero info");
    gtk_grid_attach(GTK_GRID(grid), hero_info_button, 0, 0, 1, 1);
    g_signal_connect(hero_info_button, "clicked", G_CALLBACK(heroInfo), message_area);

    inventory_button = gtk_button_new_with_label("Inventory (NO)");
    gtk_grid_attach(GTK_GRID(grid), inventory_button, 0, 1, 1, 1);
    // g_signal_connect(inventory_button, "clicked", G_CALLBACK(heroInventory), NULL);

    kill_enemies_button = gtk_button_new_with_label("Kill enemies");
    gtk_grid_attach(GTK_GRID(grid), kill_enemies_button, 0, 2, 1, 1);
    g_signal_connect(kill_enemies_button, "clicked", G_CALLBACK(killEnemies), message_area);

    change_location_button = gtk_button_new_with_label("Change location (NO)");
    gtk_grid_attach(GTK_GRID(grid), change_location_button, 0, 3, 1, 1);
    // g_signal_connect(change_location_button, "clicked", G_CALLBACK(changeLocation), NULL);

    manage_relationships_button = gtk_button_new_with_label("Manage relationships (NO)");
    gtk_grid_attach(GTK_GRID(grid), manage_relationships_button, 0, 4, 1, 1);
    // g_signal_connect(manage_relationships_button, "clicked", G_CALLBACK(manageRelationships), NULL);

    interact_characters_button = gtk_button_new_with_label("Interact with characters (NO)");
    gtk_grid_attach(GTK_GRID(grid), interact_characters_button, 0, 5, 1, 1);
    // g_signal_connect(interact_characters_button, "clicked", G_CALLBACK(interactWithCharacter), NULL);

    save_game_button = gtk_button_new_with_label("Save Game (NO)");
    gtk_grid_attach(GTK_GRID(grid), save_game_button, 0, 6, 1, 1);
    g_signal_connect(save_game_button, "clicked", G_CALLBACK(saveGame), NULL);

    load_game_button = gtk_button_new_with_label("Load Game (NO)");
    gtk_grid_attach(GTK_GRID(grid), load_game_button, 0, 7, 1, 1);
    g_signal_connect(load_game_button, "clicked", G_CALLBACK(loadGame), NULL);

    about_rpg_button = gtk_button_new_with_label("About RPG (NO)");
    gtk_grid_attach(GTK_GRID(grid), about_rpg_button, 0, 8, 1, 1);
    // g_signal_connect(about_rpg_button, "clicked", G_CALLBACK(displayAbout), NULL);

    help_button = gtk_button_new_with_label("Help (NO)");
    gtk_grid_attach(GTK_GRID(grid), help_button, 0, 9, 1, 1);
    // g_signal_connect(help_button, "clicked", G_CALLBACK(displayHelp), NULL);

    exit_button = gtk_button_new_with_label("Exit");
    gtk_grid_attach(GTK_GRID(grid), exit_button, 0, 10, 1, 1);
    g_signal_connect(exit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
}

void clear_window(GtkWidget *container) {
    GList *children, *iter;

    children = gtk_container_get_children(GTK_CONTAINER(container));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_container_remove(GTK_CONTAINER(container), GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

void close_window(GtkWidget *widget, gpointer window) {
	gtk_widget_destroy(GTK_WIDGET(window));
}

void exit_application(GtkWidget *widget, gpointer data) {
	gtk_main_quit(); 
}

int main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *new_game_button;
	GtkWidget *load_game_button;
	GtkWidget *exit_button;
	
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Moonshadow");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 150);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(window), grid);

	new_game_button = gtk_button_new_with_label("Start New Game");
	load_game_button = gtk_button_new_with_label("Load Saved Game");
	exit_button = gtk_button_new_with_label("Exit");

	gtk_grid_attach(GTK_GRID(grid), new_game_button, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), load_game_button, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), exit_button, 0, 2, 1, 1);
	 
	g_signal_connect(new_game_button, "clicked", G_CALLBACK(newGame), NULL);
	g_signal_connect(load_game_button, "clicked", G_CALLBACK(loadGame), NULL);
	g_signal_connect(exit_button, "clicked", G_CALLBACK(exit_application), NULL);

	//g_signal_connect(new_game_button, "clicked", G_CALLBACK(close_window), grid);
	//g_signal_connect(load_game_button, "clicked", G_CALLBACK(close_window), grid);
	 
	gtk_widget_show_all(window);
	
	gtk_main();
	
	return 0;
}
