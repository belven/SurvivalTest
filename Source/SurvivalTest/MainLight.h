#pragma once
#include "CoreMinimal.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Light.h"
#include "MainLight.generated.h"

class UArrowComponent;
class UDirectionalLightComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeTrigger, AMainLight*, mainLight);

UCLASS()
class SURVIVALTEST_API AMainLight : public ALight
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AMainLight(const FObjectInitializer& ObjectInitializer);

	FOnTimeTrigger OnTimeTransition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float lightLevel;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 dayLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float minLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> timeTriggers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float lastTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float totalTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float timeOfDay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator startRotation = FRotator(-45,0,0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator endRotation = FRotator(-45, 180, 0);

	UPROPERTY()
		TObjectPtr<UArrowComponent> ArrowComponent;

	/* EditorOnly reference to the light component to allow it to be displayed in the details panel correctly */
	UPROPERTY(VisibleAnywhere, Category = "Light")
		TObjectPtr<UDirectionalLightComponent> DirectionalLightComponent;
	
	// UObject Interface
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void LoadedFromAnotherClass(const FName& OldClassName) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
#endif
	
#if WITH_EDITORONLY_DATA
	/** Returns ArrowComponent subobject **/
	UArrowComponent* GetArrowComponent() const { return ArrowComponent; }

	/** Returns SkyAtmosphereComponent subobject */
	UDirectionalLightComponent* GetComponent() const { return DirectionalLightComponent; }
#endif

private:

	UPROPERTY()
		UCurveFloat* lightCurve;
};