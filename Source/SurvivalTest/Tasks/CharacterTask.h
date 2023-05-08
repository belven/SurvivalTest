#pragma once
#include "CoreMinimal.h"
#include "TaskAction.h"
#include "UObject/NoExportTypes.h"
#include "CharacterTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskComplete, FStatusData, status);

UCLASS()
class SURVIVALTEST_API UCharacterTask : public UObject
{
	GENERATED_BODY()

public:
	FTaskComplete OnTaskComplete;
	
	virtual void PerformTask(AController* inController);
	void PerformNextAction();
	
	void AddAction(UTaskAction* action);	
	TArray<UTaskAction*>& GetActions() { return actions; }

	void ActionComplete(FStatusData actionStatus);
	bool CancelAction(bool force = false);

private:
	UPROPERTY()
	AController* controller;

	UPROPERTY()
	UTaskAction* currentAction;

	UPROPERTY()
	TArray<UTaskAction*> actions;
};