#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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
	Primary_Weapon,
	Secondary_Weapon,
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
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 entityID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	ECharacterType type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 weaponID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 headArmourID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 chestArmourID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 legsArmourID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loadout")
	int32 moveSpeed;
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 maxStack;

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
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 itemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EGearType gearType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float useRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float healthChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool heals;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 range;
};

USTRUCT(BlueprintType)
struct FMeleeWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 weaponID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float cleaveRadius;
};

USTRUCT(BlueprintType)
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 rangedWeaponID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FString projectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 magazineSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float reloadSpeed;
};

USTRUCT(BlueprintType)
struct FRangedWeaponData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 weaponID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float accuracy;
};

UENUM(BlueprintType)
enum class EContainerType : uint8
{
	Box,
	Armour,
	Player,
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
	static bool GetBoolean(FString value);
	static FItemData GetRandomItemData(UBaseGameInstance* game);
};

USTRUCT(BlueprintType)
struct FArmourData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 itemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 containerID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	int32 resistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armour")
	EGearType slot;
};

USTRUCT(BlueprintType)
struct FContainerData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 slots = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString name;
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
	EContainerType type;

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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
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
		int32 amountToTake = FMath::Min(stackSize, itemToAdd.amount);
		amount += amountToTake;
		itemToAdd.amount -= amountToTake;
	}

	FInstanceItemData CopyItem(int32 emptySlot, int32 nextID, int32 instanceContainerID)
	{
		FInstanceItemData newData = {};
		newData.ID = nextID;
		newData.itemID = itemID;
		newData.containerInstanceID = instanceContainerID;
		newData.amount = 0;
		newData.slot = emptySlot;
		return newData;
	}

	int GetRemainingSpace(int32 maxStackSize)
	{
		return maxStackSize - amount;
	}
};
