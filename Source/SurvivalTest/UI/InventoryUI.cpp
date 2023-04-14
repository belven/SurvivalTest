#include "InventoryUI.h"

#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/Interfaces/Interactable.h"
#include "SurvivalTest/Interfaces/ItemContainerInterface.h"

TArray<UItemContainer*> UInventoryUI::GetContainers()
{
	TArray<UItemContainer*> containers;

	for(IInteractable* inter : GetController()->GetBaseCharacter()->GetOverlappingInteractables())
	{
		if (Cast<AActor>(inter)->Implements<UItemContainerInterface>()) {
			IItemContainerInterface* container = Cast<IItemContainerInterface>(inter);
			containers.Add(container->GetItemContainer());
		}
	}
	return containers;
}

UItemContainer* UInventoryUI::GetPlayerInventory()
{
	return GetController()->GetBaseCharacter()->GetInventory();
}
