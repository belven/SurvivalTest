#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "UObject/NoExportTypes.h"
#include "ArmourCreator.generated.h"

class UArmour;

UCLASS()
class SURVIVALTEST_API UArmourCreator : public UObject
{
	GENERATED_BODY()
public:
	static UArmour* CreateArmour(int32 itemID, UWorld* world, int32 instanceItemDataID);
	static void CreateArmourData(int32 itemID, UBaseGameInstance* game, UArmour* armour, int32 instanceItemDataID);
};
