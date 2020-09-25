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
	ChestStatus FillInventory(int damage, int armor, int health, int boost);
	size_t GetInventorySize();
	size_t GetInventoryColumnSize();
	void ResizeInventory();
	void BoostHealth(int h);
	void BoostDamage(int d, int idx);
	void Equip();
	void DrinkHealthPotion();
	void EatMagicMushroom();
	int GetDamage();
	int GetArmorPoints();
	
	LifeStatus GetLifeStatus();
	void SetLifeStatus(LifeStatus status);
	void ViewCurrentInventory();
	bool IsInventoryEmpty();
	size_t GetActivePackageIdx();
	void ViewActiveInventory();
	int ReceiveDamage(int damage);

private:
	Location location;
	vector<vector<Item*>> inventory;
	int health;
	LifeStatus lifeStatus;
	// there can be several packages of items in inventory; each package is one column
	size_t activePackageIdx;
};