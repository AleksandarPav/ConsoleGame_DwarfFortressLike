#include "Item.h"
// Weapon, Armor, Health potion and Magic mushroom are all items

Weapon::Weapon(int damage)
{
	SetValue(damage);
}

int Weapon::GetValue()
{
	return damage;
}

void Weapon::SetValue(int damage)
{
	this->damage = damage;
}


Armor::Armor(int armorPoints)
{
	SetValue(armorPoints);
}

int Armor::GetValue()
{
	return armorPoints;
}

void Armor::SetValue(int armorPoints)
{
	this->armorPoints = armorPoints;
}


HealthPotion::HealthPotion(int healthBoost)
{
	SetValue(healthBoost);
}

int HealthPotion::GetValue()
{
	return healthBoost;
}

void HealthPotion::SetValue(int healthBoost)
{
	this->healthBoost = healthBoost;
}


MagicMushroom::MagicMushroom(int damageBoost)
{
	SetValue(damageBoost);
}

int MagicMushroom::GetValue()
{
	return damageBoost;
}

void MagicMushroom::SetValue(int damageBoost)
{
	this->damageBoost = damageBoost;
}



ostream& operator<<(ostream& it, Item* mm)
{
	it << mm->GetValue() << endl;
	return it;
}


void Item::displayItem(size_t activeItem)
{
	switch (activeItem)
	{
	case 0:
		cout << "Weapon damage: ";
		break;
	case 1:
		cout << "Armor points: ";
		break;
	case 2:
		cout << "Health potion: ";
		break;
	case 3:
		cout << "Damage boost: ";
		break;
	}
}