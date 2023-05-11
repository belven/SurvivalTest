#include "BaseGameInstance.h"
#include "FactionManager.h"
#include "HelperFunctions.h"
#include "Events/RPGEventManager.h"
#include "Items/ItemContainer.h"
#include "Missions/MissionManager.h"
#include "Tables/ArmourDataTable.h"
#include "Tables/TableManager.h"
#include "Tables/Items/WeaponInstanceTable.h"

#define GetLastMapItem(type, values) UHelperFunctions::GetLastMapItem<int32, type>(values)

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
	TMap<int32, FInstanceItemData> items = GetInstancedItems();
	
	if (items.Num() > 0)
	{
		instanceItemDataID = GetLastMapItem(FInstanceItemData, items).ID + 1;
	}
	return instanceItemDataID;
}

int32 UBaseGameInstance::GetNextBoxID()
{
	int32 boxID = 0;
	TMap<int32, FInstanceBoxData> boxes = GetInstancedBoxes();
	if (boxes.Num() > 0)
	{
		boxID = GetLastMapItem(FInstanceBoxData, boxes).boxID + 1;
	}
	return boxID;
}

int32 UBaseGameInstance::GetNextInstanceBoxDataID()
{
	int32 instanceBoxDataID = 0;
	TMap<int32, FInstanceBoxData> boxes = GetInstancedBoxes();
	if (boxes.Num() > 0)
	{
		instanceBoxDataID = GetLastMapItem(FInstanceBoxData, boxes).ID + 1;
	}
	return instanceBoxDataID;
}

int32 UBaseGameInstance::GetNextInstanceArmourDataID()
{
	int32 instanceArmourDataID = 0;
	TMap<int32, FInstanceArmourData> armour = GetInstancedArmour();
	if (armour.Num() > 0)
	{
		instanceArmourDataID = GetLastMapItem(FInstanceArmourData, armour).ID + 1;
	}
	return instanceArmourDataID;
}

int32 UBaseGameInstance::GetNextInstanceContainerDataID()
{
	int32 instanceContainerDataID = 0;
	TMap<int32, FInstanceContainerData> containers = GetInstancedContainers();
	if (containers.Num() > 0)
	{
		instanceContainerDataID = GetLastMapItem(FInstanceContainerData, containers).ID + 1;
	}
	return instanceContainerDataID;
}


int32 UBaseGameInstance::GetNextInstanceWeaponDataID()
{
	int32 instanceWeaponDataID = 0;
	TMap<int32, FInstanceWeaponData> instancedWeapons = GetTableManager()->GetWeaponInstanceTable()->GetData();

	if (instancedWeapons.Num() > 0)
	{
		instanceWeaponDataID = GetLastMapItem(FInstanceWeaponData, instancedWeapons).ID + 1;
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