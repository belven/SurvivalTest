#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "ItemStructs.h"
#include "Armour.generated.h"

class UItemContainer;

UCLASS()
class SURVIVALTEST_API UArmour : public UItem
{
	GENERATED_BODY()
public:

	FArmourData GetData() const { return data; }
	void SetData(const FArmourData& inData) { this->data = inData; }

	FInstanceArmourData GetInstanceArmourData() const { return instanceArmourData; }
	void SetInstanceArmourData(FInstanceArmourData inContainerData) { this->instanceArmourData = inContainerData; }

	UItemContainer* GetContainer() const { return container; }
	void SetContainer(UItemContainer* inContainer) { this->container = inContainer; }

	FInstanceContainerData GetInstanceContainerData() const { return instanceContainerData; }
	void SetInstanceContainerData(const FInstanceContainerData& inInstanceContainerData) { instanceContainerData = inInstanceContainerData; }
private:
	FArmourData data;
	FInstanceArmourData instanceArmourData;
	FInstanceContainerData instanceContainerData;

	UPROPERTY()
		UItemContainer* container;
};