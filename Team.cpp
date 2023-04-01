#include "Team.h"
#include "FactionManager.h"
#include "SurvivalGameInstance.h"

ERelationshipType ITeam::GetRelationship(ITeam* other, USurvivalGameInstance* instance)
{
	return instance->GetFactionManager()->GetRelationship(this, other);
}
