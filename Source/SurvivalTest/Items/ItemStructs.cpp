#include "ItemStructs.h"

#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/Tables/Items/ItemDataTable.h"
#include "SurvivalTest/Tables/TableManager.h"


const int32 UItemStructs::InvalidInt = -1;

EWeaponType UItemStructs::GetWeaponType(const FString& typeName)
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

EGearType UItemStructs::GetGearType(const FString& typeName)
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

EItemType UItemStructs::GetItemType(const FString& typeName)
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
	else	if (typeName.Equals("Ammo"))
	{
		return EItemType::Ammo;
	}
	else	if (typeName.Equals("Resource"))
	{
		return EItemType::Resource;
	}
	return EItemType::Consumable;
}

EGearType UItemStructs::GetArmourSlot(const FString& typeName)
{
	if (typeName.Equals("Chest"))
	{
		return EGearType::Chest;
	}
	else if (typeName.Equals("Head"))
	{
		return EGearType::Head;
	}
	else if (typeName.Equals("Legs"))
	{
		return EGearType::Legs;
	}
	else  if (typeName.Equals("Vest"))
	{
		return EGearType::Vest;
	}
	return EGearType::Head;
}

ECharacterType UItemStructs::GetCharacterType(const FString& typeName)
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

EContainerType UItemStructs::GetContainerType(const FString& typeName)
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

EConsumableType UItemStructs::GetConsumableType(const FString& typeName)
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

EFireMode UItemStructs::GetFireMode(const FString& typeName)
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

ERecipeType UItemStructs::GetRecipeType(const FString& typeName)
{
	if (typeName.Equals("Automatic"))
	{
		return ERecipeType::Automatic;
	}
	if (typeName.Equals("Manual"))
	{
		return ERecipeType::Manual;
	}
	return ERecipeType::End;
}

EInputOutputType UItemStructs::GetInputOutputType(const FString& typeName)
{
	if (typeName.Equals("Item"))
	{
		return EInputOutputType::Item;
	}
	if (typeName.Equals("Gas"))
	{
		return EInputOutputType::Gas;
	}
	if (typeName.Equals("Liquid"))
	{
		return EInputOutputType::Liquid;
	}
	if (typeName.Equals("Power"))
	{
		return EInputOutputType::Power;
	}
	return EInputOutputType::End;
}

EInputOrOutput UItemStructs::GetInputOrOutput(const FString& typeName)
{
	if (typeName.Equals("Input"))
	{
		return EInputOrOutput::Input;
	}
	if (typeName.Equals("Output"))
	{
		return EInputOrOutput::Output;
	}
	return EInputOrOutput::End;
}

EOwnerType UItemStructs::GetOwnerType(const FString& typeName)
{
	if (typeName.Equals("AI"))
	{
		return EOwnerType::AI;
	}
	if (typeName.Equals("Player"))
	{
		return EOwnerType::Player;
	}
	if (typeName.Equals("Group"))
	{
		return EOwnerType::Group;
	}
	return EOwnerType::End;
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

FString UItemStructs::GetRecipeType(ERecipeType type)
{
	switch (type) {
	case ERecipeType::Automatic: return "Automatic";
	case ERecipeType::Manual: return "Manual";
	case ERecipeType::End: return "End";
	default: return "End";
	}
}


FString UItemStructs::GetInputOutputType(EInputOutputType type)
{
	switch (type) {
	case EInputOutputType::Item: return "Item";
	case EInputOutputType::Gas: return "Gas";
	case EInputOutputType::Liquid: return "Liquid";
	case EInputOutputType::Power: return "Power";
	case EInputOutputType::End: return "End";
	default: return "End";
	}
}

FString UItemStructs::GetInputOrOutput(EInputOrOutput type)
{
	switch (type) {
	case EInputOrOutput::Input: return "Input";
	case EInputOrOutput::Output: return "Output";
	case EInputOrOutput::End: return "End";
	default: return "End";
	}
}

FString UItemStructs::GetOwnerType(EOwnerType type)
{
	switch (type)
	{
	case EOwnerType::AI:
		return "AI";
	case EOwnerType::Player:
		return "Player";
	case EOwnerType::Group:
		return "Group";
	default:
		return "End";
	}
}

bool UItemStructs::GetBoolean(const FString& value)
{
	return value.Equals("true") ? true : false;
}

FItemData UItemStructs::GetRandomItemData(UBaseGameInstance* game)
{
	TArray<FItemData> itemData = game->GetTableManager()->GetItemDataTable()->GetData();
	FItemData id = itemData[FMath::RandRange(0, itemData.Num() - 1)];
	return id;
}
