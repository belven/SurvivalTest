#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "ItemStructs.h"
#include "SurvivalTest/SurvivalGameInstance.h"
#include "Armour.generated.h"

class UItemContainer;

UCLASS()
class SURVIVALTEST_API UArmour : public UItem
{
	GENERATED_BODY()
public:

	FArmourData GetData() const { return data; }
	void SetData(FArmourData inData) { this->data = inData; }

	static UArmour* CreateArmour(int32 itemID, USurvivalGameInstance* game);
	static UArmour* LoadArmour(int32 armourInstanceID, USurvivalGameInstance* game);

	FInstanceArmourData GetInstanceArmourData() const { return instanceArmourData; }
	void SetInstanceArmourData(FInstanceArmourData inContainerData) { this->instanceArmourData = inContainerData; }

	UItemContainer* GetContainer() const { return container; }
	void SetContainer(UItemContainer* inContainer) { this->container = inContainer; }
private:
	FArmourData data;
	FInstanceArmourData instanceArmourData;

	UPROPERTY()
		UItemContainer* container;
};