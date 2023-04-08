#include "TP_WeaponComponent.h"
#include "BaseCharacter.h"
#include "BaseProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "BasePlayerController.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character != nullptr)
	{
		ABasePlayerController* con = Cast<ABasePlayerController>(Character->GetController());

		if (con != NULL && IsValid(con) && con->IsValidLowLevel())
			con->OnUseItem.RemoveDynamic(this, &UTP_WeaponComponent::Fire);
	}
}

void UTP_WeaponComponent::AttachWeapon(ABaseCharacter* TargetCharacter)
{
	//Character = TargetCharacter;
	//if (Character != nullptr && Character->GetController()->IsA(ABasePlayerController::StaticClass()))
	//{
	//	// Attach the weapon to the First Person Character
	//	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//	GetOwner()->AttachToComponent(Character->GetMesh(), AttachmentRules, FName(TEXT("GripPoint")));

	//	ABasePlayerController* con = Cast<ABasePlayerController>(Character->GetController());
	//	// Register so that Fire is called every time the character tries to use the item being held
	//	con->OnUseItem.AddDynamic(this, &UTP_WeaponComponent::Fire);
	//}
}