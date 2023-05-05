#include "ItemStructs.h"

#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/Tables/Items/ItemDataTable.h"
#include "SurvivalTest/Tables/TableManager.h"


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

EGearType UItemStructs::GetGearType(FString typeName)
{
	if (typeName.Equals("Legs"))
	{
		return EGearType::Legs;
	}
	else if (typeName.Equals("Vest"))
	{
		return EGearType::Vest;
	}
	else if (typeName.Equals("Bag"))
	{
		return EGearType::Bag;
	}
	else if (typeName.Equals("Chest"))
	{
		return EGearType::Chest;
	}
	else if (typeName.Equals("Head"))
	{
		return EGearType::Head;
	}
	else if (typeName.Equals("Weapon"))
	{
		return EGearType::Weapon;
	}
	else if (typeName.Equals("Sidearm"))
	{
		return EGearType::Sidearm;
	}
	else if (typeName.Equals("Legs"))
	{
		return EGearType::Legs;
	}
	return EGearType::Legs;
}

EItemType UItemStructs::GetItemType(FString typeName)
{
	if (typeName.Equals("Weapon"))
	{
		return EItemType::Weapon;
	}
	else	if (typeName.Equals("Consumable"))
	{
		return EItemType::Consumable;
	}
	else	if (typeName.Equals("Armour"))
	{
		return EItemType::Armour;
	}
	else	if (typeName.Equals("Resource"))
	{
		return EItemType::Resource;
	}
	return EItemType::Consumable;
}

EGearType UItemStructs::GetArmourSlot(FString typeName)
{
	if (typeName.Equals("Chest"))
	{
		return EGearType::Chest;
	}
	else 	if (typeName.Equals("Head"))
	{
		return EGearType::Head;
	}
	else 	if (typeName.Equals("Legs"))
	{
		return EGearType::Legs;
	}
	else 	if (typeName.Equals("Vest"))
	{
		return EGearType::Vest;
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

EConsumableType UItemStructs::GetConsumableType(FString typeName)
{
	if (typeName.Equals("Drink"))
	{
		return EConsumableType::Drink;
	}
	if (typeName.Equals("Food"))
	{
		return EConsumableType::Food;
	}
	if (typeName.Equals("Medical"))
	{
		return EConsumableType::Medical;
	}
	return EConsumableType::Drink;
}

EFireMode UItemStructs::GetFireMode(FString typeName)
{
	if (typeName.Equals("FullAuto"))
	{
		return EFireMode::FullAuto;
	}
	if (typeName.Equals("SemiAuto"))
	{
		return EFireMode::SemiAuto;
	}
	if (typeName.Equals("Burst"))
	{
		return EFireMode::Burst;
	}
	return EFireMode::End;
}

FString UItemStructs::GetFireMode(EFireMode mode)
{
	switch (mode) {
		case EFireMode::SemiAuto: return "SemiAuto";
	case EFireMode::FullAuto: return "FullAuto";
	case EFireMode::Burst: return "Burst";
	case EFireMode::End: return "End";
	default: return "End";
	}
}

bool UItemStructs::GetBoolean(FString value)
{
	return value.Equals("true") ? true : false;
}

FItemData UItemStructs::GetRandomItemData(UBaseGameInstance* game)
{
	TArray<FItemData> itemData = game->GetTableManager()->GetItemDataTable()->GetData();
	FItemData id = itemData[FMath::RandRange(0, itemData.Num() - 1)];
	return id;
}
