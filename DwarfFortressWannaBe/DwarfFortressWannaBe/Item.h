#pragma once
#include <iostream>
#include <string>
using namespace std;

// abstract class
class Item
{
public:
	virtual int GetValue() = 0;
	virtual void SetValue(int) = 0;
	// item can be displayed by the operator<<
	friend ostream& operator<<(ostream& it, Item* mm);
	// indicator for the item used
	size_t activeItem;
	void displayItem(size_t activeItem);
};

// value becomes damage
class Weapon : public Item
{
public:
	Weapon(int damage);
	virtual int GetValue() override;
	virtual void SetValue(int damage) override;
private:
	int damage;
};

// value becomes armor points
class Armor : public Item
{
public:
	Armor(int armorPoints);
	virtual int GetValue() override;
	virtual void SetValue(int armorPoints) override;
private:
	int armorPoints;
};

// value becomes health points
class HealthPotion : public Item
{
public:
	HealthPotion(int health);
	virtual int GetValue() override;
	virtual void SetValue(int) override;
private:
	int healthBoost;
};

// value becomes damage boost
class MagicMushroom : public Item
{
public:
	MagicMushroom(int damageBoost);
	virtual int GetValue() override;
	virtual void SetValue(int damageBoost) override;
private:
	int damageBoost;
};