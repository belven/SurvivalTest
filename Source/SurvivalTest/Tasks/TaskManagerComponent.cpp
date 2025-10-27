#include "TaskManagerComponent.h"
#include "SurvivalTest/BaseCharacter.h"

UTaskManagerComponent::UTaskManagerComponent() : controller(nullptr), currentTask(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTaskManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	GetController();
}

// void UTaskManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
// }

bool UTaskManagerComponent::PerformTask(UCharacterTask* newTask, bool force)
{
	FScopeLock Lock(&PerformTaskLock);

	bool result = false;

	if (!currentTask)
	{
		//UE_LOG(LogTemp, Log, TEXT("New Task Started, no existing Task"));
		currentTask = newTask;
		currentTask->OnTaskComplete.AddUniqueDynamic(this, &UTaskManagerComponent::TaskComplete);
		currentTask->PerformTask(GetController());
		result = true;
	}
	else if (currentTask->GetClass() != newTask->GetClass())
	{
		if (currentTask->CanBeInterrupted() || force)
		{
			//UE_LOG(LogTemp, Log, TEXT("New Task Started, Existing Task interrupted"));
			currentTask->CancelAction(force);
			currentTask->OnTaskComplete.RemoveAll(this);

			currentTask = newTask;
			currentTask->OnTaskComplete.AddUniqueDynamic(this, &UTaskManagerComponent::TaskComplete);
			currentTask->PerformTask(GetController());
			result = true;
		}
		else
		{
			//UE_LOG(LogTemp, Log, TEXT("New Task Queued"));
			newTask->OnTaskComplete.AddUniqueDynamic(this, &UTaskManagerComponent::TaskComplete);
			tasks.Enqueue(newTask);
			result = true;
		}
	}
	return result;
}

AController* UTaskManagerComponent::GetController()
{
	if (controller == NULL)
	{
		controller = Cast<ABaseCharacter>(GetOwner())->GetController();
	}
	return controller;
}

void UTaskManagerComponent::TaskComplete(const FStatusData& status)
{
	if (!tasks.IsEmpty())
	{
		//UE_LOG(LogTemp, Log, TEXT("Task Completed, next task in queue started"));
		currentTask->OnTaskComplete.RemoveAll(this);

		tasks.Dequeue(currentTask);
		currentTask->PerformTask(GetController());
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("All Tasks Completed"));
		currentTask = NULL;
	}
}