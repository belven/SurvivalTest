#pragma once
#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "SurvivalTest/CharacterStructs.h"
#include "CombatStateEvent.generated.h"

#define  mCreateCombatStateEvent UCombatStateEvent::CreateCombatStateEvent

UCLASS()
class SURVIVALTEST_API UCombatStateEvent : public UBaseEvent
{
	GENERATED_BODY()
public:
	static UCombatStateEvent* CreateCombatStateEvent(UObject* owner, FCombatStateChange change);
	FCombatStateChange GetStateChange( ) { return csc; }
private:
	FCombatStateChange csc;
};