#include "ReloadTask.h"
#include "AnimationAction.h"
#include "ReloadAction.h"
#include "SurvivalTest/Items/ProjectileWeapon.h"
#include "SurvivalTest/Items/Weapon.h"

bool UReloadTask::CheckForReload()
{
	bool result = false;

	ABaseCharacter* character = GetCharacter();

	if (character) {
		UWeapon* equippedWeapon = character->GetEquippedWeapon();

		if (equippedWeapon && equippedWeapon->IsProjectileWeapon())
		{
			UProjectileWeapon* weapon = Cast<UProjectileWeapon>(equippedWeapon);

			if (weapon->NeedsReloading())
			{
				int32 ammoLeft = character->GetInventory()->GetItemAmount(weapon->GetProjectileWeaponData().ammoID);

				if (ammoLeft > 0)
				{
					result = true;
				}
			}
		}
	}
	return result;
}

void UReloadTask::PerformTask(AController* inController)
{
	Super::PerformTask(inController);

	//UE_LOG(LogTemp, Log, TEXT("ReloadTask PerformTask"));

	if (CheckForReload())
	{
		//UE_LOG(LogTemp, Log, TEXT("CheckForReload Passed"));

		ABaseCharacter* character = GetCharacter();
		UWeapon* equippedWeapon = character->GetEquippedWeapon();
		UProjectileWeapon* weapon = Cast<UProjectileWeapon>(equippedWeapon);

		if (!animationAction) {
			// TODO change to get and use the animation from the weapon

			// UAnimMontage* anim = LoadObject<UAnimMontage>(weapon->GetRangedWeaponData().animation);
			// animationAction = UAnimationAction::CreateAnimationAction(weapon->GetRangedWeaponData().animation, GetCharacter());
			animationAction = UAnimationAction::CreateAnimationActionTemp(GetCharacter(), weapon->GetProjectileWeaponData().reloadSpeed);
		}
		else {
			//UAnimMontage* anim = LoadObject<UAnimMontage>(weapon->GetRangedWeaponData().animation);
			animationAction->SetAnimationLength(weapon->GetProjectileWeaponData().reloadSpeed);
		}

		if (!reloadAction) {
			reloadAction = UReloadAction::CreateReloadAction(character);
		}

		AddAction(animationAction);
		AddAction(reloadAction);

		weapon->Reload();

		//UE_LOG(LogTemp, Log, TEXT("ReloadTask started"));
		PerformNextAction();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("CheckForReload Failed"));
		OnTaskComplete.Broadcast(FStatusData(false));
	}
}