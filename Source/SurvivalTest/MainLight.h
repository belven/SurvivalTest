#pragma once
#include "CoreMinimal.h"
#include "Engine/Light.h"
#include "MainLight.generated.h"

UCLASS()
class SURVIVALTEST_API AMainLight : public ALight
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
};