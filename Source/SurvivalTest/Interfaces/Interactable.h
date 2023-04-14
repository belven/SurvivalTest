#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class ABasePlayerController;

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALTEST_API IInteractable
{
	GENERATED_BODY()
public:
	virtual void Interact(ABasePlayerController* instigator) = 0;
	virtual void Highlight(bool activate) = 0;
};
