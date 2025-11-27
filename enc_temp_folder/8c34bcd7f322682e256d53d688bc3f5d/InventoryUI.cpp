#include "InventoryUI.h"

#include "ItemContainerUI.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/Interfaces/Interactable.h"
#include "SurvivalTest/Interfaces/ItemContainerInterface.h"

void UInventoryUI::SetupInventory(ABasePlayerController* inController, UBaseGameInstance* inGameInstance)
{
	SetBaseGameInstance(inGameInstance);
	SetController(inController);
	UInventory* inv = GetPlayerInventory();

	inv->OnContainerAdded.AddUniqueDynamic(this, &UInventoryUI::ContainerAdded);
	inv->OnContainerRemoved.AddUniqueDynamic(this, &UInventoryUI::ContainerRemoved);
	GenerateInventory_CPP();
}

void UInventoryUI::ContainerAdded(UItemContainer* container)
{
	AddContainerToPanel(container, GetNearbyContainerGrid());
}

void UInventoryUI::ContainerRemoved(UItemContainer* container)
{
	RemoveContainerFromPanel(container, GetNearbyContainerGrid());
}


void UInventoryUI::GenerateInventory_CPP()
{
	//GetNearbyContainerGrid()->ClearChildren();

	//for (auto& container : createdItemContainerUI)
	//{
	//	container.Value->ContainerRemovedFromUI();
	//}

	//createdItemContainerUI.Empty();

	for (UItemContainer* container : GetContainers())
	{
		AddContainerToPanel(container, GetNearbyContainerGrid());
	}

	if (playerContainerUI == NULL)
	{
		playerContainerUI = AddContainerToPanel(GetPlayerInventory(), GetPlayerInventoryPanel());
	}	
}

UItemContainerUI* UInventoryUI::AddContainerToPanel(UItemContainer* container, UPanelWidget* panel)
{
	UItemContainerUI* itemContainerUI = UItemContainerUI::CreateOrGetItemContainerUI(GetController(), container, GetBaseGameInstance());

	if (itemContainerUI != NULL)
	{
		createdItemContainerUI.Add(container->GetContainerInstanceID(), itemContainerUI);
		panel->AddChild(itemContainerUI);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AddContainerToGrid itemContainerUI was null"));
	}

	return itemContainerUI;
}

void UInventoryUI::RemoveContainerFromPanel(UItemContainer* container, UPanelWidget* panel)
{
	if (createdItemContainerUI.Contains(container->GetContainerInstanceID()))
	{
		UItemContainerUI* widget = createdItemContainerUI.FindChecked(container->GetContainerInstanceID());
		panel->RemoveChild(widget);
		createdItemContainerUI.Remove(container->GetContainerInstanceID());
	}
}

TArray<UItemContainer*> UInventoryUI::GetContainers()
{
	TArray<UItemContainer*> containers;

	for (IInteractable* inter : GetController()->GetBaseCharacter()->GetOverlappingInteractables())
	{
		if (Cast<AActor>(inter)->Implements<UItemContainerInterface>()) {
			IItemContainerInterface* container = Cast<IItemContainerInterface>(inter);
			containers.Add(container->GetItemContainer());
		}
	}
	return containers;
}

UInventory* UInventoryUI::GetPlayerInventory()
{
	return GetController()->GetBaseCharacter()->GetInventory();
}
