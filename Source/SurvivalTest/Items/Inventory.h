#pragma once
#include "CoreMinimal.h"
#include "ItemContainer.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainersUpdated);

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

	FOnContainersUpdated OnContainersUpdated;
	virtual UWorld* GetWorld() const override;

	void SetEquippedWeapon(UWeapon* weapon);
	void EquipArmour(UArmour* armour);
	
	UFUNCTION()
		void ItemUpdated(const FInstanceItemData& inItem, const FInstanceItemData& oldItem);

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
		UWeapon* equippedWeapon;

	UPROPERTY()
		TMap<EGearType, UArmour*> equippedArmour;	
};