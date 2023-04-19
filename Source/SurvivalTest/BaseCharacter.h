#pragma once
#include "CoreMinimal.h"
#include "NavigationInvokerComponent.h"
#include "Interfaces/Damagable.h"
#include "Interfaces/Team.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interactable.h"
#include "Interfaces/ItemContainerInterface.h"
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
class ABaseCharacter : public ACharacter, public IDamagable, public ITeam, public IItemContainerInterface, public IInteractable
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;

	static const FVector cameraCenter;
	static const FVector leftLean;
	static const FVector rightLean;

	UPROPERTY()
	UNavigationInvokerComponent* navInvoker;	

	UCameraComponent* GetBaseCameraComponent() const { return baseCameraComponent; }

#pragma region Combat
	UStaticMeshComponent* GetWeaponMeshComp() const { return weaponMeshComp; }
	FORCEINLINE UWeapon* GetEquippedWeapon() { return equippedWeapon; }
	void SetEquippedWeapon(UWeapon* weapon);
	void EquipArmour(UArmour* armour);

	UStaticMesh* GetItemMesh(FItemData data);

	virtual EFaction GetFaction() override { return faction; }
	void SetFaction(EFaction inFaction) { faction = inFaction; }
#pragma endregion Combat

#pragma region Stats
	virtual void ChangeHealth(FHealthChange& health_change) override;

	float GetDamageAfterResistance(float damage);
	int32 GetDamageResistance();

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
	FOnContainersUpdated OnContainersUpdated;

	virtual UItemContainer* GetItemContainer() override { return inventory; }
	UItemContainer* GetInventory() const { return inventory; }

	void SetInventory(UItemContainer* inInventory) { inventory = inInventory; }

	UBaseGameInstance* GetGame() const { return game; }
	void SetGame(UBaseGameInstance* inGame) { game = inGame; }
	
	UFUNCTION()
	void ItemAdded(FInstanceItemData inItem);

	UFUNCTION()
	void ItemRemoved(FInstanceItemData inItem);
	int32 GetSlotForGear(EGearType type);
	void SetupLoadout(FString loadoutName);
	
#pragma endregion Inventory
	
protected:
	static float interactionRadius;
	void ResetStats();
	bool inCombat;
	virtual void BeginPlay() override;

	UFUNCTION()
	void CreateNewItemForInventory(int32 itemID);
	void DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds);

	UPROPERTY()
		UStaticMeshComponent* weaponMeshComp;

	UPROPERTY()
	USphereComponent* interactionSphere;

	TArray<IInteractable*> overlappingInteractables;

	UPROPERTY()
	UBaseGameInstance* game;

	UPROPERTY()
	UWeapon* equippedWeapon;

	UPROPERTY()
	TMap<EGearType, UArmour*> equippedArmour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Factions, meta = (AllowPrivateAccess = "true"))
	EFaction faction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* baseCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	FCharacterStats currentStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	UItemContainer* inventory;

	UPROPERTY()
	FCharacterStats maxStats;
};
