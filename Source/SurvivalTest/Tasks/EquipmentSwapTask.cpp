#include "EquipmentSwapTask.h"

#include "AnimationAction.h"
#include "SwapEquipmentAction.h"

UEquipmentSwapTask::UEquipmentSwapTask() : Super()
{
	SetTaskName("Swapping Equipment");
}

void UEquipmentSwapTask::PerformTask(AController* inController)
{
	Super::PerformTask(inController);

	if (animationAction == NULL) {
		// TODO change to get and use the animation from the weapon

		// UAnimMontage* anim = LoadObject<UAnimMontage>(weapon->GetRangedWeaponData().animation);
		// animationAction = UAnimationAction::CreateAnimationAction(weapon->GetRangedWeaponData().weaponSwapAnimation, GetCharacter());
		animationAction = UAnimationAction::CreateAnimationActionTemp(GetCharacter(), 1.5);
	}
	else {
		//UAnimMontage* anim = LoadObject<UAnimMontage>(weapon->GetRangedWeaponData().weaponSwapAnimation);
		animationAction->SetAnimationLength(1.5);
	}

	if (!swapAction)
	{
		swapAction = USwapEquipmentAction::CreateSwapEquipmentAction(GetCharacter(), slot);
	}
	else {
		swapAction->SetSlot(slot);
	}

	AddAction(animationAction);
	AddAction(swapAction);

	//UE_LOG(LogTemp, Log, TEXT("EquipmentSwapTask started"));
	PerformNextAction();
}
