#pragma once
#include "CoreMinimal.h"
#include "PowerSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSystemAdded, APowerSystem*, powerSystem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSystemRemoved, APowerSystem*, powerSystem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPowerSystemActiveChanged, APowerSystem*, powerSystem);

UCLASS()
class SURVIVALTEST_API APowerSystem : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual float GetTotalPowerProvided();
	UFUNCTION()
	float GetTotalPowerConsumed();
	UFUNCTION()
	void DetermineActiveState();
	UFUNCTION()
	void SystemAdded(APowerSystem* system);
	UFUNCTION()
	void SystemRemoved(APowerSystem* system);
	UFUNCTION()
	void SystemActiveChanged(APowerSystem* system);
	UFUNCTION()
	void AddPowerSystem(APowerSystem* system);
	UFUNCTION()
	void RemovePowerSystem(APowerSystem* system);

	virtual bool IsActive() { return isActive; }
	void SetActive(bool active);
	void ToggleActive() { SetActive(!isActive); }

	virtual bool IsAutomatic() { return isAutomatic; }
	virtual bool ProvidesPower() { return providesPower; }
	virtual bool ConsumesPower() { return consumesPower; }

	virtual float GetPowerProvided() { return powerProvided; }
	virtual float GetPowerConsumed() { return powerConsumed; }
	virtual TArray<APowerSystem*>& GetPowerSystems() { return systems; }
	
	FPowerSystemAdded OnPowerSystemAdded;
	FPowerSystemRemoved OnPowerSystemRemoved;
	FPowerSystemActiveChanged OnPowerSystemActiveChanged;

private:
	bool providesPower = false;
	bool consumesPower = false;
	bool isActive = false;
	bool isAutomatic = true;
	
	float powerConsumed = 0.0f;
	float powerProvided = 0.0f;

	UPROPERTY()
	TArray<APowerSystem*> systems;
};