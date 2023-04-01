// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Items/ItemStructs.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalGameInstance.generated.h"

#define GameInstance(world) Cast<USurvivalGameInstance>(UGameplayStatics::GetGameInstance(world))

UCLASS()
class SURVIVALTEST_API USurvivalGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	USurvivalGameInstance();

	FItemData GetItemByID(int32 itemID);
	int32 GetNextItemID();
private:
	TArray<FItemData> items;
};