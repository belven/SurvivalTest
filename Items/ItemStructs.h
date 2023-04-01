#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStructs.generated.h"

UCLASS()
class SURVIVALTEST_API UItemStructs : public UObject
{
	GENERATED_BODY()
public:
	static const int32 InvalidInt;
};


UENUM(BlueprintType)
enum class  EWeaponType : uint8 {
	Melee,
	Projectile,
	End
};

UENUM(BlueprintType)
enum class  EItemType : uint8 {
	Weapon,
	Consumable,
	Armour,
	End
};

UENUM(BlueprintType)
enum class  ECharacterType : uint8 {
	Player,
	Neutral,
	Ally,
	Enemy,
	End
};

UENUM(BlueprintType)
enum class  EArmourSlot : uint8 {
	Head,
	Chest,
	Left_Leg,
	RIght_Leg,
	Left_Arm,
	Right_Arm,
	End
};

USTRUCT(BlueprintType)
struct FLoadoutData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	int32 entityID;
	ECharacterType type;
	int32 weaponID;
	int32 headArmourID;
	int32 chestArmourID;
	int32 leftLegArmourID;
	int32 rightLegArmourID;
	int32 leftArmArmourID;
	int32 rightArmArmourID;
	int32 health;
	int32 moveSpeed;
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	FString name;
	EItemType type;
	int32 maxStack;

	FItemData() : ID(0), type(EItemType::Consumable), maxStack(0) {	}
	FItemData(int32 id, const FString& name, EItemType type, int32 maxStack)
		: ID(id),
		name(name),
		type(type),
		maxStack(maxStack)
	{
	}

};

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID = UItemStructs::InvalidInt;
	int32 itemID = UItemStructs::InvalidInt;
	int32 amount = UItemStructs::InvalidInt;
	int32 slot = UItemStructs::InvalidInt;

	friend bool operator==(const FInventoryItemData& lhs, const FInventoryItemData& rhs)
	{
		return lhs.ID == rhs.ID;
	}

	friend bool operator!=(const FInventoryItemData& lhs, const FInventoryItemData& rhs)
	{
		return !(lhs == rhs);
	}

	bool isValid()
	{
		return ID != UItemStructs::InvalidInt;
	}

	void TakeFrom(FInventoryItemData& itemToAdd)
	{
		amount += itemToAdd.amount;
		itemToAdd.amount = 0;
	}

	FInventoryItemData CopyItem(int32 emptySlot, int32 nextID)
	{
		FInventoryItemData newData = {};
		newData.ID = nextID;
		newData.itemID = itemID;
		newData.amount = 0;
		newData.slot = emptySlot;
		return newData;
	}

	int GetRemainingSpace(int32 maxStackSize)
	{
		return maxStackSize - amount;
	}
};

USTRUCT(BlueprintType)
struct FArmourData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	int32 itemID;
	int32 resistance;
	EArmourSlot slot;
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	int32 itemID;
	EWeaponType type;
	float useRate;
	float healthChange;
	bool heals;
	int32 range;
};

USTRUCT(BlueprintType)
struct FMeleeWeaponData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	int32 weaponID;
	float cleaveRadius;
};

USTRUCT(BlueprintType)
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	int32 rangedWeaponID;
	FString projectileClass;
	int32 magazineSize;
	float reloadSpeed;
};

USTRUCT(BlueprintType)
struct FRangedWeaponData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	int32 weaponID;
	float accuracy;
};