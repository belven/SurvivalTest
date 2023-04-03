#include "Team.h"
#include "FactionManager.h"
#include "BaseGameInstance.h"

ERelationshipType ITeam::GetRelationship(ITeam* other, UBaseGameInstance* instance)
{
	return instance->GetFactionManager()->GetRelationship(this, other);
}
