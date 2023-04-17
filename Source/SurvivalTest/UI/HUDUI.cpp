#include "HUDUI.h"
#include "SurvivalTest/BaseCharacter.h"

float UHUDUI::GetHealthProgress()
{
	return (GetPlayer()->GetCurrentStats().health / GetPlayer()->GetMaxStats().health) ;
}

float UHUDUI::GetWaterProgress()
{
	return (GetPlayer()->GetCurrentStats().water / GetPlayer()->GetMaxStats().water);
}

float UHUDUI::GetFoodProgress()
{
	return (GetPlayer()->GetCurrentStats().hunger / GetPlayer()->GetMaxStats().hunger);
}

float UHUDUI::GetRestProgress()
{
	return (GetPlayer()->GetCurrentStats().rest / GetPlayer()->GetMaxStats().rest);
}
