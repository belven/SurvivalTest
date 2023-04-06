// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Team.generated.h"

UENUM(BlueprintType)
enum class  ERelationshipType : uint8 {
	Ally,
	Enemy,
	Neutral,
	End
};

UENUM(BlueprintType)
enum class  EFaction : uint8 {
	Synths,
	Bandits,
	Guardians,
	End
};

class UBaseGameInstance;

UINTERFACE(MinimalAPI)
class UTeam : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALTEST_API ITeam
{
	GENERATED_BODY()
public:
	virtual ERelationshipType GetRelationship(ITeam* other, UBaseGameInstance* instance);
	virtual EFaction GetFaction() { return EFaction::Synths;  }
};