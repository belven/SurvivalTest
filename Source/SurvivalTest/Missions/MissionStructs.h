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

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		EMissionType type;
};

USTRUCT(BlueprintType)
struct FMissionContainerData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	EMissionType type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	int32 containerID;
};

USTRUCT(BlueprintType)
struct FMissionItemData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		EMissionType type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		int32 itemID;
};

USTRUCT(BlueprintType)
struct FMissionLoadoutData
{
	GENERATED_USTRUCT_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		int32 loadoutID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
		EMissionType type;
};

UCLASS()
class SURVIVALTEST_API UMissionStructs : public UObject
{
	GENERATED_BODY()
	public:
		static EMissionType GetMissionType(const FString& type);

		static FString GetMissionTypeString(EMissionType type)
		{
			switch (type)
			{
			case EMissionType::Medical:
				return "Medical";
			case EMissionType::Military:
				return "Military";
			case EMissionType::Civilian:
				return "Civilian";
			case EMissionType::Secret:
				return "Secret";
			case EMissionType::End:
				return "Unknown";
			}
			return "Unknown";
		}
};
