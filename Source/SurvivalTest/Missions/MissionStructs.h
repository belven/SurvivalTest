#pragma once
#include "CoreMinimal.h"
#include "MissionStructs.generated.h"

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	Medical,
	Military,
	Civilian,
	Secret,
	End
};

USTRUCT(BlueprintType)
struct FMissionData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		EMissionType type;
};

USTRUCT(BlueprintType)
struct FMissionLoadoutData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		EMissionType type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		FString name;
};

UCLASS()
class SURVIVALTEST_API UMissionStructs : public UObject
{
	GENERATED_BODY()
	public:
		static EMissionType GetMissionType(FString type);
};
