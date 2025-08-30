// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTask.h"
#include "ReloadTask.generated.h"

UCLASS()
class SURVIVALTEST_API UReloadTask : public UCharacterTask
{
	GENERATED_BODY()
private:
	bool CheckForReload();

public:
	virtual void PerformTask(AController* inController) override;
};
