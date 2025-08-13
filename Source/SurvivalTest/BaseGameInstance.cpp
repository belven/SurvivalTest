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
	FScopeLock Lock(&InstanceItemIDLock); // Thread-safe guard

	const TMap<int32, FInstanceItemData>& items = GetInstancedItems();

	int32& id = LastInstanceItemID;

	if (id < 0 || id < items.Num() - 1)
	{
		id = -1;
		for (const auto& Pair : items)
		{
			if (Pair.Value.ID > id)
			{
				id = Pair.Value.ID;
			}
		}
	}

	//TArray< FInstanceItemData> itemsInMap;

	//GetInstancedItems().GenerateValueArray(itemsInMap);
	//FInstanceItemData iidFound = GetDataByID<FInstanceItemData>(itemsInMap, instanceItemFilter, id);

	//if (id == iidFound.ID)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("ID Found"));
	//}

	return ++id;

}
int32 UBaseGameInstance::GetNextBoxID()
{
	FScopeLock Lock(&BoxIDLock);

	TMap<int32, FInstanceBoxData> boxes = GetInstancedBoxes();

	if (LastBoxID < 0 || LastBoxID < boxes.Num() - 1)
	{
		LastBoxID = -1;
		for (const auto& Pair : boxes)
		{
			if (Pair.Value.ID > LastBoxID)
			{
				LastBoxID = Pair.Value.ID;
			}
		}
	}

	return ++LastBoxID;
}

int32 UBaseGameInstance::GetNextInstanceBoxDataID()
{
	FScopeLock Lock(&BoxDataIDLock);

	TMap<int32, FInstanceBoxData> boxes = GetInstancedBoxes();

	int32& id = LastBoxID;

	if (id < 0 || id < boxes.Num() - 1)
	{
		id = -1;
		for (const auto& Pair : boxes)
		{
			if (Pair.Value.ID > id)
			{
				id = Pair.Value.ID;
			}
		}
	}

	return ++id;
}

int32 UBaseGameInstance::GetNextInstanceArmourDataID()
{
	FScopeLock Lock(&InstanceArmourDataLock);

	TMap<int32, FInstanceArmourData> armour = GetInstancedArmour();

	int32& id = LastInstanceArmourDataID;

	if (id < 0 || id < armour.Num() - 1)
	{
		id = -1;
		for (const auto& Pair : armour)
		{
			if (Pair.Value.ID > id)
			{
				id = Pair.Value.ID;
			}
		}
	}

	return ++id;
}

int32 UBaseGameInstance::GetNextInstanceContainerDataID()
{
	FScopeLock Lock(&InstanceContainerDataLock);

	TMap<int32, FInstanceContainerData> containers = GetInstancedContainers();

	int32& id = LastInstanceContainerDataID;

	if (id < 0 || id < containers.Num() - 1)
	{
		id = -1;
		for (const auto& Pair : containers)
		{
			if (Pair.Value.ID > id)
			{
				id = Pair.Value.ID;
			}
		}
	}

	return ++id;
}

int32 UBaseGameInstance::GetNextInstanceWeaponDataID()
{
	FScopeLock Lock(&InstanceWeaponDataLock);

	TMap<int32, FInstanceWeaponData> instancedWeapons = GetTableManager()->GetWeaponInstanceTable()->GetData();
	
	int32& id = LastInstanceWeaponDataID;

	if (id < 0 || id < instancedWeapons.Num() - 1)
	{
		id = -1;
		for (const auto& Pair : instancedWeapons)
		{
			if (Pair.Value.ID > id)
			{
				id = Pair.Value.ID;
			}
		}
	}

	return ++id;
}

UFactionManager* UBaseGameInstance::GetFactionManager()
{
	return GetSingletonObject(factionManager);
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
	return GetSingletonObject(eventManager);
}


UTableManager* UBaseGameInstance::GetTableManager()
{
	return GetSingletonObject(tableManager);
}

template <class T>
T* UBaseGameInstance::GetSingletonObject(T*& object)
{
    if (object == nullptr) { object = NewObject<T>(); }
    return object;
}