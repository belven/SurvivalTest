#pragma once
#include "CoreMinimal.h"
#include "TaskAction.h"
#include "UObject/NoExportTypes.h"
#include "CharacterTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskComplete, FStatusData, status);

template<typename T>
class TArrayQueue
{
public:
    void Enqueue(const T& Item)
    {
        Data.Add(Item);
    }

    bool Dequeue(T& OutItem)
    {
        if (HeadIndex >= Data.Num())
        {
            return false; // Empty
        }

        OutItem = Data[HeadIndex];
        HeadIndex++;

        // Optional cleanup: if head index gets too far ahead, compact the array
        if (HeadIndex > 32 && HeadIndex > Data.Num() / 2)
        {
            Data.RemoveAt(0, HeadIndex, EAllowShrinking::Yes);
            HeadIndex = 0;
        }

        return true;
    }

    bool IsEmpty() const
    {
        return HeadIndex >= Data.Num();
    }

    int32 Num() const
    {
        return Data.Num() - HeadIndex;
    }

private:
    TArray<T> Data;
    int32 HeadIndex = 0;
};

UCLASS()
class SURVIVALTEST_API UCharacterTask : public UObject
{
	GENERATED_BODY()

public:
	FTaskComplete OnTaskComplete;
	
	virtual void PerformTask(AController* inController);
	void PerformNextAction();
	
	void AddAction(UTaskAction* action);	
    TArrayQueue<UTaskAction*>& GetActions() { return actions; }

    UFUNCTION()
	void ActionComplete(FStatusData actionStatus);

	bool CancelAction(bool force = false);

private:
	UPROPERTY()
	AController* controller;

	UPROPERTY()
	UTaskAction* currentAction;

    TArrayQueue<UTaskAction*> actions;
};
