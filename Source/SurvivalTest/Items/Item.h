#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStructs.h"
#include "Item.generated.h"

UCLASS()
class SURVIVALTEST_API UItem : public UObject
{
	GENERATED_BODY()
public:
	FItemData GetItemData() const { return itemData; }
	void SetItemData(FItemData data) { itemData = data; }
protected:
	FItemData itemData;
};