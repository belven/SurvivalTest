#pragma once
#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "SurvivalTest/CharacterStructs.h"
#include "HealthChangeEvent.generated.h"

#define mCreateHealthChangeEvent UHealthChangeEvent::CreateHealthChangeEvent

UCLASS()
class SURVIVALTEST_API UHealthChangeEvent : public UBaseEvent
{
	GENERATED_BODY()
public:
	static UHealthChangeEvent* CreateHealthChangeEvent(UObject* owner, FHealthChange hc, bool preHealthChange);

	FHealthChange GetChange() { return hc; }
	bool GetPreChange() { return preHealthChange; }

private:
	FHealthChange hc;
	bool preHealthChange;
};