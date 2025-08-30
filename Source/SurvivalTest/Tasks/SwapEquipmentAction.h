#pragma once
#include "CoreMinimal.h"
#include "TaskAction.h"
#include "SwapEquipmentAction.generated.h"


UCLASS()
class SURVIVALTEST_API USwapEquipmentAction : public UTaskAction
{
	GENERATED_BODY()
public:
	static USwapEquipmentAction* CreateSwapEquipmentAction(ABaseCharacter* character);

	virtual void StartAction() override;

private:
};
