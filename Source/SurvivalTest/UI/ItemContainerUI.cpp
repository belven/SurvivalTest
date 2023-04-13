#include "ItemContainerUI.h"

#include "ItemUI.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "SurvivalTest/Tables/ContainerTableData.h"

UItemContainerUI::UItemContainerUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//static ConstructorHelpers::FClassFinder<UUserWidget> itemWidgetClassFound(TEXT("WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/ItemUI_BP.ItemUI_BP_C'"));

	//if (itemWidgetClassFound.Class != nullptr)
	//{
	//	itemWidgetClass = itemWidgetClassFound.Class;
	//}

	//static ConstructorHelpers::FClassFinder<UUserWidget> itemContainerWidgetClassFound(TEXT("WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/ItemContainerUI_BP.ItemContainerUI_BP_C'"));

	//if (itemContainerWidgetClassFound.Class != nullptr)
	//{
	//	itemContainerWidgetClass = itemContainerWidgetClassFound.Class;
	//}
}

void UItemContainerUI::GenerateInventory()
{
	//SetInventoryName_BP(GetContainerName());
	ResetIndex();
	GetItemsGrid()->ClearChildren();

	for (int slot = 0; slot < GetItemContainer()->GetMaxItemCount() - 1; ++slot)
	{
		FInstanceItemData iid = GetItemContainer()->GetInstanceItemAtSlot(slot);

		if (iid.ID == UItemStructs::InvalidInt)
		{
			iid.slot = slot;
			iid.amount = 1;
		}

		AddItemToGrid(iid);
	}
}

int32 UItemContainerUI::GetCurrentColumn()
{
	return index % 5;
}

int32 UItemContainerUI::GetNextRowIndex()
{
	int32 nextRow = GetCurrentRow() + 1;
	return (nextRow * 5);
}

void UItemContainerUI::GetGridData(int32& row, int32& column)
{
	row = GetCurrentRow();
	column = GetCurrentColumn();
}

int32 UItemContainerUI::GetCurrentRow()
{
	int32 itemsPerRow = 5;
	int32 rowMod = index % itemsPerRow;
	int32 nearestRow = index - rowMod;
	return nearestRow / itemsPerRow;
}

void UItemContainerUI::SetItemContainer(UItemContainer* inContainer)
{
	container = inContainer;

	// Set up add and remove listeners for our new container, so we can update our UI as things are added and removed
	container->OnItemRemoved.AddUniqueDynamic(this, &UItemContainerUI::ItemRemoved);
	container->OnItemAdded.AddUniqueDynamic(this, &UItemContainerUI::ItemAdded);
}

FString UItemContainerUI::GetContainerName()
{
	return GetBaseGameInstance()->GetContainerInstanceName(container->GetInstanceContainerData().ID);
}

UItemContainer* UItemContainerUI::GetItemContainerForArmour(FInstanceItemData data)
{
	FInstanceArmourData iadFound;
	FArmourData ad = GetBaseGameInstance()->GetArmourDataByItemID(data.itemID);
	FContainerData cd = GetBaseGameInstance()->GetContainerDataByID(ad.containerID);

	if (cd.slots > 0)
	{
		for (auto& iad : gameInstance->GetInstancedArmour())
		{
			if (iad.Value.instancedItemDataID == data.ID)
			{
				iadFound = iad.Value;
			}
		}

		FInstanceContainerData icd = gameInstance->GetInstancedContainers().FindChecked(iadFound.containerInstanceID);

		UItemContainer* ic = UItemContainer::CreateItemContainer(cd, icd, gameInstance);
		return ic;
	}
	return nullptr;
}

void UItemContainerUI::ItemAdded(FInstanceItemData inItem)
{
	GenerateInventory();
}

void UItemContainerUI::ItemRemoved(FInstanceItemData inItem)
{
	GenerateInventory();
}

UGridSlot* UItemContainerUI::AddToGrid(UUserWidget* widget)
{
	//return AddItemWidgetToGrid(widget, GetCurrentRow(), GetCurrentColumn());
	UGridSlot* slot = GetItemsGrid()->AddChildToGrid(widget, GetCurrentRow(), GetCurrentColumn());
	//Cast<UGridSlot>(widget->Slot)->SetColumn(GetCurrentColumn());
	//Cast<UGridSlot>(widget->Slot)->SetRow( GetCurrentRow());
	return slot;
}

void UItemContainerUI::AddItemToGrid(FInstanceItemData iid)
{
	UItemUI* item = CreateWidget<UItemUI>(this, itemWidgetClass, "Item UI");
	//UItemUI* item = WidgetTree.Get()->ConstructWidget<UItemUI>(itemWidgetClass, "Item UI");
	item->UpdateItemData(iid, GetBaseGameInstance()->GetItemData(iid.itemID), GetItemContainer());
	item->SetOwningPlayer(GetOwningPlayer());

	if (item->GetItemData().type == EItemType::Armour)
	{
		AddArmourUI(iid, item);
	}
	else
	{
		AddToGrid(item);
		IncrementIndex();
	}

	TArray< FStringFormatArg > args;
	args.Add(FStringFormatArg(item->GetItemData().ID));
	args.Add(FStringFormatArg(item->GetInstanceItemData().ID));
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Format(TEXT("Item ID {0}, Instance ID {1} "), args));

	args.Empty();
	args.Add(FStringFormatArg(GetCurrentColumn()));
	args.Add(FStringFormatArg(GetCurrentRow()));
	args.Add(FStringFormatArg(GetItemsGrid()->GetAllChildren().Num()));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, item->GetItemData().name + " added at " + FString::Format(TEXT("Column {0}, row {1} Items Count {2} "), args));
}

void UItemContainerUI::AddArmourUI(FInstanceItemData iid, UItemUI* itemUI)
{
	UItemContainer* armourContainer = GetItemContainerForArmour(iid);

	if (armourContainer)
	{
		SetNextRowIndex();
		AddToGrid(itemUI);

		//UItemContainerUI* itemContainerUI = WidgetTree.Get()->ConstructWidget<UItemContainerUI>(itemContainerWidgetClass, "Armour Item Container UI");
		UItemContainerUI* itemContainerUI = CreateWidget<UItemContainerUI>(this, itemContainerWidgetClass, "Armour Item Container UI");
		itemContainerUI->SetOwningPlayer(GetOwningPlayer());

		itemUI->SetItemContainer(armourContainer);
		itemContainerUI->SetItemContainer(armourContainer);
		itemContainerUI->SetBaseGameInstance(GetBaseGameInstance());
		itemContainerUI->SetVisibility(ESlateVisibility::Collapsed);

		SetNextRowIndex();

		UGridSlot* slot = AddToGrid(itemContainerUI);
		slot->SetColumnSpan(5);

		SetNextRowIndex();
		itemContainerUI->GenerateInventory();
	}
	else
	{
		AddToGrid(itemUI);
		IncrementIndex();
	}
}
