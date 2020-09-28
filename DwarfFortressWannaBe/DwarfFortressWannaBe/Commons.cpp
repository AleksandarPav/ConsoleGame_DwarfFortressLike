#include "Commons.h"

Location::Location(int row, int col) : row(row), col(col)
{

}

// create items
Chest::Chest(int row, int col, int damage, int armor, int health, int boost) : location(row, col)
{
	items.push_back(new Weapon(damage));
	items.push_back(new Armor(armor));
	items.push_back(new HealthPotion(health));
	items.push_back(new MagicMushroom(boost));
}

// remove all item pointers
Chest::~Chest()
{
	for (size_t i(0); i < items.size(); ++i)
	{
		delete items[i];
		items[i] = nullptr;
	}
}

// when they're picked up, they don't have values anymore (their value is zero)
void Chest::CleanItems()
{
	for (auto item : items)
		item->SetValue(0);
}

// check if the passed life status is valid
bool CheckLifeStatus(LifeStatus status)
{
	if ((status == Status_ALIVE) or (status == Status_DEAD))
		return true;

	return false;
}

bool CheckNewLocation(int newLocationRow, int newLocationCol, vector<vector<char>>& map)
{
	// if it is within the borders and the desired field is grass, than ok
	if ((newLocationRow >= 0) and (newLocationCol >= 0) and (newLocationRow < map.size()) and (newLocationCol < map[0].size()) and (map[newLocationRow][newLocationCol] == ','))
		return true;
	return false;
}