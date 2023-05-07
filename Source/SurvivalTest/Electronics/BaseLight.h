#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalTest/MainLight.h"
#include "BaseLight.generated.h"

class UBaseGameInstance;
class AMainLight;

UCLASS()
class SURVIVALTEST_API ABaseLight : public AActor
{
	GENERATED_BODY()

public:
	ABaseLight();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Light")
	void TimeChanged(AMainLight* light);

protected:
	virtual void BeginPlay() override;

	bool bound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float startingLightLevel;

	UPROPERTY()
		UBaseGameInstance* game;
};
