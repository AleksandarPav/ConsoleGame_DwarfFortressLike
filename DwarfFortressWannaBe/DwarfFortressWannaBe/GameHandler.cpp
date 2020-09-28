#include "GameHandler.h"

GameHandler::GameHandler()
{
}

GameHandler::~GameHandler()
{
	// remove player
	player = nullptr;

	// remove all chests
	for (size_t i(0); i < chests.size(); ++i)
	{
		chests[i]->~Chest();
		delete chests[i];
		chests[i] = nullptr;
	}

	// remove all enemies
	for (size_t i(0); i < enemies.size(); ++i)
	{
		enemies[i]->~Enemy();
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
	// all the activities that happen until the game isn't over
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
	LegendAndControls();

	// initially, it's player's turn
	turn = Turn_PLAYER;
}


void GameHandler::LoadConfig()
{
	Json::Value root;
	// path to config file
	string cfgPath;
	// indicator that loading was done without problems
	bool loadingOK = false;
	ifstream file;

	// while file isn't successfully opened, prompt user to input an existing configuration number
	while (!loadingOK)
	{
		cout << endl << "Choose a configuration by inputing an integer representing the number of configuration!" << endl;
		cin >> activeConfig;
		cfgPath = "../Configs/cfg" + to_string(activeConfig) + ".json";
		file.open(cfgPath, ifstream::in);
		if (file.is_open())
			loadingOK = true;
	}
	
	// load file content into Json::Value root
	file >> root;

	// if vector of enemies was left not empty for any reason, clear it and destroy each enemy in vector
	if (!(enemies.empty()))
	{
		for (auto enemy : enemies)
			enemy->~Enemy();
		enemies.clear();
	}

	// if vector of chests was left not empty for any reason, clear it and destroy each chest in vector
	if (!chests.empty())
	{
		for (auto chest : chests)
			chest->~Chest();
		chests.clear();
	}

	// get size for the map
	int mapSize = root["size"].asInt();

	// set map to be squared
	map.resize(mapSize);
	for (size_t i(0); i < mapSize; ++i)
		map[i].resize(mapSize);

	// put grass everywhere
	for (size_t row(0); row < map.size(); ++row)
		for (size_t col(0); col < map[row].size(); ++col)
			//map[row][col] = 'g';
			map[row][col] = ',';

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
	// constantly check what the user inputs, until user presses ESC key, or the player dies from suicide (Q pressed) or enemy kills him
	int ch;
	combatResult = Result_CONTINUE;
	while (((ch = GetKey()) != Key_ESC) and (player->GetLifeStatus() == Status_ALIVE) and (ch != Key_Q) and (ch != Key_q) and (combatResult == Result_CONTINUE))
	{
		HandleKey(ch, turn);
	}

	// if suicide
	if ((ch == Key_Q) or (ch == Key_q))
		player->SetLifeStatus(Status_DEAD);

	// if died by enemy
	if (player->GetLifeStatus() == Status_DEAD)
		cout << endl << "Sorry, your hero is dead and you should feel bad :(" << endl;
}

void GameHandler::FillMapWithNature(const Json::Value& root, const string& object)
{
	// check if there are any objects wanted (mountains, lakes or trees, depending what's passed as argument)
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
			startRow = objects[objectNo]["startRow"].asInt();
			startCol = objects[objectNo]["startCol"].asInt();
			width = objects[objectNo]["width"].asInt();
			height = objects[objectNo]["height"].asInt();

			// fill map with objects
			for (size_t j(startRow); j < (startRow + height); ++j)
				for (size_t k(startCol); k < (startCol + width); ++k)
				{
					// fill map with appropriate ASCII symbols
					map[j][k] = MapSymbol(toupper(object[0]));
				}
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
			row = objects[objectNo]["row"].asInt();
			col = objects[objectNo]["col"].asInt();
			damage = objects[objectNo]["damage"].asInt();
			armor = objects[objectNo]["armor"].asInt();
			health = objects[objectNo]["health"].asInt();
			boost = objects[objectNo]["boost"].asInt();

			// fill map with chests
			map[row][col] = '#';

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
		// take location and health of the enemy and properties of his item
		for (size_t i(0); i < objects.size(); ++i)
		{
			objectNo = to_string(i);
			row = objects[objectNo]["row"].asInt();
			col = objects[objectNo]["col"].asInt();
			health = objects[objectNo]["health"].asInt();
			damage = objects[objectNo]["item"]["damage"].asInt();
			armor = objects[objectNo]["item"]["armor"].asInt();

			// fill map with enemies
			map[row][col] = '&';

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
		int row;
		int col;
		int health;
		string packageNo;
		// take location and health
		row = objects["row"].asInt();
		col = objects["col"].asInt();
		health = objects["health"].asInt();

		// fill map with player
		map[row][col] = 'X';

		// create player
		player = unique_ptr<Player>(new Player(row, col, health));

		// if there are any items in inventory listed in cfg file, fill the inventory
		if (!objects["inventory"].empty())
		{
			// each package takes one column in player's inventory matrix, while each row is reserved for specific type of item
			for (size_t i(0); i < objects["inventory"].size(); ++i)
			{
				// set inventory from empty matrix to have 4 rows (one for each type of item)
				player->ResizeInventory();
				// one set of items is set in columns
				packageNo = to_string(i);
				// damage is put in the first, armor points in the second, health potion in the third and magic mushroom in the fourth row
				player->FillInventory(objects["inventory"][packageNo]["damage"].asInt(), objects["inventory"][packageNo]["armor"].asInt(),
					objects["inventory"][packageNo]["health_potion"].asInt(), objects["inventory"][packageNo]["magic_mushroom"].asInt());
			}
		}
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

		// show legend and controls on screen
		case Key_K:
		case Key_k:
			LegendAndControls();
			break;

		// save current configuration
		case Key_J:
		case Key_j:
			SaveConfig();
			break;
		}
	}

	// don't handle key input if it's not player's turn
	else if (turn == Turn_ENEMY)
		cout << endl << "Wait! It is the enemy's turn!" << endl;

	// don't handle key input if the combat is happening
	else
		cout << endl << "Please, do not interrupt the combat!" << endl;
}

void GameHandler::UpdateMap()
{
	// leave mountains, lakes, trees and chests where they are and put grass everywhere
	for (size_t i(0); i < map.size(); ++i)
		for (size_t j(0); j < map[i].size(); ++j)
			if ((map[i][j] != '^') and (map[i][j] != 'T') and (map[i][j] != '~') and (map[i][j] != '#'))
				map[i][j] = ',';


	//// TODO: chests also don't change location!
	//// check every chest location and put it in the map
	//int row, col;
	//for (auto chest : chests)
	//{
	//	row = chest->location.row;
	//	col = chest->location.col;
	//	map[row][col] = '#';
	//}

	int row, col;
	// check every enemy location and put them accordingly
	for (auto enemy : enemies)
	{
		row = enemy->GetLocation().row;
		col = enemy->GetLocation().col;
		map[row][col] = '&';
	}

	// check player's location and put it in the map
	row = player->GetLocation().row;
	col = player->GetLocation().col;
	map[row][col] = 'X';
}

bool GameHandler::CheckForChests()
{
	Chest* chest;
	// for every chest, check if it is above, below, left or right from the player
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
	// if player's inventory doesn't have NUM_OF_ITEMS (4) rows, that means player doesn't have anything
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

	// if the location is valid, move player there and update map
	if (CheckNewLocation(newLocationRow, newLocationCol, map))
	{
		player->SetLocation(newLocationRow, newLocationCol);
		UpdateMap();
		// if there are any enemies surrounding new location, start battle
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
	// notify the user that it's the enemy's turn
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
	// for every enemy, check if it surrounds player
	for (size_t i(0); i < enemies.size(); ++i)
	{
		enemy = enemies[i];
		if (((enemy->GetLocation().row == player->GetLocation().row + 1) and (enemy->GetLocation().col == player->GetLocation().col)) or
			((enemy->GetLocation().row == player->GetLocation().row - 1) and (enemy->GetLocation().col == player->GetLocation().col)) or
			((enemy->GetLocation().row == player->GetLocation().row) and (enemy->GetLocation().col == player->GetLocation().col + 1)) or
			((enemy->GetLocation().row == player->GetLocation().row) and (enemy->GetLocation().col == player->GetLocation().col - 1)))
		{
			// remember which enemy in vector of enemies is next to the player
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
	// specific enemy that is found to be next to the player
	Enemy* enemy = enemies[enemyIdx];
	// fight till death
	while (playerAlive and enemyAlive)
	{
		cout << "Enemy hits" << endl;
		// enemy's damage is decreased by the amount of player's armor points
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
		// player's damage is decreased for the amount of enemy's armor points
		enemyHealthLeft = enemy->ReceiveDamage(player->GetDamage() - enemy->GetArmorPoints());
		cout << endl << "Enemy has " << enemyHealthLeft << " health points left!" << endl;
		if (enemyHealthLeft == 0)
		{
			// enemy dead
			enemy->SetLifeStatus(Status_DEAD);
			// add enemy's items to the player's inventory (enemy doesn't have health potion and magic mushroom)
			player->FillInventory(enemy->GetDamage(), enemy->GetArmorPoints(), 0, 0);
			// where once was the enemy, now is the grass
			map[enemy->GetLocation().row][enemy->GetLocation().col] = ',';
			enemyAlive = false;
			// now it's the player's turn to move
			turn = Turn_PLAYER;
			// send dead enemy to the end of the vector so it can be removed
			swap(enemies[enemyIdx], enemies[enemies.size() - 1]);
			// delete all pointers from enemy items
			enemies[enemies.size() - 1]->~Enemy();
			// remove dead enemy from the back of enemies vector
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

void GameHandler::LegendAndControls()
{
	cout << "------------------------------LEGEND------------------------------" << endl;
	cout << "X - player; , - grass; & - enemy; # - chest; ~ - lake; ^ - mountain; T - tree" << endl << endl;
	cout << "----------------------------------------------CONTROLS----------------------------------------------" << endl;
	cout << "K - show legend and controls; V - view current state of the map; J - save configuration; C - choose package of items;" << endl;
	cout << "WASD / arrow keys - movement; SPACE - loot chest; H - drink health potion; M - eat magic mushroom; Q - suicide" << endl << endl;
}

char GameHandler::MapSymbol(char letter)
{
	char symbol;
	switch (letter)
	{
	// mountain
	case 'M':
		symbol = '^';
		break;

	// tree
	case 'T':
		symbol = 'T';
		break;

	// lake
	case 'L':
		symbol = '~';
		break;
	}

	return symbol;
}

void GameHandler::SaveConfig()
{
	// file for writing new configuration
	ofstream outFile;
	// file that is loaded for current configuration
	ifstream inFile;
	string newCfgPath, currentCfgPath;
	string activeCfg = to_string(activeConfig);
	Json::Value rootRead, rootWrite;
	Json::StyledStreamWriter writer;
	int chestCnt(0), enemyCnt(0), itemsCnt(0);
	string chestCnt_str, enemyCnt_str, itemsCnt_str;

	// path to the file from which current configuration is loaded
	currentCfgPath = "../Configs/cfg" + activeCfg + ".json";
	inFile.open(currentCfgPath, ifstream::in);
	// copy content from current config file to Json::Value
	inFile >> rootRead;

	// size of the map and locations of mountains, lakes and trees are the same as in the configuration currently used
	rootWrite["size"] = rootRead["size"];
	rootWrite["mountains"] = rootRead["mountains"];
	rootWrite["lakes"] = rootRead["lakes"];
	rootWrite["trees"] = rootRead["trees"];

	char currentSymbol;
	// update map
	UpdateMap();
	// go through every point on map
	for (size_t i(0); i < map.size(); ++i)
		for (size_t j(0); j < map[i].size(); ++j)
		{
			currentSymbol = map[i][j];
			switch (currentSymbol)
			{
			// chest
			case '#':
				// number of chest found
				chestCnt_str = to_string(chestCnt);
				rootWrite["chests"][chestCnt_str]["row"] = i;
				rootWrite["chests"][chestCnt_str]["col"] = j;
				// find which chest from the vector is found
				chestIdx = FindChestIdx(i, j);
				rootWrite["chests"][chestCnt_str]["damage"] = chests[chestIdx]->items[Row_WEAPON]->GetValue();
				rootWrite["chests"][chestCnt_str]["armor"] = chests[chestIdx]->items[Row_ARMOR]->GetValue();
				rootWrite["chests"][chestCnt_str]["health"] = chests[chestIdx]->items[Row_HEALTH_POTION]->GetValue();
				rootWrite["chests"][chestCnt_str]["boost"] = chests[chestIdx]->items[Row_MAGIC_MUSHROOM]->GetValue();
				++chestCnt;
				break;

			// enemy
			case '&':
				// number of enemy found
				enemyCnt_str = to_string(enemyCnt);
				rootWrite["enemies"][enemyCnt_str]["row"] = i;
				rootWrite["enemies"][enemyCnt_str]["col"] = j;
				// find which enemy from the vector is found
				enemyIdx = FindEnemyIdx(i, j);
				rootWrite["enemies"][enemyCnt_str]["health"] = enemies[enemyIdx]->GetHealth();
				rootWrite["enemies"][enemyCnt_str]["item"]["damage"] = enemies[enemyIdx]->GetDamage();
				rootWrite["enemies"][enemyCnt_str]["item"]["armor"] = enemies[enemyIdx]->GetArmorPoints();
				++enemyCnt;
				break;

			// player
			case 'X':
				rootWrite["player"]["row"] = i;
				rootWrite["player"]["col"] = j;
				rootWrite["player"]["health"] = player->GetHealth();
				// write down all the inventory
				for (size_t k(0); k < player->GetInventoryColumnSize(); ++k)
				{
					itemsCnt_str = to_string(itemsCnt);
					rootWrite["player"]["inventory"][itemsCnt_str]["damage"] = player->GetDamage(k);
					rootWrite["player"]["inventory"][itemsCnt_str]["armor"] = player->GetArmorPoints(k);
					rootWrite["player"]["inventory"][itemsCnt_str]["health_potion"] = player->GetHealthPotion(k);
					rootWrite["player"]["inventory"][itemsCnt_str]["magic_mushroom"] = player->GetMagicMushroom(k);
					++itemsCnt;
				}
				break;

			default:
				break;
			}
		}

	// name the new file so that it keeps the first digit of the original, and the number formed by the rest of the digitis is incremented
	if (activeCfg.size() == 1)
		newCfgPath = "../Configs/cfg" + activeCfg + to_string(1) + ".json";
	else
	{
		stringstream cfgSubversion(activeCfg.substr(1, activeCfg.size() - 1));
		int newVersion;
		cfgSubversion >> newVersion;
		++newVersion;
		stringstream baseVersion(activeCfg.substr(0, 1));
		int baseVersion_num;
		baseVersion >> baseVersion_num;

		newCfgPath = "../Configs/cfg" + to_string(baseVersion_num) + to_string(newVersion) + ".json";
	}

	// write to file
	outFile.open(newCfgPath);
	writer.write(outFile, rootWrite);

	inFile.close();
	outFile.close();

	cout << endl << "Configuration saved!" << endl;
}

size_t GameHandler::FindChestIdx(int row, int col)
{
	Chest* chest;
	// find the index of registered chest, by comparing location found to the location of each chest
	for (size_t i(0); i < chests.size(); ++i)
	{
		chest = chests[i];
		if ((chest->location.row == row) and (chest->location.col == col))
			return i;
	}
}

size_t GameHandler::FindEnemyIdx(int row, int col)
{
	Enemy* enemy;
	// find the index of registered enemy, by comparing location found to the location of each enemy
	for (size_t i(0); i < enemies.size(); ++i)
	{
		enemy = enemies[i];
		if ((enemy->GetLocation().row == row) and (enemy->GetLocation().col == col))
			return i;
	}
}