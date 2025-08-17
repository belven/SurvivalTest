#pragma once

#include "CoreMinimal.h"
#include "TaskAction.h"
#include "SurvivalTest/BaseCharacter.h"
#include "AnimationAction.generated.h"

UCLASS()
class SURVIVALTEST_API UAnimationAction : public UTaskAction
{
	GENERATED_BODY()
public:
	static UAnimationAction* CreateAnimationAction(const FString& animationPath, ABaseCharacter* inCharacter);
	static UAnimationAction* CreateAnimationActionTemp( ABaseCharacter* inCharacter, float animationLength);

	virtual void StartAction() override;
	virtual void CancelAction() override;
	virtual void ActionComplete(FStatusData data) override;

	UFUNCTION()
	void AnimationEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void TimerEnded();

protected:
	void SetAnimation(UAnimMontage* inAnimation);

	UPROPERTY()
	ABaseCharacter* character;
	FTimerHandle TimerHandle_PlayAnimation;

	float animation_length = 0;

private:
	UPROPERTY()
	class UAnimMontage* animation = NULL;
};
