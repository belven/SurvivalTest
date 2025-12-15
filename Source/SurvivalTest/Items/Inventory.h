#pragma once
#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "Inventory.generated.h"

class UEquipmentSwapTask;
DECLARE_LOG_CATEGORY_EXTERN(Inventory, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContainerUpdated, UItemContainer*, container);

class ABaseCharacter;
class UWeapon;
class UArmour;

UCLASS()
class SURVIVALTEST_API UInventory : public UItemContainer
{
	GENERATED_BODY()
public:
	static UInventory* CreateInventory(const FContainerData& inContainerData, const FInstanceContainerData& inInstanceContainerData, UBaseGameInstance* inGame, ABaseCharacter* inCharacterOwner);

	FORCEINLINE UWeapon* GetEquippedWeapon() { return equippedWeapon; }

	UFUNCTION()
		void CreateNewItemForInventory(int32 itemID);

	FOnContainerUpdated OnContainerAdded;
	FOnContainerUpdated OnContainerRemoved;

	virtual FString GetContainerName() override;

	void SetEquippedWeapon(UWeapon* weapon);
	void EquipArmour(UArmour* armour);

	void CheckWeaponItems(const FInstanceItemData& oldInstanceItemData, const FItemData& newItemData, const FItemData& inOldItemData);
	void PerformWeaponSwap(int32 slot);
	void CheckArmourItems(const FInstanceItemData& updatedInstanceItem, const FInstanceItemData& newInstanceItemData, const FInstanceItemData& oldInstanceItemData, const FItemData& newItemData, const FItemData& oldItemData);
	void GetItemArmourData(int32 instanceItemID, FInstanceArmourData& instanceArmourData, FArmourData& armourData);

	UFUNCTION()
		void ItemUpdated(const FInstanceItemData& newItem, const FInstanceItemData& oldItem);

	TArray<int32> GetSlotForGear(EGearType type);
	int32 GetPrimaryWeaponSlot();
	int32 GetSecondaryWeaponSlot();
	int32 GetSidearmWeaponSlot();

	UFUNCTION()
		void SetupLoadout(const FLoadoutData& ld);

	TMap<EGearType, UArmour*>& GetEquippedArmour()
	{
		return equippedArmour;
	}

private:
	UPROPERTY()
	ABaseCharacter* characterOwner;

	UPROPERTY()
	UEquipmentSwapTask* equipmentSwapTask;

	UPROPERTY()
		UWeapon* equippedWeapon;

	UPROPERTY()
		TMap<EGearType, UArmour*> equippedArmour;	
};