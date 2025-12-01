#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerBuildingComponent.generated.h"

class UBaseGameInstance;
class ABaseCharacter;
class ABuildingPart;

UENUM(BlueprintType)
enum class EOneDirection : uint8
{
	North,
	NorthEast,
	NorthWest,
	East,
	West,
	South,
	SouthEast,
	SouthWest,
	End
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALTEST_API UPlayerBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerBuildingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void Build();

	void CreateBuildingPart(ABuildingPart* bp, EOneDirection direction);
	FVector MoveVectorByDirection(const FVector& centerLocation, const FRotator& currentRotation, float distance, EOneDirection direction);

	ABaseCharacter* GetBaseCharacter() const { return baseCharacter; }
	void SetBaseCharacter(ABaseCharacter* inBaseCharacter) { this->baseCharacter = inBaseCharacter; }

	UBaseGameInstance* GetBaseGameInstance();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ABaseCharacter* baseCharacter;

	UPROPERTY()
	UBaseGameInstance* baseGameInstance;			
};