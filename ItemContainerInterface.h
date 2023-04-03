#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Items/ItemContainer.h"
#include "ItemContainerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemContainerInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALTEST_API IItemContainerInterface
{
	GENERATED_BODY()
public:
	virtual UItemContainer* GetItemContainer() = 0;
};
