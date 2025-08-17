#include "CharacterTask.h"

void UCharacterTask::PerformTask(AController* inController)
{
	controller = inController;
	PerformNextAction();
}

void UCharacterTask::PerformNextAction()
{
	 actions.Dequeue(currentAction);
	currentAction->StartAction();
}

void UCharacterTask::AddAction(UTaskAction* action)
{
	actions.Enqueue(action);
	action->OnActionComplete.AddUniqueDynamic(this, &UCharacterTask::ActionComplete);
}

void UCharacterTask::ActionComplete(FStatusData actionStatus)
{
	if (!actionStatus.successful)
	{
		OnTaskComplete.Broadcast(actionStatus);
	}
	else if (actions.Num() > 1)
	{
		PerformNextAction();
	}
	else
	{
		OnTaskComplete.Broadcast(FStatusData(true));
	}
}

bool UCharacterTask::CancelAction(bool force)
{
	bool result = false;
	if (currentAction->CanBeInterrupted() || force)
	{
		currentAction->CancelAction();
		result = true;
	}
	return result;
}