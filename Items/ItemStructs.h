#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStructs.generated.h"

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
	Legs,
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
	int32 legsArmourID;
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
	FString mesh;
	EItemType type;
	int32 maxStack;

	FItemData() : ID(0), type(EItemType::Consumable), maxStack(0) {	}
	FItemData(int32 id, const FString& name, EItemType type, int32 maxStack, FString mesh)
		: ID(id),
		name(name),
		type(type),
		maxStack(maxStack),
		mesh(mesh)
	{
	}

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

UENUM(BlueprintType)
enum class  EContainerType : uint8 {
	Box,
	Armour,
	End
};

class USurvivalGameInstance;

UCLASS()
class SURVIVALTEST_API UItemStructs : public UObject
{
	GENERATED_BODY()
public:
	static const int32 InvalidInt;
	static EWeaponType GetWeaponType(FString typeName);
	static EItemType GetItemType(FString typeName);
	static EArmourSlot GetArmourSlot(FString typeName);
	static ECharacterType GetCharacterType(FString typeName);
	static EContainerType GetContainerType(FString typeName);
	static bool GetBoolean(FString value);
	static FItemData GetRandomItemData(USurvivalGameInstance* game);
};

USTRUCT(BlueprintType)
struct FArmourData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID;
	int32 itemID;
	int32 containerID;
	int32 resistance;
	EArmourSlot slot;
};

USTRUCT(BlueprintType)
struct FContainerData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID = UItemStructs::InvalidInt;
	int32 slots = 10;
	FString name;
};


USTRUCT(BlueprintType)
struct FInstanceContainerData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID = UItemStructs::InvalidInt;
	int32 containerID = UItemStructs::InvalidInt;
	EContainerType type;
};

USTRUCT(BlueprintType)
struct FInstanceBoxData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID = UItemStructs::InvalidInt;
	int32 boxID = UItemStructs::InvalidInt;
	int32 containerInstanceID = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FInstanceArmourData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID = UItemStructs::InvalidInt;
	int32 containerInstanceID = UItemStructs::InvalidInt;
	int32 armourID = UItemStructs::InvalidInt;
	int32 instancedItemDataID = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FInstanceItemData
{
	GENERATED_USTRUCT_BODY()
public:
	int32 ID = UItemStructs::InvalidInt;
	int32 itemID = UItemStructs::InvalidInt;
	int32 containerInstanceID = UItemStructs::InvalidInt;
	int32 amount = UItemStructs::InvalidInt;
	int32 slot = UItemStructs::InvalidInt;

	friend bool operator==(const FInstanceItemData& lhs, const FInstanceItemData& rhs)
	{
		return lhs.ID == rhs.ID;
	}

	friend bool operator!=(const FInstanceItemData& lhs, const FInstanceItemData& rhs)
	{
		return !(lhs == rhs);
	}

	bool isValid()
	{
		return ID != UItemStructs::InvalidInt;
	}

	void TakeFrom(FInstanceItemData& itemToAdd)
	{
		amount += itemToAdd.amount;
		itemToAdd.amount = 0;
	}

	FInstanceItemData CopyItem(int32 emptySlot, int32 nextID)
	{
		FInstanceItemData newData = {};
		newData.ID = nextID;
		newData.itemID = itemID;
		newData.containerInstanceID = containerInstanceID;
		newData.amount = 0;
		newData.slot = emptySlot;
		return newData;
	}

	int GetRemainingSpace(int32 maxStackSize)
	{
		return maxStackSize - amount;
	}
};