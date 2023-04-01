#include "CombatStateEvent.h"
#include "../CharacterStructs.h"

UCombatStateEvent* UCombatStateEvent::CreateCombatStateEvent(UObject* owner, FCombatStateChange change)
{
	UCombatStateEvent* cse = NewObject<UCombatStateEvent>();
	cse->csc = change;
	cse->SetEventType(EEventType::CombatState);
	cse->SetEventOwner(owner);
	return cse;
}