#include "TaskAction.h"

void UTaskAction::StartAction()
{

}

void UTaskAction::CancelAction()
{

}

void UTaskAction::ActionComplete(FStatusData data)
{
	OnActionComplete.Broadcast(data);
}
