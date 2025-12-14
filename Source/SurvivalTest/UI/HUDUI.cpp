#include "HUDUI.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "SurvivalTest/Items/ProjectileWeapon.h"
#include "SurvivalTest/Items/Weapon.h"
// ReSharper disable once CppUnusedIncludeDirective
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

FString UHUDUI::GetWeaponText()
{
	return GetPlayer()->GetEquippedWeapon() ? GetPlayer()->GetEquippedWeapon()->GetWeaponHUDText() : "None";
}

float UHUDUI::GetRestProgress()
{
	return (GetPlayer()->GetCurrentStats().rest / GetPlayer()->GetMaxStats().rest);
}

float UHUDUI::GetStaminaProgress()
{
	return (GetPlayer()->GetCurrentStats().stamina / GetPlayer()->GetMaxStats().stamina);
}

void UHUDUI::EnemyHit(ABaseCharacter* inActor)
{
	TriggerHitMarker(inActor);
}
