#include "MissionStructs.h"

EMissionType UMissionStructs::GetMissionType(FString type)
{
	if (type.Equals("Medical"))
	{
		return EMissionType::Medical;
	}
	else if (type.Equals("Civilian"))
	{
		return EMissionType::Civilian;
	}
	else if (type.Equals("Military"))
	{
		return EMissionType::Military;
	}
	else if (type.Equals("Secret"))
	{
		return EMissionType::Secret;
	}
	return EMissionType::End;
}
