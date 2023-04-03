#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStructs.h"
#include "LootBox.generated.h"

class UItemContainer;

UCLASS()
class SURVIVALTEST_API ALootBox : public AActor
{
	GENERATED_BODY()

public:
	ALootBox();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	USurvivalGameInstance* GetGame();
	void CreateLootboxData();
	FInstanceItemData CreateLoot(FItemData id);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		int32 boxID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
	int32 containerID = 2;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
		int32 itemQuantity;

	FInstanceContainerData icd;
	FInstanceBoxData ibd;

	UPROPERTY()
	UItemContainer* container;

	UPROPERTY()
	USurvivalGameInstance* gameIn;
};