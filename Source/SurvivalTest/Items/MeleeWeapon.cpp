#include "MeleeWeapon.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseGameInstance.h"

#define mSphereTraceMultiWeapon(start, end, radius, trace, hits, ignore) UKismetSystemLibrary::SphereTraceMulti(GetWorld(), start, end, radius, trace, true, ignore, EDrawDebugTrace::None, hits, true);

void UMeleeWeapon::UseWeapon(const FVector& LookAtRotation)
{
	if (canAttack) {
		canAttack = false;

		mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UWeapon::AttackComplete, GetWeaponData().useRate);
		TArray<FHitResult> hits;
		TArray<IDamagable*> hitTargets;
		TArray<AActor*> ignore;
		ignore.Add(GetOwner());

		FVector actorLocation = GetOwner()->GetActorLocation();
		FVector actorForwardVector = GetOwner()->GetActorForwardVector();
		actorLocation.Z += GetOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		FVector endLoc = actorLocation + (actorForwardVector * GetWeaponData().range);
		FVector startLoc = actorLocation;
		float dist = FVector::Dist(endLoc, startLoc);
		double radius = GetOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.5;
		mSphereTraceMultiWeapon(startLoc, endLoc, radius, ETraceTypeQuery::TraceTypeQuery3, hits, ignore);

		for (FHitResult hit : hits)
		{
			if (hit.GetActor()->Implements<UDamagable>())
			{
				ITeam* team = Cast<ITeam>(hit.GetActor());

				if (team && team->GetRelationship(GetOwner(), mGameInstance()) == ERelationshipType::Enemy)
				{
					hitTargets.AddUnique(Cast<IDamagable>(hit.GetActor()));
				}
			}
		}

		for (IDamagable* hit : hitTargets)
		{
			FHealthChange change;
			change.changeAmount = GetWeaponData().healthChange;
			change.source = GetOwner();
			change.heals = GetWeaponData().heals;
			hit->ChangeHealth(change);
		}
	}
}

UWorld* UMeleeWeapon::GetWorld() const
{
	return GetOwner()->GetWorld();
}
