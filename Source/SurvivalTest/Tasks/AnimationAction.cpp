#include "AnimationAction.h"

UAnimationAction* UAnimationAction::CreateAnimationAction(const FString& animationPath, ABaseCharacter* inCharacter)
{
	UAnimationAction* action = NewObject<UAnimationAction>();
	action->SetAnimation(LoadObject<UAnimMontage>(action, *animationPath));
	action->character = inCharacter;
	return action;
}

UAnimationAction* UAnimationAction::CreateAnimationActionTemp(ABaseCharacter* inCharacter, float animationLength)
{
	UAnimationAction* action = NewObject<UAnimationAction>();
	action->character = inCharacter;
	action->animation_length = animationLength;
	return action;	
}

void UAnimationAction::StartAction()
{
	character->GetWorldTimerManager().SetTimer(TimerHandle_PlayAnimation, this, &UAnimationAction::TimerEnded, animation_length);
	//UE_LOG(LogTemp, Log, TEXT("Animation Action Started"));

	//UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance();
	//AnimInstance->Montage_Play(animation);
}

void UAnimationAction::CancelAction()
{
	//UE_LOG(LogTemp, Log, TEXT("Animation Action Cancelled"));
	character->GetWorldTimerManager().ClearAllTimersForObject(this);
	//UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance();
	//AnimInstance->StopSlotAnimation();
}

void UAnimationAction::ActionComplete(FStatusData data)
{
	Super::ActionComplete(data);
	//UE_LOG(LogTemp, Log, TEXT("Animation Action Completed"));
}

void UAnimationAction::AnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//UE_LOG(LogTemp, Log, TEXT("Animation Action AnimationEnded"));
	ActionComplete(FStatusData(!bInterrupted));
}

void UAnimationAction::TimerEnded()
{
	//UE_LOG(LogTemp, Log, TEXT("Animation Action TimerEnded"));
	ActionComplete(FStatusData(true));
}

void UAnimationAction::SetAnimation(UAnimMontage* inAnimation)
{
	if (inAnimation != NULL)
	{
		animation = inAnimation;
		UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance();
		AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &UAnimationAction::AnimationEnded);
	}
}
