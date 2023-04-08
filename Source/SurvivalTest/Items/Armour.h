#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "ItemStructs.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "Armour.generated.h"

class UItemContainer;

UCLASS()
class SURVIVALTEST_API UArmour : public UItem
{
	GENERATED_BODY()
public:

	FArmourData GetData() const { return data; }
	void SetData(FArmourData inData) { this->data = inData; }

	static UArmour* CreateArmour(int32 itemID, UBaseGameInstance* game, int32 instanceItemDataID);
	static UArmour* LoadArmour(int32 armourInstanceID, UBaseGameInstance* game);

	FInstanceArmourData GetInstanceArmourData() const { return instanceArmourData; }
	void SetInstanceArmourData(FInstanceArmourData inContainerData) { this->instanceArmourData = inContainerData; }

	UItemContainer* GetContainer() const { return container; }
	void SetContainer(UItemContainer* inContainer) { this->container = inContainer; }

	FInstanceContainerData GetInstanceContainerData() const { return instanceContainerData; }
	void SetInstanceItemData(FInstanceContainerData inInstanceContainerData) { instanceContainerData = inInstanceContainerData; }
private:
	FArmourData data;
	FInstanceArmourData instanceArmourData;
	FInstanceContainerData instanceContainerData;

	UPROPERTY()
		UItemContainer* container;
};