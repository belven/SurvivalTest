#include "ItemContainerUI.h"
#include "ItemUI.h"

const int UItemContainerUI::itemsPerRow = 5;

TSubclassOf<UUserWidget> UItemContainerUI::itemUIClass = NULL;
TSubclassOf<UUserWidget> UItemContainerUI::itemContainerClass;

UItemContainerUI* UItemContainerUI::CreateOrGetItemContainerUI(APlayerController* controller, UItemContainer* inContainer, UBaseGameInstance* inGameInstance)
{
	UItemContainerUI* itemContainerUI = NULL;

	TMap<int32, UItemContainerUI*> createdItemContainerUI = inGameInstance->GetCreatedItemContainerUI();

	if (itemContainerClass == NULL)
	{
		itemContainerClass = LoadClass<UUserWidget>(controller, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/ItemContainerUI_BP.ItemContainerUI_BP_C'"));
	}

	if (createdItemContainerUI.Contains(inContainer->GetContainerInstanceID()))
	{
		itemContainerUI = createdItemContainerUI.FindChecked(inContainer->GetContainerInstanceID());
	}
	else	if (itemContainerClass != NULL)
	{
		itemContainerUI = CreateWidget<UItemContainerUI>(controller, itemContainerClass);
		itemContainerUI->SetupItemContainerUI(inContainer, inGameInstance);
		createdItemContainerUI.Add(inContainer->GetContainerInstanceID(), itemContainerUI);
	}

	return itemContainerUI;
}

void UItemContainerUI::SetupItemContainerUI(UItemContainer* inContainer, UBaseGameInstance* inGameInstance)
{
	SetItemContainer(inContainer);
	SetBaseGameInstance(inGameInstance);
	GenerateInventory();
}

void UItemContainerUI::AddItemToGrid(FInstanceItemData iid, int32 index)
{
	if (itemUIClass == NULL)
	{
		itemUIClass = LoadClass<UUserWidget>(GetOwningPlayer(), TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/ItemUI_BP.ItemUI_BP_C'"));
	}

	if (itemUIClass != NULL)
	{
		UItemUI* itemUI = CreateWidget<UItemUI>(GetOwningPlayer(), itemUIClass);
		itemUI->UpdateItemData(iid, GetBaseGameInstance()->GetItemData(iid.itemID), GetItemContainer());
		GetItemGrid()->AddChildToGrid(itemUI, GetRow(index), GetColumn(index));
		itemUIAtSlots.Add(iid.slot, itemUI);
	}
}

void UItemContainerUI::GenerateInventory()
{
	GetItemGrid()->ClearChildren();
	itemUIAtSlots.Empty();

	for (int index = 0; index < GetItemContainer()->GetMaxItemCount() - 1; index++)
	{
		FInstanceItemData iid = GetItemContainer()->GetInstanceItemAtSlot(index);

		if (iid.isValid())
		{
			AddItemToGrid(iid, index);
		}
		else
		{
			AddItemToGrid(GetBlankInstanceItemData(index), index);
		}
	}
}

int32 UItemContainerUI::GetColumn(int32 index)
{
	return index % itemsPerRow;
}

int32 UItemContainerUI::GetRow(int32 index)
{
	int32 rowMod = index % itemsPerRow;
	int32 nearestRow = index - rowMod;
	return nearestRow / itemsPerRow;
}

void UItemContainerUI::SetItemContainer(UItemContainer* inContainer)
{
	container = inContainer;

	if (container != NULL)
	{
		// Set up add and remove listeners for our new container, so we can update our UI as things are added and removed
		container->OnItemUpdated.AddUniqueDynamic(this, &UItemContainerUI::ItemUpdated);
		SetInventoryName();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UItemContainerUI container was null"));
	}
}

FString UItemContainerUI::GetContainerName()
{
	if (name.IsEmpty() && container != NULL)
	{
		name = container->GetContainerName();
	}
	return name;
}

UItemContainer* UItemContainerUI::GetItemContainerForArmour(FInstanceItemData data)
{
	FInstanceArmourData iadFound;
	FArmourData ad = GetBaseGameInstance()->GetArmourDataByItemID(data.itemID);
	FContainerData cd = GetBaseGameInstance()->GetContainerDataByID(ad.containerID);

	if (cd.slots > 0)
	{
		TArray<FInstanceArmourData> armour;
		gameInstance->GetInstancedArmour().GenerateValueArray(armour);

		for (FInstanceArmourData iad : armour)
		{
			if (iad.instancedItemDataID == data.ID)
			{
				iadFound = iad;
			}
		}

		FInstanceContainerData icd = gameInstance->GetInstancedContainers().FindChecked(iadFound.containerInstanceID);

		UItemContainer* ic = UItemContainer::CreateItemContainer(cd, icd, gameInstance);
		return ic;
	}
	return nullptr;
}

bool UItemContainerUI::IsArmour(FItemData id)
{
	return id.type == EItemType::Armour;
}

UItemUI* UItemContainerUI::GetItemAtSlot(int32 itemSlot)
{
	if (itemUIAtSlots.Contains(itemSlot))
	{
		return itemUIAtSlots.FindChecked(itemSlot);
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to find item at slot %d in container %s"), itemSlot, *GetContainerName());
	return NULL;
}

FInstanceItemData UItemContainerUI::GetBlankInstanceItemData(int32 containerSlot)
{
	return FInstanceItemData::CreateEmptyItem(containerSlot, container->GetContainerInstanceID());
}

void UItemContainerUI::UpdateItemUI(const FInstanceItemData& newItem, const FInstanceItemData& oldItem)
{
	UItemUI* itemAtSlot = GetItemAtSlot(oldItem.slot);

	// TODO this only happened due to the UI being removed as we moved around 
	if (itemAtSlot)
	{
		if (newItem.isValid())
		{
			FItemData data = GetBaseGameInstance()->GetItemData(newItem.itemID);
			FInstanceItemData updatedNewItem = newItem;
			updatedNewItem.slot = oldItem.slot;
			updatedNewItem.containerInstanceID = GetItemContainer()->GetContainerInstanceID();
			itemAtSlot->UpdateItemData(updatedNewItem, data, GetItemContainer());
		}
		else
		{
			itemAtSlot->UpdateItemData(FInstanceItemData::CreateEmptyItem(oldItem.slot, GetItemContainer()->GetContainerInstanceID()), FItemData(), GetItemContainer());
		}
	}
}

void UItemContainerUI::ItemUpdated(const FInstanceItemData& newItem, const FInstanceItemData& oldItem)
{
	UpdateItemUI(newItem, oldItem);
}
