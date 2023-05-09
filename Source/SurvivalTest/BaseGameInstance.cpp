#include "BaseGameInstance.h"
#include "FactionManager.h"
#include "Events/RPGEventManager.h"
#include "Items/ItemContainer.h"
#include "Missions/MissionManager.h"
#include "Tables/ArmourDataTable.h"
#include "Tables/TableManager.h"
#include "Tables/Items/WeaponInstanceTable.h"


void UBaseGameInstance::Init()
{
	Super::Init();
	mTable()->LoadTableData();
}

void UBaseGameInstance::Shutdown()
{
	mTable()->SaveTableToFile(mTable()->GetInstanceItemDataTable());
	Super::Shutdown();
}

int32 UBaseGameInstance::GetNextInstanceItemDataID()
{
	int32 instanceItemDataID = 0;
	if (GetInstancedItems().Num() > 0)
	{
		instanceItemDataID = GetInstancedItems()[GetInstancedItems().Num() - 1].ID + 1;
	}
	return instanceItemDataID;
}

int32 UBaseGameInstance::GetNextBoxID()
{
	int32 boxID = 0;
	if (GetInstancedBoxes().Num() > 0)
	{
		boxID = GetInstancedBoxes()[GetInstancedBoxes().Num() - 1].boxID + 1;
	}
	return boxID;
}

int32 UBaseGameInstance::GetNextInstanceBoxDataID()
{
	int32 instanceBoxDataID = 0;
	if (GetInstancedBoxes().Num() > 0)
	{
		instanceBoxDataID = GetInstancedBoxes()[GetInstancedBoxes().Num() - 1].ID + 1;
	}
	return instanceBoxDataID;
}

int32 UBaseGameInstance::GetNextInstanceArmourDataID()
{
	int32 instanceArmourDataID = 0;
	if (GetInstancedArmour().Num() > 0)
	{
		instanceArmourDataID = GetInstancedArmour().FindChecked(GetInstancedArmour().Num() - 1).ID + 1;
	}
	return instanceArmourDataID;
}

int32 UBaseGameInstance::GetNextInstanceContainerDataID()
{
	int32 instanceContainerDataID = 0;
	if (GetInstancedContainers().Num() > 0)
	{
		instanceContainerDataID = GetInstancedContainers()[GetInstancedContainers().Num() - 1].ID + 1;
	}
	return instanceContainerDataID;
}


int32 UBaseGameInstance::GetNextInstanceWeaponDataID()
{
	int32 instanceWeaponDataID = 0;
	TMap<int32, FInstanceWeaponData> instancedWeapons = GetTableManager()->GetWeaponInstanceTable()->GetData();

	if (instancedWeapons.Num() > 0)
	{
		instanceWeaponDataID = instancedWeapons[instancedWeapons.Num() - 1].ID + 1;
	}
	return instanceWeaponDataID;
}

UFactionManager* UBaseGameInstance::GetFactionManager()
{
	if (factionManager == nullptr) { factionManager = NewObject<UFactionManager>(); }
	return factionManager;
}

UMissionManager* UBaseGameInstance::GetMissionManager()
{
	if (missionManager == nullptr)
	{
		missionManager = NewObject<UMissionManager>();
		missionManager->SetGame(this);
	}

	return missionManager;
}

URPGEventManager* UBaseGameInstance::GetEventManager()
{
	if (eventManager == nullptr) { eventManager = NewObject<URPGEventManager>(); }
	return eventManager;
}


UTableManager* UBaseGameInstance::GetTableManager()
{
	if (tableManager == nullptr) { tableManager = NewObject<UTableManager>(); }
	return tableManager;
}