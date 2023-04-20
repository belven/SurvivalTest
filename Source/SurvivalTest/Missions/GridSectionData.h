#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridSectionData.generated.h"

USTRUCT(BlueprintType)
struct FGridSectionData
{
	GENERATED_USTRUCT_BODY()
	FGridSectionData() : centerLocation(0),
	                 sectionSize(0)
	{
	}

	FGridSectionData(const FVector& inCenterLocation, int32 inSectionSize)
		: centerLocation(inCenterLocation),
		  sectionSize(inSectionSize)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid")
	FVector centerLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Grid")
	int32 sectionSize;

	FVector CornerLocation()
	{
		float xY =  sectionSize / 2;
		return FVector(centerLocation.X - xY, centerLocation.Y - xY, 0);
	}
};

UCLASS()
class SURVIVALTEST_API AGridSection : public AActor
{
	GENERATED_BODY()

public:
	AGridSection();

	static AGridSection* CreateGridSection(UWorld* world, FGridSectionData data);

	//UStaticMeshComponent* GetGridSectionComp() const { return gridSectionComp; }
	//void SetGridSectionComp(UStaticMeshComponent* inGridSectionComp) { gridSectionComp = inGridSectionComp; }

	FGridSectionData GetSectionData() const { return sectionData; }

	void SetSectionData(FGridSectionData inSectionData) { sectionData = inSectionData; }
	void HighlightSection(float time);

protected:
	virtual void BeginPlay() override;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LootBox, meta = (AllowPrivateAccess = "true"))
	//UStaticMeshComponent* gridSectionComp;

	FGridSectionData sectionData;
};