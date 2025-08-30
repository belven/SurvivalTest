#include "CharacterTask.h"

void UCharacterTask::PerformTask(AController* inController)
{
	controller = inController;
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
	else if (!actions.IsEmpty())
	{
		PerformNextAction();
	}
	else
	{
		OnTaskComplete.Broadcast(FStatusData(true));
	}
}

bool UCharacterTask::CanBeInterrupted()
{
	return !currentAction || currentAction->CanBeInterrupted();
}

bool UCharacterTask::CancelAction(bool force)
{
	bool result = false;
	if (CanBeInterrupted() || force)
	{
		if (currentAction) {
			currentAction->CancelAction();
			currentAction->OnActionComplete.RemoveAll(this);
		}

		if (!actions.IsEmpty())
		{
			for (UTaskAction* action : actions.GetData())
			{
				action->OnActionComplete.RemoveAll(this);
			}
		}
		result = true;
	}
	return result;
}

ABaseCharacter* UCharacterTask::GetCharacter()
{
	ACharacter* src = controller->GetCharacter();

	if (src) {
		return Cast<ABaseCharacter>(src);
	}
	return nullptr;
}
