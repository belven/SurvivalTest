#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

class USplineComponent;
UCLASS(HideCategories = ("Rendering", "Replication", "Collision", "HLOD", "World_Partition", "Input", "Replication", "Actor", "Cooking", "Data_Layers"))
class SURVIVALTEST_API APatrolPath : public AActor
{
	GENERATED_BODY()
public:
	APatrolPath();
	
protected:
	virtual void BeginPlay() override;

public:
	FORCEINLINE USplineComponent* GetSpline() const { return spline; }
private:

	UPROPERTY(EditAnywhere)
		USplineComponent* spline;
};