#include "GameHandler.h"

GameHandler::GameHandler()
{
}

GameHandler::~GameHandler()
{
	// remove player
	delete player;
	player = nullptr;
	// remove all chests
	for (size_t i(0); i < chests.size(); ++i)
	{
		delete chests[i];
		chests[i] = nullptr;
	}
	// remove all enemies
	for (size_t i(0); i < enemies.size(); ++i)
	{
		delete enemies[i];
		enemies[i] = nullptr;
	}
}

char GameHandler::GameCycle()
{
	// print welcome massage and instructions
	Init();
	// load desired configuration and create all objects
	LoadConfig();
	// after everything is loaded, display current state of the map
	DisplayMap();
	// all the activities that happen until user presses ESC key
	MainGameLoop();

	// check if the player wants another game
	char anotherGame;
	cout << endl << "Would you like another game? (y/n)" << endl;
	cin >> anotherGame;
	return anotherGame;
}

void GameHandler::Init()
{
	// print welcome massage and give instructions
	cout << endl << "====================" << endl << "WELCOME TO THE GAME!" << endl << "====================" << endl;
	cout << "Legend: G - grass, X - player, E - enemy, C - chest, L - lake, M - mountain" << endl;
	cout << "Controls: SPACE - open the chest, WASD/arrow keys - movement,  V - view current state of the map" << endl
		 << "C - choose package of items, H - drink health potion, M - eat magic mushroom, Q - suicide" << endl;

	// initially, it's player's turn
	turn = Turn_PLAYER;
}


void GameHandler::LoadConfig()
{
	Json::Value root;
	// number of configuration wanted
	int cfgNum;
	// path to config file
	string cfgPath;
	bool loadingOK = false;
	ifstream file;

	// while file isn't successfully opened, prompt user to input an existing configuration number
	while (!loadingOK)
	{
		cout << endl << "Choose a configuration by inputing an integer representing the number of configuration!" << endl;
		cin >> cfgNum;
		cfgPath = "../Configs/cfg" + to_string(cfgNum) + ".json";
		file.open(cfgPath, ifstream::in);
		if (file.is_open())
			loadingOK = true;
	}
	
	// load file content into Json::Value root
	file >> root;
	// get size for the map
	int mapSize = root["size"].asUInt();

	// set map to be squared
	map.resize(mapSize);
	for (size_t i(0); i < mapSize; ++i)
		map[i].resize(mapSize);

	// put grass everywhere
	for (size_t row(0); row < map.size(); ++row)
		for (size_t col(0); col < map[row].size(); ++col)
			map[row][col] = 'g';

	// fill map with mountains, lakes and trees
	vector<string> nature = { "mountains", "lakes", "trees" };
	for (auto object : nature)
		FillMapWithNature(root, object);

	// put chests, enemies and player to their place in the map and create them
	FillMapWithChests(root, "chests");
	FillMapWithEnemies(root, "enemies");
	InitPlayer(root, "player");

	file.close();
}

void GameHandler::MainGameLoop()
{
	// constantly check what the user inputs, until user presses ESC key, or the player dies from suicide or enemy kills him
	int ch;
	combatResult = Result_CONTINUE;
	while (((ch = GetKey()) != Key_ESC) and (player->GetLifeStatus() == Status_ALIVE) and (ch != Key_Q) and (ch != Key_q) and (combatResult == Result_CONTINUE))
	{
		HandleKey(ch, turn);
	}

	// if suicide
	if ((ch == Key_Q) or (ch == Key_q))
		player->SetLifeStatus(Status_DEAD);

	// if died regularly
	if (player->GetLifeStatus() == Status_DEAD)
		cout << endl << "Sorry, your hero is dead and you should feel bad :(" << endl;
}

void GameHandler::FillMapWithNature(const Json::Value& root, const string& object)
{
	// check if there are any objects wanted
	Json::Value objects = root[object];
	if (!objects.empty())
	{
		int startRow;
		int startCol;
		int width;
		int height;
		string objectNo;

		// if there are, take starting index and dimensions of each of them
		for (size_t i(0); i < objects.size(); ++i)
		{
			objectNo = to_string(i);
			startRow = objects[objectNo]["startRow"].asUInt();
			startCol = objects[objectNo]["startCol"].asUInt();
			width = objects[objectNo]["width"].asUInt();
			height = objects[objectNo]["height"].asUInt();

			// fill map with objects
			for (size_t j(startRow); j < (startRow + height); ++j)
				for (size_t k(startCol); k < (startCol + width); ++k)
					map[j][k] = toupper(object[0]);
		}
	}
}

void GameHandler::FillMapWithChests(const Json::Value& root, const string& object)
{
	// check if there are any objects of chest type
	Json::Value objects = root[object];
	if (!objects.empty())
	{
		// location for the chest
		int row;
		int col;
		// damage for the weapon, armor points for the armor, health points for the health potion and damage boost for magic mushroom
		int damage;
		int armor;
		int health;
		int boost;
		string objectNo;
		// take location and content of each of them
		for (size_t i(0); i < objects.size(); ++i)
		{
			objectNo = to_string(i);
			row = objects[objectNo]["row"].asUInt();
			col = objects[objectNo]["col"].asUInt();
			damage = objects[objectNo]["damage"].asUInt();
			armor = objects[objectNo]["armor"].asUInt();
			health = objects[objectNo]["health"].asUInt();
			boost = objects[objectNo]["boost"].asUInt();

			// fill map with chests
			map[row][col] = 'C';

			// add current chest to the vector of chests
			chests.push_back(new Chest(row, col, damage, armor, health, boost));
		}
	}
}

void GameHandler::FillMapWithEnemies(const Json::Value& root, const string& object)
{
	// check if there are any objects of enemy type
	Json::Value objects = root[object];
	if (!objects.empty())
	{
		// location for the enemies
		int row;
		int col;
		// health of the enemy, damage for enemy's weapon and armor points for enemy's armor
		int health;
		int damage;
		int armor;
		string objectNo;
		// take location health of the enemy and properties of his item
		for (size_t i(0); i < objects.size(); ++i)
		{
			objectNo = to_string(i);
			row = objects[objectNo]["row"].asUInt();
			col = objects[objectNo]["col"].asUInt();
			health = objects[objectNo]["health"].asInt();
			damage = objects[objectNo]["item"]["damage"].asUInt();
			armor = objects[objectNo]["item"]["armor"].asUInt();

			// fill map with enemies
			map[row][col] = 'E';

			// add current enemy to the vector of enemies
			enemies.push_back(new Enemy(row, col, health, damage, armor));
		}
	}
}

void GameHandler::InitPlayer(const Json::Value& root, const string& object)
{
	// check if there are any objects of player type
	Json::Value objects = root[object];
	if (!objects.empty())
	{
		// location and health of the player
		int row;
		int col;
		int health;
		// take location and health
		row = objects["row"].asUInt();
		col = objects["col"].asUInt();
		health = objects["health"].asInt();

		// fill map with player
		map[row][col] = 'X';

		// create player
		player = new Player(row, col, health);
	}
}

void GameHandler::DisplayMap()
{
	cout << endl;
	// go through every map element and display it on the screen
	for (size_t i(0); i < map.size(); ++i)
	{
		for (size_t j(0); j < map[i].size(); ++j)
			cout << map[i][j];

		// after each row, go to new line
		cout << endl;
	}
	cout << endl;
}

int GameHandler::GetKey()
{
	// get user input from keyboard
	int ch = getch();
	if (ch == 0 || ch == 224)
		ch = 256 + getch();

	return ch;
}

void GameHandler::HandleKey(int ch, Turn& turn)
{
	// handle input from keyboard only if it's the player's turn
	if (turn == Turn_PLAYER)
	{
		switch (ch)
		{
		// go up
		case Key_W:
		case Key_w:
		case Key_ARROW_UP:
			MovementDecision(-1, 0);
			break;

		// go down
		case Key_S:
		case Key_s:
		case Key_ARROW_DOWN:
			MovementDecision(1, 0);
			break;

		// go left
		case Key_A:
		case Key_a:
		case Key_ARROW_LEFT:
			MovementDecision(0, -1);
			break;

		// go right
		case Key_D:
		case Key_d:
		case Key_ARROW_RIGHT:
			MovementDecision(0, 1);
			break;

		// view map
		case Key_V:
		case Key_v:
			UpdateMap();
			DisplayMap();
			break;

		// choose set of items from inventory
		case Key_C:
		case Key_c:
			player->Equip();
			break;

		// loot chest if it exists
		case Key_SPACE:
			if (CheckForChests())
				PickItemsFromChest();
			else
				cout << endl << "There's no chest here!" << endl;
			break;

		// heal if possible
		case Key_H:
		case Key_h:
			player->DrinkHealthPotion();
			break;

		// boost damage if possible
		case Key_M:
		case Key_m:
			player->EatMagicMushroom();
			break;
		}
	}

	// don't handle key input if it's not player's turn
	else if (turn == Turn_ENEMY)
		cout << endl << "Wait! It is the enemy's turn!" << endl;

	// turn == Turn_COMBAT
	else
		cout << endl << "Please, do not interrupt the combat!" << endl;
}

void GameHandler::UpdateMap()
{
	// leave mountains, lakes and trees where they are
	for (size_t i(0); i < map.size(); ++i)
		for (size_t j(0); j < map[i].size(); ++j)
			if ((map[i][j] != 'M') and (map[i][j] != 'T') and (map[i][j] != 'L'))
				map[i][j] = 'g';


	// TODO: chests also don't change location!
	// check every chest location and put it in the map
	int row, col;
	for (auto chest : chests)
	{
		row = chest->location.row;
		col = chest->location.col;
		map[row][col] = 'C';
	}

	// check every enemy location and put them accordingly
	for (auto enemy : enemies)
	{
		row = enemy->GetLocation().row;
		col = enemy->GetLocation().col;
		map[row][col] = 'E';
	}

	// check player's location and put it in the map
	row = player->GetLocation().row;
	col = player->GetLocation().col;
	map[row][col] = 'X';
}

bool GameHandler::CheckForChests()
{
	Chest* chest;
	// for every chest, check if it is around player
	for (size_t i(0); i < chests.size(); ++i)
	{
		chest = chests[i];
		if (((chest->location.row == player->GetLocation().row + 1) and (chest->location.col == player->GetLocation().col)) or
			((chest->location.row == player->GetLocation().row - 1) and (chest->location.col == player->GetLocation().col)) or
			((chest->location.row == player->GetLocation().row) and (chest->location.col == player->GetLocation().col + 1)) or
			((chest->location.row == player->GetLocation().row) and (chest->location.col == player->GetLocation().col - 1)))
		{
			// which chest is located
			chestIdx = i;
			return true;
		}
	}
	return false;
}

void GameHandler::PickItemsFromChest()
{
	// player's inventory has as many rows as there are different types of items (4 in this case)
	if (player->GetInventorySize() != NUM_OF_ITEMS)
		player->ResizeInventory();

	// if located chest was empty, do nothing
	Chest* chest = chests[chestIdx];
	if (player->FillInventory(chest->items[Row_WEAPON]->GetValue(), chest->items[Row_ARMOR]->GetValue(),
		chest->items[Row_HEALTH_POTION]->GetValue(), chest->items[Row_MAGIC_MUSHROOM]->GetValue()) == Chest_WAS_EMPTY)
	{
		turn = Turn_PLAYER;
	}
	// if not, content of the chest is moved to player's inventory and chest is emptied
	else
	{
		chest->CleanItems();

		// show current inventory
		player->ViewCurrentInventory();

		// after this event, it's enemy's turn
		turn = Turn_ENEMY;
		ActivateEnemy();
	}	
}

void GameHandler::MovementDecision(int addRow, int addCol)
{
	// calculate the potential new location
	int newLocationRow = player->GetLocation().row + addRow;
	int newLocationCol = player->GetLocation().col + addCol;

	// if the location is valid, set it
	if (CheckNewLocation(newLocationRow, newLocationCol, map))
	{
		player->SetLocation(newLocationRow, newLocationCol);
		UpdateMap();
		if (CheckForEnemies())
			combatResult = Combat();
	}
	// else notify the user the location isn't available
	else
		cout << "Can't go there!" << endl;

	return;
}

void GameHandler::ActivateEnemy()
{
	// notify the user
	cout << endl << "It's the enemy's turn now!" << endl;
	// move every enemy for some pseudo random amount
	for (auto enemy : enemies)
		enemy->Move(map);

	// when the move is finished, it's player's turn
	turn = Turn_PLAYER;
	// update map because of all the movement
	UpdateMap();
	DisplayMap();
	cout << endl << "It's player's turn again!" << endl;
}

bool GameHandler::CheckForEnemies()
{
	Enemy* enemy;
	// for every enemy, check if it around player
	for (size_t i(0); i < enemies.size(); ++i)
	{
		enemy = enemies[i];
		if (((enemy->GetLocation().row == player->GetLocation().row + 1) and (enemy->GetLocation().col == player->GetLocation().col)) or
			((enemy->GetLocation().row == player->GetLocation().row - 1) and (enemy->GetLocation().col == player->GetLocation().col)) or
			((enemy->GetLocation().row == player->GetLocation().row) and (enemy->GetLocation().col == player->GetLocation().col + 1)) or
			((enemy->GetLocation().row == player->GetLocation().row) and (enemy->GetLocation().col == player->GetLocation().col - 1)))
		{
			// remember which enemy is next to the player
			enemyIdx = i;
			return true;
		}
	}
	return false;
}

CombatResult GameHandler::Combat()
{
	// nobody's turn
	turn = Turn_COMBAT;
	cout << endl << "Combat has begun!" << endl;

	// if inventory is empty, player dies immediately
	if (player->IsInventoryEmpty())
	{
		player->SetLifeStatus(Status_DEAD);
		cout << "The enemy has won because you had no weapon!" << endl;
		return Result_LOST;
	}

	// inventory wasn't empty, so it's safe to equip player
	player->Equip();
	bool playerAlive = true;
	bool enemyAlive = true;
	int playerHealthLeft;
	int enemyHealthLeft;
	// enemy that is located in CheckForEnemies()
	Enemy* enemy = enemies[enemyIdx];
	// fight till death
	while (playerAlive and enemyAlive)
	{
		cout << "Enemy hits" << endl;
		// damage is decreased for the amount of armor points
		playerHealthLeft = player->ReceiveDamage(enemy->GetDamage() - player->GetArmorPoints());
		cout << endl << "Player has " << playerHealthLeft << " health points left!" << endl;
		// player is dead, game over
		if (playerHealthLeft == 0)
		{
			player->SetLifeStatus(Status_DEAD);
			playerAlive = false;
			turn = Turn_PLAYER;
			return Result_LOST;
		}

		cout << endl << "Player hits" << endl;
		// damage is decreased for the amount of armor points
		enemyHealthLeft = enemy->ReceiveDamage(player->GetDamage() - enemy->GetArmorPoints());
		cout << endl << "Enemy has " << enemyHealthLeft << " health points left!" << endl;
		if (enemyHealthLeft == 0)
		{
			// enemy dead
			enemy->SetLifeStatus(Status_DEAD);
			player->FillInventory(enemy->GetDamage(), enemy->GetArmorPoints(), 0, 0);
			map[enemy->GetLocation().row][enemy->GetLocation().col] = 'g';
			enemyAlive = false;
			turn = Turn_PLAYER;
			// send dead enemy to the end of the vector and remove it
			swap(enemies[enemyIdx], enemies[enemies.size() - 1]);
			enemies.pop_back();
			cout << "Great! One enemy down, " << enemies.size() << " more to go!" << endl;
			// if there are no more enemies, player has won
			if (enemies.empty())
			{
				cout << endl << "There are no more enemies! Congratulations, you have won!" << endl;
				return Result_WON;
			}
			// if there are still more enemies, continue playing
			else
				return Result_CONTINUE;
		}
	}
	return Result_CONTINUE;
}