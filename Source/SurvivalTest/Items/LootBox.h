#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStructs.h"
#include "SurvivalTest/Interactable.h"
#include "SurvivalTest/ItemContainerInterface.h"
#include "LootBox.generated.h"

class UItemContainer;

UCLASS()
class SURVIVALTEST_API ALootBox : public AActor, public IInteractable, public IItemContainerInterface
{
	GENERATED_BODY()

public:
	ALootBox();
	void SetUpBox();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual UItemContainer* GetItemContainer() override { return container; }
	virtual void Interact(ABasePlayerController* instigator) override;
	virtual void Highlight(bool activate) override;

	UFUNCTION()
		void ItemAdded(FInstanceItemData inItem);

	UFUNCTION()
	void RemoveLootBox();

	UFUNCTION()
		void ItemRemoved(FInstanceItemData inItem);

	UFUNCTION()
	void SpawnLoot();

protected:
	FInstanceItemData CreateLoot(FItemData id);

	FTimerHandle TimerHandle_LootboxClear;
	virtual void BeginPlay() override;

	UBaseGameInstance* GetGame();
	TMap<EItemType, TArray<FItemData>> GetItemList();
	void CreateLootboxData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		UStaticMesh* boxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* boxMeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
	int32 containerID = 2;

	FInstanceContainerData icd;
	FInstanceBoxData ibd;
	bool isHighlighted = false;
	int32 minTime = 10;

public:
	void SetItemTypes(TMap<EItemType, int32> inItemTypes)
	{
		itemTypes = inItemTypes;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		TMap<EItemType, int32> itemTypes;

	UPROPERTY()
	UItemContainer* container;

	UPROPERTY()
	UBaseGameInstance* gameIn;
};