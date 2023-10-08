#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStructs.h"
#include "SurvivalTest/Interfaces/Interactable.h"
#include "SurvivalTest/Interfaces/ItemContainerInterface.h"
#include "LootBox.generated.h"

class UItemContainer;

UCLASS(HideCategories = ("Rendering", "Replication", "Collision", "HLOD", "World_Partition", "Input", "Replication", "Actor", "Cooking", "Data_Layers"))
class SURVIVALTEST_API ALootBox : public AActor, public IInteractable, public IItemContainerInterface
{
	friend class FSpawnLootRunnable;

	GENERATED_BODY()

public:
	ALootBox();
	virtual ~ALootBox() override;
	void SetUpBox();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual UItemContainer* GetItemContainer() override { return container; }
	virtual void Interact(ABasePlayerController* instigator) override;
	virtual void Highlight(bool activate) override;

	FContainerData GetContainerData() const { return containerData; }
	void SetContainerData(FContainerData inContainerData) { containerData = inContainerData; }

	UFUNCTION()
	void ItemUpdated(FInstanceItemData inItem, FInstanceItemData oldItem);

	UFUNCTION()
	void RemoveLootBox();
	
	UFUNCTION()
	void SpawnLoot();
	void ClearData();

	void SetItemTypes(TArray<int32>inItemTypes) { itemTypes = inItemTypes; }

protected:
	FInstanceItemData CreateLoot(FItemData id);

	FTimerHandle TimerHandle_LootboxClear;
	virtual void BeginPlay() override;

	UBaseGameInstance* GetGame();
	void CreateLootboxData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* boxMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		int32 minItems = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		FString defaultContainer;
	
	FContainerData containerData;

	FInstanceContainerData icd;
	FInstanceBoxData ibd;

	bool isHighlighted = false;
	int32 minTime = 10;
	
	TArray<int32> itemTypes;
	
	UPROPERTY()
	UItemContainer* container;

	UPROPERTY()
	UBaseGameInstance* gameIn;
};