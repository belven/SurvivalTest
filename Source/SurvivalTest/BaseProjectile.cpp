#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "BaseCharacter.h"
#include "BaseGameInstance.h"
#include "BasePlayerController.h"
#include "Interfaces/Damagable.h"
#include "Interfaces/Team.h"
#include "Items/Weapon.h"

const float ABaseProjectile::Default_Initial_Speed = 8000.0f;
const float ABaseProjectile::Default_Initial_Lifespan = 20.0f;

ABaseProjectile::ABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	RootComponent = CollisionComp;

	// Only visual
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->CastShadow = false;
	ProjectileMesh->SetWorldScale3D(FVector(0.05));
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // purely visual
	ProjectileMesh->SetRelativeRotation(FRotator(-90, 0, 0));

	// Overlap handling
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlap);

	// Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = Default_Initial_Speed;
	ProjectileMovement->MaxSpeed = 60000;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 3.0f;

	InitialLifeSpan = Default_Initial_Lifespan;
}

float ABaseProjectile::CalculateDamageFalloff()
{
	float damage = healthChange.changeAmount;
	float dist = FVector::Dist(GetActorLocation(), startLoc);
	int32 range = GetWeaponUsed()->GetWeaponData().range;
	if (dist > range)
	{
		damage *= 1 - ((dist - range) / range);
	}

	return FMath::Max(damage, 1);
}

void ABaseProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherComp && healthChange.source)
	{
		if (OtherActor->Implements<UDamagable>())
		{
			ITeam* hitTeam = Cast<ITeam>(OtherActor);
			if (hitTeam && hitTeam->GetRelationship(healthChange.source, mGameInstance()) == ERelationshipType::Enemy)
			{
				IDamagable* hitTarget = Cast<IDamagable>(OtherActor);

				if (hitTarget->IsAlive()) {
					healthChange.changeAmount = CalculateDamageFalloff();
					hitTarget->ChangeHealth(healthChange);
				}
				Destroy();
			}
		}
		else
		{
			Destroy();
		}
	}
}

void ABaseProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (startLoc == FVector::ZeroVector)
	{
		startLoc = GetActorLocation();
	}

	FVector start = GetActorLocation() - (GetActorForwardVector() * 100);
	FVector end = GetActorLocation() + (GetActorForwardVector() * 100);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 5);
}

void ABaseProjectile::SetHealthChange(FHealthChange inHealthChange)
{
	this->healthChange = inHealthChange;
	CollisionComp->IgnoreActorWhenMoving(inHealthChange.source, true);
}