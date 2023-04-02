#include "ItemStructs.h"


const int32 UItemStructs::InvalidInt = -1;

EWeaponType UItemStructs::GetWeaponType(FString typeName)
{
	if(typeName.Equals("Melee"))
	{
		return EWeaponType::Melee;
	}
	else if (typeName.Equals("Projectile"))
	{
		return EWeaponType::Projectile;
	}
	return EWeaponType::Melee;
}

EItemType UItemStructs::GetItemType(FString typeName)
{
	if (typeName.Equals("Weapon"))
	{
		return EItemType::Weapon;
	}
	else if (typeName.Equals("Consumable"))
	{
		return EItemType::Consumable;
	}
	else if (typeName.Equals("Armour"))
	{
		return EItemType::Armour;
	}
	return EItemType::Consumable;
}

EArmourSlot UItemStructs::GetArmourSlot(FString typeName)
{
	if (typeName.Equals("Chest"))
	{
		return EArmourSlot::Chest;
	}
	else if (typeName.Equals("Head"))
	{
		return EArmourSlot::Head;
	}
	else if (typeName.Equals("LeftArm"))
	{
		return EArmourSlot::Left_Arm;
	}
	else if (typeName.Equals("RightArm"))
	{
		return EArmourSlot::Right_Arm;
	}
	else if (typeName.Equals("LeftLeg"))
	{
		return EArmourSlot::Left_Leg;
	}
	else if (typeName.Equals("RIghtLeg"))
	{
		return EArmourSlot::RIght_Leg;
	}
	return EArmourSlot::Head;
}


ECharacterType UItemStructs::GetCharacterType(FString typeName)
{
	if (typeName.Equals("Player"))
	{
		return ECharacterType::Player;
	}
	else if (typeName.Equals("Ally"))
	{
		return ECharacterType::Ally;
	}
	else if (typeName.Equals("Enemy"))
	{
		return ECharacterType::Enemy;
	}
	else if (typeName.Equals("Neutral"))
	{
		return ECharacterType::Neutral;
	}
	return ECharacterType::Neutral;
}

EContainerType UItemStructs::GetContainerType(FString typeName)
{
	if (typeName.Equals("Armour"))
	{
		return EContainerType::Armour;
	}
	else if (typeName.Equals("Box"))
	{
		return EContainerType::Box;
	}
	return EContainerType::Box;
}

bool UItemStructs::GetBoolean(FString value)
{
	return value.Equals("true") ? true : false;
}
