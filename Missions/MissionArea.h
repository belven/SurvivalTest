#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include "MissionArea.generated.h"

UCLASS()
class SURVIVALTEST_API AMissionArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AMissionArea();
	virtual void Tick(float DeltaTime) override;

	UBoxComponent* GetBox() const	{		return box;	}
protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY()
	UBoxComponent* box;
};