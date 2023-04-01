#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "ItemStructs.h"
#include "Armour.generated.h"

UCLASS()
class SURVIVALTEST_API UArmour : public UItem
{
	GENERATED_BODY()
public:

	FArmourData GetData() const { return data; }
	void SetData(FArmourData inData) { this->data = inData; }	

	static UArmour* CreateArmour(FArmourData inData);
private:
	FArmourData data;
};