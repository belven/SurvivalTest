#pragma once
#include "CoreMinimal.h"
#include "TaskAction.h"
#include "SwapEquipmentAction.generated.h"


UCLASS()
class SURVIVALTEST_API USwapEquipmentAction : public UTaskAction
{
	GENERATED_BODY()
public:
	static USwapEquipmentAction* CreateSwapEquipmentAction(ABaseCharacter* character, int32 slot);

	void SetSlot(int32 newSlot) { slot = newSlot; }

	virtual void StartAction() override;
private:
	int32 slot = UItemStructs::InvalidInt;
};
