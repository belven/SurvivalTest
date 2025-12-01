#pragma once
#include "CoreMinimal.h"
#include "CharacterTask.h"
#include "EquipmentSwapTask.generated.h"

class UAnimationAction;
class USwapEquipmentAction;

UCLASS()
class SURVIVALTEST_API UEquipmentSwapTask : public UCharacterTask
{
	GENERATED_BODY()
public:
	UEquipmentSwapTask();
	virtual void PerformTask(AController* inController) override;

	void SetSlot(int32 newSlot) { slot = newSlot; }

private:
	UPROPERTY();
	USwapEquipmentAction* swapAction; 

	UPROPERTY();
	UAnimationAction* animationAction;

	UPROPERTY()
	int32 slot;
};
