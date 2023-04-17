#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "BaseCharacter.h"
#include "BaseGameInstance.h"
#include "Interfaces/Damagable.h"
#include "Interfaces/Team.h"
#include "Items/Weapon.h"

const float ABaseProjectile::Default_Initial_Speed = 5000.0f;
const float ABaseProjectile::Default_Initial_Lifespan = 1.2f;

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::BeginOverlap);
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));

	RootComponent = CollisionComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->CastShadow = false;
	ProjectileMesh->SetWorldScale3D(FVector(0.05));
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetRelativeRotation(FRotator(-90, 0, 0));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = Default_Initial_Speed;
	ProjectileMovement->MaxSpeed = Default_Initial_Speed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	InitialLifeSpan = Default_Initial_Lifespan;
}

float ABaseProjectile::CalculateDamageFalloff()
{
	float damage = healthChange.changeAmount;
	float dist = FVector::Dist(GetActorLocation(), startLoc);	
	int32 range = GetWeaponUsed()->GetWeaponData().range;
	if(dist > range)
	{
		damage *= 1 - ((dist - range) / range);
	}

	return FMath::Max(damage, 1);
}

void ABaseProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(startLoc == FVector::ZeroVector)
	{
		startLoc = GetActorLocation();
	}

	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), GetActorLocation() + (GetActorForwardVector() * 50), ECollisionChannel::ECC_Pawn);
	
	if(hit.IsValidBlockingHit())
	{
		if (hit.GetActor()->Implements<UDamagable>() && Cast<IDamagable>(hit.GetActor())->IsAlive())
		{
			ITeam* hitTeam = Cast<ITeam>(hit.GetActor());

			if (hitTeam->GetRelationship(healthChange.source, mGameInstance()) == ERelationshipType::Enemy) {
				IDamagable* hitActor = Cast<IDamagable>(hit.GetActor());
				healthChange.changeAmount = CalculateDamageFalloff();
				hitActor->ChangeHealth(healthChange);
				Destroy();
			}
		}
		else
		{
			Destroy();
		}
	}
}

//void ABaseProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	ABaseCharacter* us = healthChange.source;
//
//	if (OtherActor != NULL && OtherActor != this && OtherActor != us && us != NULL && OtherComp->GetName().Equals("CollisionCylinder"))
//	{
//		if (OtherActor->Implements<UDamagable>())
//		{
//			ITeam* hitTeam = Cast<ITeam>(OtherActor);
//
//			if (hitTeam->GetRelationship(us, mGameInstance()) == ERelationshipType::Enemy) {
//				IDamagable* hit = Cast<IDamagable>(OtherActor);
//				hit->ChangeHealth(healthChange);
//				Destroy();
//			}
//		}
//		else
//		{
//			Destroy();
//		}
//	}
//}

void ABaseProjectile::SetHealthChange(FHealthChange inHealthChange)
{
	this->healthChange = inHealthChange;
	CollisionComp->IgnoreActorWhenMoving(inHealthChange.source, true);
}