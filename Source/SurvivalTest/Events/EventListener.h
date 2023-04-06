#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EventListener.generated.h"

class UBaseEvent;

UINTERFACE(MinimalAPI)
class UEventListener : public UInterface
{
	GENERATED_BODY()
};

class SURVIVALTEST_API IEventListener
{
	GENERATED_BODY()

public:
	UFUNCTION()
		virtual void EventTriggered(UBaseEvent* inEvent);
};