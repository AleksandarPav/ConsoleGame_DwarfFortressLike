#pragma once
#include "Commons.h"
#include "Item.h"
#include <vector>
using namespace std;

class Player
{
public:
	Player(int row, int col, int health);
	~Player();

	Location GetLocation();
	void SetLocation(int x, int y);
	// fill inventory with passed item characteristics
	ChestStatus FillInventory(int damage, int armor, int health, int boost);
	// return inventory number of rows (should be 4 if inventory exists)
	size_t GetInventorySize();
	// return inventory number of columns, that is number of different item packages
	size_t GetInventoryColumnSize();
	// initiate inventory with size, if empty
	void ResizeInventory();

	// equip player
	void Equip();
	// drink health potion
	void DrinkHealthPotion();
	// eat magic mushroom (for boosting weapon damage)
	void EatMagicMushroom();

	// if nothing is passed as argument, activePackageIdx should be used; Infinity is just an indicator that no argument is passed
	int GetDamage(size_t packageIdx = INFINITY);
	int GetArmorPoints(size_t packageIdx = INFINITY);
	int GetHealthPotion(size_t packageIdx = INFINITY);
	int GetMagicMushroom(size_t packageIdx = INFINITY);
	
	LifeStatus GetLifeStatus();
	void SetLifeStatus(LifeStatus status);

	// all of the current possessions are shown on screen
	void ViewCurrentInventory();
	// check if there's anything in the inventory
	bool IsInventoryEmpty();
	// return which package of items is currently used
	size_t GetActivePackageIdx();
	// view just the package currently used
	void ViewActiveInventory();
	// take hit
	int ReceiveDamage(int damage);
	int GetHealth();

private:
	Location location;
	vector<vector<Item*>> inventory;
	int health;
	LifeStatus lifeStatus;
	// activePackageIdx is a coulmn that represents group of items currently being used
	size_t activePackageIdx;
};