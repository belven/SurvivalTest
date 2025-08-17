#pragma once
#include "CoreMinimal.h"
#include "TaskAction.generated.h"

USTRUCT(BlueprintType)
struct FStatusData
{
	GENERATED_USTRUCT_BODY()
	bool successful;

	FStatusData() : successful(false) {}

	FStatusData(bool inSuccessful) : successful(inSuccessful) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionComplete, FStatusData, status);

UCLASS()
class SURVIVALTEST_API UTaskAction : public UObject
{
	GENERATED_BODY()

public:
	FActionComplete OnActionComplete;

	bool CanBeInterrupted() { return canBeInterrupted; }

	virtual void StartAction();
	virtual void CancelAction();
	virtual void ActionComplete(FStatusData data);

private:
	bool canBeInterrupted;

	UPROPERTY()
	AController* controller;
};