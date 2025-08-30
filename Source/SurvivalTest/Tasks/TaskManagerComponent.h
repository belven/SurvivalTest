// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterTask.h"
#include "TaskManagerComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SURVIVALTEST_API UTaskManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTaskManagerComponent();

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	bool PerformTask(UCharacterTask* newTask, bool force);
	AController* GetController();

	UFUNCTION()
	void TaskComplete(const FStatusData& status);

protected:
	UPROPERTY()
	AController* controller;

	FCriticalSection PerformTaskLock;

	TArrayQueue<UCharacterTask*> tasks;

	UPROPERTY()
	UCharacterTask* currentTask;

	// Called when the game starts
	virtual void BeginPlay() override;
};