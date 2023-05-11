#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SurvivalTest/Missions/MissionStructs.h"
#include "ItemStructs.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Melee,
	Projectile,
	End
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Consumable,
	Armour,
	Resource,
	Ammo,
	End
};

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	Player,
	Neutral,
	Ally,
	Enemy,
	End
};

UENUM(BlueprintType)
enum class EGearType : uint8
{
	Head,
	Chest,
	Legs,
	Weapon,
	Sidearm,
	Bag,
	Vest,
	End
};

USTRUCT(BlueprintType)
struct FLoadoutData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	FString name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	ECharacterType type = ECharacterType::End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 weaponID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 headArmourID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 chestArmourID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 vestArmourID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 legsArmourID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 health = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 moveSpeed = 800;
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString mesh = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType type = EItemType::End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 maxStack = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString icon = "";

	FItemData() : ID(0), type(EItemType::Consumable), maxStack(0)
	{
	}

	FItemData(int32 id, const FString& name, EItemType type, int32 maxStack, FString mesh)
		: ID(id),
		  name(name),
		  mesh(mesh),
		  type(type),
		  maxStack(maxStack)
	{
	}
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 itemID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType type = EWeaponType::End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EGearType gearType = EGearType::End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float useRate = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float healthChange = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool heals = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 range = -1;
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SemiAuto,
	FullAuto,
	Burst,
	End
};

USTRUCT(BlueprintType)
struct FInstanceWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 instanceItemID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 ammo = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EFireMode mode = EFireMode::FullAuto;
};

USTRUCT(BlueprintType)
struct FMeleeWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 weaponID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float cleaveRadius = 100;
};

USTRUCT(BlueprintType)
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 rangedWeaponID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ammoID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 magazineSize = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float reloadSpeed = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 bulletVelocity = 1;
};

USTRUCT(BlueprintType)
struct FRangedWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 weaponID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float accuracy = 1;
};

UENUM(BlueprintType)
enum class EContainerType : uint8
{
	Box,
	Armour,
	Player,
	End
};

UENUM(BlueprintType)
enum class EConsumableType : uint8
{
	Food,
	Drink,
	Medical,
	End
};


class UBaseGameInstance;

UCLASS()
class SURVIVALTEST_API UItemStructs : public UObject
{
	GENERATED_BODY()

public:
	static const int32 InvalidInt;
	static EWeaponType GetWeaponType(FString typeName);
	static EGearType GetGearType(FString typeName);
	static EItemType GetItemType(FString typeName);
	static EGearType GetArmourSlot(FString typeName);
	static ECharacterType GetCharacterType(FString typeName);
	static EContainerType GetContainerType(FString typeName);
	static EConsumableType GetConsumableType(FString typeName);
	static EFireMode GetFireMode(FString typeName);
	static FString GetFireMode(EFireMode mode);
	static bool GetBoolean(FString value);
	static FItemData GetRandomItemData(UBaseGameInstance* game);
};

USTRUCT(BlueprintType)
struct FArmourData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 itemID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 containerID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 resistance = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	EGearType slot = EGearType::End;
};

USTRUCT(BlueprintType)
struct FContainerData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 slots = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString mesh = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EMissionType type = EMissionType::End;
};

USTRUCT(BlueprintType)
struct FConsumableData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	int32 itemID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	int32 value = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	EConsumableType consumableType;
};

USTRUCT(BlueprintType)
struct FInstanceContainerData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 containerID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	EContainerType type = EContainerType::End;

	friend bool operator==(const FInstanceContainerData& lhs, const FInstanceContainerData& rhs)
	{
		return lhs.ID == rhs.ID;
	}

	friend bool operator!=(const FInstanceContainerData& lhs, const FInstanceContainerData& rhs)
	{
		return !(lhs == rhs);
	}
};

USTRUCT(BlueprintType)
struct FInstanceBoxData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 boxID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 containerInstanceID = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FInstanceArmourData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 containerInstanceID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 armourID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 instancedItemDataID = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FInstanceItemData
{
	GENERATED_USTRUCT_BODY()

	FInstanceItemData() {}
	FInstanceItemData(int32 inItemID, int32 newAmount) : itemID(inItemID), amount(newAmount) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;

	FInstanceItemData(int32 inID, int32 inItemID, int32 inContainerInstanceID, int32 inAmount, int32 inSlot)
		: ID(inID),
		  itemID(inItemID),
		  containerInstanceID(inContainerInstanceID),
		  amount(inAmount),
		  slot(inSlot)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 itemID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 containerInstanceID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 amount = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
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

	void TakeFrom(FInstanceItemData& itemToAdd, int32 stackSize)
	{
		int32 space = GetRemainingSpace(stackSize);
		int32 amountToTake = FMath::Min(itemToAdd.amount, space);
		amount += amountToTake;
		itemToAdd.amount -= amountToTake;
	}

	// Single method to get a copy of the item with new data, basically just copies itemID
	FInstanceItemData CopyItem(int32 nextID,  int32 instanceContainerID, int32 emptySlot, int32 newAmount = 0)
	{
		return FInstanceItemData(nextID, itemID, instanceContainerID, newAmount, emptySlot);
	}

	bool HasSpace(int32 maxStackSize)
	{
		return GetRemainingSpace(maxStackSize) > 0;
	}

	int GetRemainingSpace(int32 maxStackSize)
	{
		return maxStackSize - amount;
	}
};
