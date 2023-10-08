#pragma once
#include "CoreMinimal.h"
#include "NavigationInvokerComponent.h"
#include "Interfaces/Damagable.h"
#include "Interfaces/Team.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interactable.h"
#include "Interfaces/ItemContainerInterface.h"
#include "CharacterStructs.h"
#include "Items/Inventory.h"
#include "BaseCharacter.generated.h"

class UCameraComponent;
class UBaseGameInstance;
class USphereComponent;
class UInventory;
class UWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquipped, UWeapon*, oldWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyHit, ABaseCharacter*, enemy);

UCLASS(config = Game)
class ABaseCharacter : public ACharacter, public IDamagable, public ITeam, public IItemContainerInterface, public IInteractable
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	void StopSprinting();
	void StartSprinting();
	void SetMovementSpeed(EMovementState state);
	void SetMovementState(EMovementState inState);
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	void CalculateSprint(float DeltaSeconds);
	void EnemyHit(ABaseCharacter* inActor);

	static const FVector cameraCenter;
	static const FVector leftLean;
	static const FVector rightLean;
	static float interactionRadius;

	float baseWalkSpeed;
	EMovementState currentMovementState;

	UPROPERTY()
	UNavigationInvokerComponent* navInvoker;

	UCameraComponent* GetBaseCameraComponent() const { return baseCameraComponent; }

#pragma region Combat
	FORCEINLINE UWeapon* GetEquippedWeapon() { return GetInventory() ? GetInventory()->GetEquippedWeapon() : nullptr; }

	UStaticMesh* GetItemMesh(FItemData data);

	FWeaponEquipped OnWeaponEquipped;
	FCharacterDied OnCharacterDied;
	FEnemyHit OnEnemyHit;

	virtual EFaction GetFaction() override { return faction; }
	void SetFaction(EFaction inFaction) { faction = inFaction; }
#pragma endregion Combat

#pragma region Stats
	virtual void ChangeHealth(FHealthChange& health_change) override;

	float GetDamageAfterResistance(float damage);
	int32 GetDamageResistance();

	bool IsSprinting() { return isRequestingSprint; }

	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() override { return currentStats.health < 1; };

	virtual float GetCurrentHealth() override { return currentStats.health; }
	virtual float GetMaxHealth() override { return maxStats.health; }

	FCharacterStats GetCurrentStats() const { return currentStats; }
	FCharacterStats GetMaxStats() const { return maxStats; }

	void Consume(EConsumableType type, int32 value);
#pragma endregion Stats

#pragma region Interactables
	void GetOverlapsOnSpawn();
	void AddInteractable(IInteractable* inter);
	void RemoveInteractable(IInteractable* inter);
	TArray<IInteractable*> GetOverlappingInteractables() const { return overlappingInteractables; }
	void SetOverlappingInteractables(TArray<IInteractable*> inOverlappingInteractables) { this->overlappingInteractables = inOverlappingInteractables; }
	virtual void Interact(ABasePlayerController* instigator) override;
	virtual void Highlight(bool activate) override;

	UFUNCTION(BlueprintCallable)
	int32 GetNearbyContainersNum() { return overlappingInteractables.Num(); }

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma endregion Interactables

#pragma region Inventory
	virtual UItemContainer* GetItemContainer() override { return inventory; }

	UInventory* GetInventory() const { return inventory; }
	void SetInventory(UInventory* inInventory) { inventory = inInventory; }

	void SetupLoadout(FString loadoutName);

	UStaticMeshComponent* GetWeaponMeshComp() const
	{
		return weaponMeshComp;
	}

#pragma endregion Inventory

	UBaseGameInstance* GetGame() const { return game; }
	void SetGame(UBaseGameInstance* inGame) { game = inGame; }

protected:
	float timeMoved;
	bool inCombat;
	bool isRequestingSprint;

	void ResetStats();
	virtual void BeginPlay() override;
	void DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds);

	UPROPERTY()
	UStaticMeshComponent* weaponMeshComp;

	UPROPERTY()
	USphereComponent* interactionSphere;

	TArray<IInteractable*> overlappingInteractables;

	UPROPERTY()
	UBaseGameInstance* game;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Factions, meta = (AllowPrivateAccess = "true"))
	EFaction faction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* baseCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	FCharacterStats currentStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	UInventory* inventory;

	UPROPERTY()
	FCharacterStats maxStats;
};
