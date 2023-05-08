#include "CharacterTask.h"

void UCharacterTask::PerformTask(AController* inController)
{
	controller = inController;
	PerformNextAction();
}

void UCharacterTask::PerformNextAction()
{
	currentAction = actions.Pop();
	currentAction->StartAction();
}

void UCharacterTask::AddAction(UTaskAction* action)
{
	actions.Add(action);
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
	if (currentAction->CanBeInterrupted() || force)
	{
		currentAction->Cancel();
		return true;
	}
	return false;
}