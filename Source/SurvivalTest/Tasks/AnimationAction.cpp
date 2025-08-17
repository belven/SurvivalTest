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
	Super::StartAction();
	character->GetWorldTimerManager().SetTimer(TimerHandle_PlayAnimation, this, &UAnimationAction::TimerEnded, animation_length);

	//UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance();
	//AnimInstance->Montage_Play(animation);
}

void UAnimationAction::CancelAction()
{
	Super::CancelAction();
	//UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance();
	//AnimInstance->StopSlotAnimation();
}

void UAnimationAction::ActionComplete(FStatusData data)
{
	Super::ActionComplete(data);

}

void UAnimationAction::AnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	FStatusData data;
	data.successful = !bInterrupted;
	ActionComplete(data);
}

void UAnimationAction::TimerEnded()
{
	FStatusData data;
	data.successful = true;
	ActionComplete(data);
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
