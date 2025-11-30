#include "BaseGameInstance.h"
#include "Items/ItemContainer.h"
#include "Missions/MissionManager.h"
#include "Tables/ArmourDataTable.h"
#include "Tables/TableManager.h"
// ReSharper disable once CppUnusedIncludeDirective Required for GetSingletonObject
#include "Events/RPGEventManager.h"
// ReSharper disable once CppUnusedIncludeDirective Required for GetSingletonObject
#include "FactionManager.h"
#include "HelperFunctions.h"
#include "ObjectInstanceManager.h"

#define GetLastMapItem(type, values) UHelperFunctions::GetLastMapItem<int32, type>(values)
TMap<int32, UItemContainerUI*> UBaseGameInstance::createdItemContainerUI;

void UBaseGameInstance::Init()
{
	Super::Init();
	mTable()->LoadTableData();
}

void UBaseGameInstance::Shutdown()
{
	mTable()->SaveTableToFile(mTable()->GetInstanceItemDataTable());
	createdItemContainerUI.Empty();
	Super::Shutdown();
}

int32 UBaseGameInstance::GetNextInstanceItemDataID()
{
	return mTable()->GetNextInstanceItemDataID();

}

int32 UBaseGameInstance::GetNextInstanceLootBoxDataID()
{
	return mTable()->GetNextInstanceLootBoxDataID();
}

int32 UBaseGameInstance::GetNextInstanceArmourDataID()
{
	return mTable()->GetNextInstanceArmourDataID();
}

FInstanceWeaponData UBaseGameInstance::CreateNewInstanceWeaponData(int32 instanceItemID, const FProjectileWeaponData& pwd)
{
	return mTable()->CreateNewInstanceWeaponData(instanceItemID, pwd);
}

int32 UBaseGameInstance::GetNextInstanceContainerDataID()
{
	return mTable()->GetNextInstanceContainerDataID();
}

int32 UBaseGameInstance::GetNextInstanceWeaponDataID()
{
	return mTable()->GetNextInstanceWeaponDataID();
}

TMap<int32, UItemContainerUI*>& UBaseGameInstance::GetCreatedItemContainerUI()
{
	return createdItemContainerUI;
}

UFactionManager* UBaseGameInstance::GetFactionManager()
{
	return UHelperFunctions::GetValue(factionManager, this);
}

UMissionManager* UBaseGameInstance::GetMissionManager()
{
	if (missionManager == NULL)
	{
		UHelperFunctions::GetValue(missionManager, this);
		missionManager->SetGame(this);
	}

	return missionManager;
}

AObjectInstanceManager* UBaseGameInstance::GetObjectInstanceManager()
{
	return UHelperFunctions::GetValue(objectInstanceManager, GetWorld());
}

URPGEventManager* UBaseGameInstance::GetEventManager()
{
	return UHelperFunctions::GetValue(eventManager, this);
}

UTableManager* UBaseGameInstance::GetTableManager()
{
	return UHelperFunctions::GetValue(tableManager, this);
}
