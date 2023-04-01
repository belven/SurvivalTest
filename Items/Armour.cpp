#include "Armour.h"

UArmour* UArmour::CreateArmour(FArmourData inData)
{
	UArmour* armour = NewObject<UArmour>();
	armour->SetData(inData);
	return armour;
}