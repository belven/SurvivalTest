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
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual UItemContainer* GetItemContainer() override { return container; }
	virtual void Interact(ABasePlayerController* instigator) override;
	virtual void Highlight(bool activate) override;
	FInstanceItemData CreateLoot(FItemData id);

	UFUNCTION()
		void ItemAdded(FInstanceItemData inItem);

	void RemoveLootBox();
	UFUNCTION()
		void ItemRemoved(FInstanceItemData inItem);

protected:
	FTimerHandle TimerHandle_LootboxClear;
	virtual void BeginPlay() override;

	UBaseGameInstance* GetGame();
	void CreateLootboxData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		UStaticMesh* boxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* boxMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		int32 boxID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
	int32 containerID = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		int32 itemQuantity;

	FInstanceContainerData icd;
	FInstanceBoxData ibd;
	bool isHighlighted = false;
	int32 minTime = 10;
	
	UPROPERTY()
	UItemContainer* container;

	UPROPERTY()
	UBaseGameInstance* gameIn;
};