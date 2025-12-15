#pragma once
#include "CoreMinimal.h"
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

	FItemData(int32 id, const FString& name, EItemType type, int32 maxStack, const FString& mesh)
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float gravity = 3;
};

USTRUCT(BlueprintType)
struct FRangedWeaponData
{
	GENERATED_USTRUCT_BODY()

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

UENUM(BlueprintType)
enum class ERecipeType : uint8
{
	Manual,
	Automatic,
	End
};

UENUM(BlueprintType)
enum class EInputOutputType : uint8
{
	Item, Liquid, Gas, Power, End
};

UENUM(BlueprintType)
enum class EInputOrOutput : uint8
{
	Input, Output, End
};

UENUM(BlueprintType)
enum class EOwnerType : uint8
{
	Player, AI, Group, End
};

class UBaseGameInstance;

UCLASS()
class SURVIVALTEST_API UItemStructs : public UObject
{
	GENERATED_BODY()

public:
	static const int32 InvalidInt;
	static EWeaponType GetWeaponType(const FString& typeName);
	static EGearType GetGearType(const FString& typeName);
	static EItemType GetItemType(const FString& typeName);
	static EGearType GetArmourSlot(const FString& typeName);
	static ECharacterType GetCharacterType(const FString& typeName);
	static EContainerType GetContainerType(const FString& typeName);
	static EConsumableType GetConsumableType(const FString& typeName);
	static EFireMode GetFireMode(const FString& typeName);
	static ERecipeType GetRecipeType(const FString& typeName);
	static EInputOutputType GetInputOutputType(const FString& typeName);
	static EInputOrOutput GetInputOrOutput(const FString& typeName);
	static EOwnerType GetOwnerType(const FString& typeName);
	static FString GetFireMode(EFireMode mode);
	static FString GetRecipeType(ERecipeType type);
	static FString GetInputOutputType(EInputOutputType type);
	static FString GetInputOrOutput(EInputOrOutput type);
	static FString GetOwnerType(EOwnerType type);
	static bool GetBoolean(const FString& value);
	static FItemData GetRandomItemData(UBaseGameInstance* game);

	static bool IsValidID(const int ID)
	{
		return ID != InvalidInt;
	}
};

USTRUCT(BlueprintType)
struct FArmourData
{
	GENERATED_USTRUCT_BODY()

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

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	int32 slots = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	FString name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	FString mesh = "";
};

USTRUCT(BlueprintType)
struct FContainerItemData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	int32 containerID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	int32 itemID = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FConsumableData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	int32 itemID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	EConsumableType consumableType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	int32 value = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FInstanceContainerData
{
	GENERATED_USTRUCT_BODY()

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
struct FInstanceLootBoxData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 containerInstanceID = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FInstanceArmourData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 containerInstanceID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 armourID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 instancedItemDataID = UItemStructs::InvalidInt;

	friend bool operator==(const FInstanceArmourData& lhs, const FInstanceArmourData& rhs)
	{
		return lhs.ID == rhs.ID;
	}

	friend bool operator!=(const FInstanceArmourData& lhs, const FInstanceArmourData& rhs)
	{
		return !(lhs == rhs);
	}
};



USTRUCT(BlueprintType)
struct FRecipeData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	FString name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	ERecipeType type = ERecipeType::End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	float craftingTime = 1;
};

USTRUCT(BlueprintType)
struct FRecipeInputOutputData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	int32 recipeID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	int32 inputOutputDataID = UItemStructs::InvalidInt;
};

USTRUCT(BlueprintType)
struct FInputOutputData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	int32 ID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	int32 inputOutputID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	int32 amount = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	EInputOutputType type = EInputOutputType::End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	EInputOrOutput inputOrOutput = EInputOrOutput::End;
};

USTRUCT(BlueprintType)
struct FFullRecipe
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	FRecipeData recipe;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TArray<FInputOutputData> inputs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TArray<FInputOutputData> outputs;
};

USTRUCT(BlueprintType)
struct FCraftingDeviceRecipes
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CraftingDeviceRecipes")
	int32 CraftingDeviceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CraftingDeviceRecipes")
	int32 RecipeID;
};

USTRUCT(BlueprintType)
struct FCraftingDevice
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CraftingDevice")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CraftingDevice")
	FString Name;
};

USTRUCT(BlueprintType)
struct FInstanceCraftingDevice
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InstanceCraftingDevice")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InstanceCraftingDevice")
	int32 CraftingDeviceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InstanceCraftingDevice")
	FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InstanceCraftingDevice")
	int32 OwnerID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InstanceCraftingDevice")
	EOwnerType OwnerType;
};

USTRUCT(BlueprintType)
struct FInProgressCrafting
{
	GENERATED_USTRUCT_BODY()

	FInProgressCrafting(): ID(-1), InstanceCraftingDeviceID(0), RecipeID(0), QueuePosition(0), Progress(0)
	{
	}

	FInProgressCrafting(int32 inID, int32 inInstanceCraftingDeviceID, int32 inRecipeID, int32 inQueuePosition, float inProgress)
		: ID(inID),
		  InstanceCraftingDeviceID(inInstanceCraftingDeviceID),
		  RecipeID(inRecipeID),
		  QueuePosition(inQueuePosition),
		  Progress(inProgress)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InProgressCrafting")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InProgressCrafting")
	int32 InstanceCraftingDeviceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InProgressCrafting")
	int32 RecipeID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InProgressCrafting")
	int32 QueuePosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InProgressCrafting")
	float Progress;
};


USTRUCT(BlueprintType)
struct FItemsToAdd
{
	GENERATED_USTRUCT_BODY()

	FItemsToAdd() {}

	FItemsToAdd(int32 inItemID, int32 inAmount)
		: itemID(inItemID),
		amount(inAmount)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 itemID = UItemStructs::InvalidInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 amount = UItemStructs::InvalidInt;
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
	
	 bool isValid() const
	{
		return UItemStructs::IsValidID(ID);
	}

	void TakeFrom(FInstanceItemData& itemToAdd, int32 stackSize)
	{
		int32 space = GetRemainingSpace(stackSize);
		int32 amountToTake = FMath::Min(itemToAdd.amount, space);
		amount += amountToTake;
		itemToAdd.amount -= amountToTake;
	}
	void TakeFrom(FItemsToAdd& itemToAdd, int32 stackSize)
	{
		int32 space = GetRemainingSpace(stackSize);
		int32 amountToTake = FMath::Min(itemToAdd.amount, space);
		amount += amountToTake;
		itemToAdd.amount -= amountToTake;
	}

	FInstanceItemData CopySlotAndContainer(const FInstanceItemData& other) const
	{
		FInstanceItemData updatedData = *this;
		updatedData.slot = other.slot;
		updatedData.containerInstanceID = other.containerInstanceID;
		return updatedData;
	}

	static FInstanceItemData CreateEmptyItem(const FInstanceItemData& other)
	{
		return CreateEmptyItem(other.slot, other.containerInstanceID);
	}

	static FInstanceItemData CreateEmptyItem(const int32& slot, const int32& newContainerInstanceID)
	{
		FInstanceItemData iid;
		iid.slot = slot;
		iid.containerInstanceID = newContainerInstanceID;
		return iid;
	}

	FInstanceItemData CreateEmptyCopy()
	{
		return CreateEmptyItem(*this);
	}

	// Single method to get a copy of the item with new data, basically just copies itemID
	FInstanceItemData CopyItem(int32 nextID, int32 instanceContainerID, int32 emptySlot, int32 newAmount = 0)
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
