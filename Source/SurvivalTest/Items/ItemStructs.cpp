#include "ItemStructs.h"

#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/Tables/ItemDataTable.h"


const int32 UItemStructs::InvalidInt = -1;

EWeaponType UItemStructs::GetWeaponType(FString typeName)
{
	if (typeName.Equals("Melee"))
	{
		return EWeaponType::Melee;
	}
	if (typeName.Equals("Projectile"))
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
	if (typeName.Equals("Consumable"))
	{
		return EItemType::Consumable;
	}
	if (typeName.Equals("Armour"))
	{
		return EItemType::Armour;
	}
	return EItemType::Consumable;
}

EGearType UItemStructs::GetArmourSlot(FString typeName)
{
	if (typeName.Equals("Chest"))
	{
		return EGearType::Chest;
	}
	if (typeName.Equals("Head"))
	{
		return EGearType::Head;
	}
	if (typeName.Equals("Legs"))
	{
		return EGearType::Legs;
	}
	return EGearType::Head;
}

ECharacterType UItemStructs::GetCharacterType(FString typeName)
{
	if (typeName.Equals("Player"))
	{
		return ECharacterType::Player;
	}
	if (typeName.Equals("Ally"))
	{
		return ECharacterType::Ally;
	}
	if (typeName.Equals("Enemy"))
	{
		return ECharacterType::Enemy;
	}
	if (typeName.Equals("Neutral"))
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
	if (typeName.Equals("Box"))
	{
		return EContainerType::Box;
	}
	return EContainerType::Box;
}

bool UItemStructs::GetBoolean(FString value)
{
	return value.Equals("true") ? true : false;
}

FItemData UItemStructs::GetRandomItemData(UBaseGameInstance* game)
{
	TArray<FItemData> itemData = game->GetItemDataTable()->GetData();
	FItemData id = itemData[FMath::RandRange(0, itemData.Num() - 1)];
	return id;
}
