#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "ArmourCreator.generated.h"

class UArmour;

UCLASS()
class SURVIVALTEST_API UArmourCreator : public UObject
{
	GENERATED_BODY()
public:
	static UArmour* GetOrCreateArmour(UBaseGameInstance* game, const FInstanceItemData& instanceItemData);
	static void CreateArmourData(UBaseGameInstance* game, const FInstanceItemData& instanceItemData, const FItemData& itemData);
	static void CreateArmourData(UBaseGameInstance* game, const FInstanceItemData& instanceItemData, const FItemData& itemData, FArmourData& armourData, FInstanceContainerData& inInstanceContainerData, FInstanceArmourData& inInstanceArmourData);
};
