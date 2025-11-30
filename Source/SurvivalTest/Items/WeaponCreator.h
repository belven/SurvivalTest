#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "WeaponCreator.generated.h"

#define mNewObject(uClass) NewObject<uClass>() 

class UWeapon;
UCLASS()
class SURVIVALTEST_API UWeaponCreator : public UObject
{
	GENERATED_BODY()
public:
		static UWeapon* CreateWeapon(UBaseGameInstance* gameIn, const FInstanceItemData& inInstanceItemData, const FItemData& inItemData);
};