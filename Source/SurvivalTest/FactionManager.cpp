#include "FactionManager.h"

UFactionManager::UFactionManager()
{
	CreateRelationship(EFaction::A, EFaction::A, ERelationshipType::Enemy);
}

void UFactionManager::CreateRelationship(EFaction factionA, EFaction factionB, ERelationshipType type)
{
	TArray<FFactionRelationship>& aRelationships = relationships.FindOrAdd(factionA);
	aRelationships.Add(FFactionRelationship(factionB, type));

	TArray<FFactionRelationship>& bRelationships = relationships.FindOrAdd(factionB);
	bRelationships.Add(FFactionRelationship(factionA, type));
}

ERelationshipType UFactionManager::GetRelationship(ITeam* teamA, ITeam* teamB)
{
	TArray<FFactionRelationship>& aRelationships = relationships.FindOrAdd(teamA->GetFaction());

	for(const FFactionRelationship fr : aRelationships)
	{
		if(fr.faction == teamB->GetFaction())
		{
			return fr.relationship;
		}
	}

	return ERelationshipType::Neutral;
}