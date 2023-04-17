#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponCreator.generated.h"

#define mNewObject(uClass) NewObject<uClass>() 

class UWeapon;
UCLASS()
class SURVIVALTEST_API UWeaponCreator : public UObject
{
	GENERATED_BODY()
public:
		static UWeapon* CreateWeapon(const int32 itemID, const UWorld* world, int32 instanceItemID);
};