#pragma once
#include "CoreMinimal.h"
#include "Interfaces/Damagable.h"
#include "Interfaces/Team.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interactable.h"
#include "Items/ItemStructs.h"
#include "BaseCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UBaseGameInstance;
class UArmour;
class USphereComponent;
class UItemContainer;
class UWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainersUpdated);

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
	float health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float water = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float hunger = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float rest = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float waterLossRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float hungerLossRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	float restLossRate = 0;
};

UCLASS(config = Game)
class ABaseCharacter : public ACharacter, public IDamagable, public ITeam
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	FOnContainersUpdated OnContainersUpdated;

	UStaticMeshComponent* GetWeaponMeshComp() const { return weaponMeshComp; }
	UPROPERTY()
	UStaticMeshComponent* weaponMeshComp;

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	FCharacterStats GetCurrentStats() const { return currentStats; }
	FCharacterStats GetMaxStats() const { return maxStats; }

	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE UWeapon* GetEquippedWeapon() { return equippedWeapon; }
	void SetEquippedWeapon(UWeapon* weapon);
	UStaticMesh* GetItemMesh(FItemData data);

	void EquipArmour(UArmour* armour);

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

	void Consume(EConsumableType type, int32 value);

	TArray<IInteractable*> GetOverlappingInteractables() const { return overlappingInteractables; }

	void SetOverlappingInteractables(TArray<IInteractable*> inOverlappingInteractables) { this->overlappingInteractables = inOverlappingInteractables; }

	virtual void PossessedBy(AController* NewController) override;

	UItemContainer* GetInventory() const { return inventory; }
	void SetInventory(UItemContainer* inInventory) { inventory = inInventory; }

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ItemAdded(FInstanceItemData inItem);

	UFUNCTION()
	void ItemRemoved(FInstanceItemData inItem);
	int32 GetSlotForGear(EGearType type);
	static const FVector cameraCenter;
	static const FVector leftLean;
	static const FVector rightLean;

protected:
	static float interactionRadius;
	void ResetStats();
	bool inCombat;
	virtual void BeginPlay() override;
	void SetupLoadout(FString loadoutName);

	UFUNCTION()
	void CreateNewItemForInventory(int32 itemID);
	void DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds);

	UPROPERTY()
	USphereComponent* interactionSphere;

	TArray<IInteractable*> overlappingInteractables;

	UPROPERTY()
	UWeapon* equippedWeapon;

	UPROPERTY()
	TMap<EGearType, UArmour*> equippedArmour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Factions, meta = (AllowPrivateAccess = "true"))
	EFaction faction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	FCharacterStats currentStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	UItemContainer* inventory;

	UPROPERTY()
	FCharacterStats maxStats;

	UPROPERTY()
	UBaseGameInstance* gameInstance;
};