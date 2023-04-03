#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStructs.h"
#include "SurvivalTest/Interactable.h"
#include "LootBox.generated.h"

class UItemContainer;

UCLASS()
class SURVIVALTEST_API ALootBox : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ALootBox();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	virtual void Interact(ABasePlayerController* instigator) override;

	virtual void Highlight(bool activate) override;
	USurvivalGameInstance* GetGame();
	void CreateLootboxData();
	FInstanceItemData CreateLoot(FItemData id);

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

	UPROPERTY()
	UItemContainer* container;

	UPROPERTY()
	USurvivalGameInstance* gameIn;
};