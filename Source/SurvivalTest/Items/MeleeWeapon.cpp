#include "MeleeWeapon.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/HelperFunctions.h"

#define mSphereTraceMultiWeapon(start, end, radius, trace, hits, ignore) UKismetSystemLibrary::SphereTraceMulti(GetWorld(), start, end, radius, trace, true, ignore, EDrawDebugTrace::None, hits, true);

void UMeleeWeapon::UseWeapon(const FRotator& LookAtRotation)
{
	if (canAttack) {
		canAttack = false;

		mSetTimerWorld(characterOwner->GetWorld(), TimerHandle_ShotTimerExpired, &UWeapon::AttackComplete, GetWeaponData().useRate);
		TArray<FHitResult> hits;
		TArray<IDamagable*> hitTargets;
		TArray<AActor*> ignore;
		ignore.Add(GetCharacterOwner());

		FVector actorLocation = GetCharacterOwner()->GetActorLocation();
		FVector actorForwardVector = GetCharacterOwner()->GetActorForwardVector();
		actorLocation.Z += GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		float scaledCapsuleRadius = GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius();

		FVector startLoc = actorLocation + (actorForwardVector * scaledCapsuleRadius);
		FVector endLoc = startLoc + (actorForwardVector * GetWeaponData().range);
		double radius = scaledCapsuleRadius * 1.5;
		mSphereTraceMultiWeapon(startLoc, endLoc, radius, ETraceTypeQuery::TraceTypeQuery3, hits, ignore);
		
		for (FHitResult hit : hits)
		{
			if (hit.GetComponent()->GetClass() != USphereComponent::StaticClass() &&  hit.GetActor()->Implements<UDamagable>())
			{
				ITeam* team = Cast<ITeam>(hit.GetActor());

				if (team && team->GetRelationship(GetCharacterOwner(), mGameInstance()) == ERelationshipType::Enemy)
				{
					hitTargets.AddUnique(Cast<IDamagable>(hit.GetActor()));
				}
			}
		}

		for (IDamagable* hit : hitTargets)
		{
			if (hit->IsAlive()) {
				FHealthChange change;
				change.changeAmount = GetWeaponData().healthChange;
				change.source = GetCharacterOwner();
				change.heals = GetWeaponData().heals;
				hit->ChangeHealth(change);
			}
		}
	}
}

UWorld* UMeleeWeapon::GetWorld() const
{
	return GetCharacterOwner()->GetWorld();
}
