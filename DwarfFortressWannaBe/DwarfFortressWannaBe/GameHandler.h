#pragma once
#pragma warning (disable : 4996) // something deprecated with jsoncpp
#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include "Enemy.h"
#include "Player.h"
#include "Commons.h"
#include <json/json.h>
#include <json/writer.h>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <string>

using namespace std;


class GameHandler
{

public:
	GameHandler();
	~GameHandler();

	// whole game cycle
	char GameCycle();
	// welcome message and instructions
	void Init();
	// loading configuration
	void LoadConfig();
	// main funcionality
	void MainGameLoop();

	// filling map with objects defined in json and initiating them
	void FillMapWithNature(const Json::Value& root, const string& object);
	void FillMapWithChests(const Json::Value& root, const string& object);
	void FillMapWithEnemies(const Json::Value& root, const string& object);
	void InitPlayer(const Json::Value& root, const string& object);

	// if something changed on the map, update
	void UpdateMap();
	// display map on screen
	void DisplayMap();
	// get key from keyboard in real time
	int GetKey();
	// depending on the key pressed, what should happen
	void HandleKey(int ch, Turn& turn);
	// what enemies should when it's their turn
	void ActivateEnemy();

private:
	// ptr to player
	Player* player;
	// group of enemies
	vector<Enemy*> enemies;

	// map is represented as a matrix of characters
	vector<vector<char>> map;
	// group of chests
	vector<Chest*> chests;
	// defines is it player's or enemy's turn, or combat is happening
	Turn turn;

	// loot chest
	void PickItemsFromChest();
	// check if there are any chests around player's current location
	bool CheckForChests();
	// check if there are any enemies around the player
	bool CheckForEnemies();
	// defines which chest in vector of chests is found
	size_t chestIdx;
	// defines which enemy in vector of enemies is found
	size_t enemyIdx;
	// decide wheather it's ok for player to move to desired location
	void MovementDecision(int addRow, int addCol);
	// functionalities of combat
	CombatResult Combat();
	// return value of combat - for winning, losing or continuing the game
	CombatResult combatResult;
};