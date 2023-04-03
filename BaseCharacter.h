#pragma once
#include "CoreMinimal.h"
#include "Damagable.h"
#include "Team.h"
#include "GameFramework/Character.h"
#include "Items/Weapon.h"
#include "Interactable.h"
#include "BaseCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class USurvivalGameInstance;
class UArmour;
class USphereComponent;

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_USTRUCT_BODY()
public:
	void CopyStats(FCharacterStats other)
	{
		health = other.health;
		water = other.water;
		hunger = other.hunger;
		rest = other.rest;
		waterLossRate = other.waterLossRate;
		hungerLossRate = other.hungerLossRate;
		restLossRate = other.restLossRate;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float water;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float hunger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float rest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float waterLossRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float hungerLossRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float restLossRate;
};

UCLASS(config = Game)
class ABaseCharacter : public ACharacter, public IDamagable, public ITeam
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);
	ABaseCharacter();

	//USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	FCharacterStats GetCurrentStats() const { return currentStats; }
	FCharacterStats GetMaxStats() const { return maxStats; }

	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE UWeapon* GetEquippedWeapon() { return equippedWeapon; }
	FORCEINLINE void SetEquippedWeapon(UWeapon* weapon) { equippedWeapon = weapon; equippedWeapon->SetOwner(this); }
	void EquipArmour(UArmour* armour);

	USurvivalGameInstance* GetBaseGameInstance();

	virtual void ChangeHealth(FHealthChange& health_change) override;
	virtual bool IsDead() override { return currentStats.health <= 0; };
	virtual float GetCurrentHealth() override { return currentStats.health; }
	virtual float GetMaxHealth() override { return maxStats.health; }
	virtual EFaction GetFaction() override { return faction; }
	void SetFaction(EFaction inFaction) { faction = inFaction; }
	float GetDamageAfterResistance(float damage);
	int32 GetDamageResistance();
	void GetOverlapsOnSpawn();
	void AddInteractable(IInteractable* inter);
	void RemoveInteractable(IInteractable* inter);

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;
	void SetupLoadout();
	void DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds);

	UPROPERTY()
		USphereComponent* interactionSphere;

	TArray<IInteractable*> overlappingInteractables;

	UPROPERTY()
		UWeapon* equippedWeapon;

	UPROPERTY()
		TMap<EArmourSlot, UArmour*> equippedArmour;

	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Factions, meta = (AllowPrivateAccess = "true"))
		EFaction faction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		FCharacterStats currentStats;

	bool inCombat;

	UPROPERTY()
		FCharacterStats maxStats;
	UPROPERTY()
		USurvivalGameInstance* gameInstance;
};