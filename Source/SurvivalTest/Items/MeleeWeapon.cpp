#include "MeleeWeapon.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseGameInstance.h"

#define mSphereTraceMulti(start, end, radius, hits, ignore) UKismetSystemLibrary::SphereTraceMulti(GetWorld(), start, end, radius, ETraceTypeQuery::TraceTypeQuery_MAX, true, ignore, EDrawDebugTrace::ForOneFrame, hits, true);

void UMeleeWeapon::UseWeapon(const FVector& LookAtRotation)
{
	TArray<FHitResult> hits;
	TArray<IDamagable*> hitTargets;
	TArray<AActor*> ignore;
	ignore.Add(GetOwner());

	FVector endLoc = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * GetWeaponData().range);
	FVector startLoc = GetOwner()->GetActorLocation();
	float dist = FVector::Dist(endLoc, startLoc);
	double radius = GetOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.5;

	mSphereTraceMulti(startLoc, endLoc, radius, hits, ignore);

	for(FHitResult hit : hits)
	{
		if(hit.GetActor()->Implements<UDamagable>())
		{
			ITeam* team = Cast<ITeam>(hit.GetActor());

			if(team && team->GetRelationship(GetOwner(), mGameInstance()) == ERelationshipType::Enemy)
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