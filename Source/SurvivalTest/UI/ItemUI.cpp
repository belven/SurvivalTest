#include "ItemUI.h"

#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/Items/ItemContainer.h"

void UItemUI::UpdateItemData(FInstanceItemData inInstanceData, FItemData inItemData, UItemContainer* inItemContainer)
{
	SetItemData(inItemData);
	SetInstanceItemData(inInstanceData);
	SetItemContainer(inItemContainer);
	UpdateItemDetails();
}

FReply UItemUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetPressedButtons().Contains(EKeys::RightMouseButton))
	{
		if (itemData.type == EItemType::Consumable && instanceItemData.amount > 0)
		{
			UBaseGameInstance* baseGameInstance = itemContainer->GetGame();
			FConsumableData cd = baseGameInstance->GetConsumableData(itemData.ID);
			ABasePlayerController* basePlayerController = Cast<ABasePlayerController>(GetOwningPlayer());

			basePlayerController->GetBaseCharacter()->Consume(cd.consumableType, cd.value);
			instanceItemData.amount--;

			if (instanceItemData.amount > 0)
			{
				baseGameInstance->AddUpdateData(instanceItemData);
				itemContainer->OnItemRemoved.Broadcast(instanceItemData);
			}
			else
			{
				baseGameInstance->GetInstancedItems().Remove(instanceItemData.ID);
				itemContainer->OnItemRemoved.Broadcast(instanceItemData);
			}
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

UTexture2D* UItemUI::GetItemIcon()
{
	if (GetItemData().icon.Equals(""))
		return nullptr;

	return LoadObject<UTexture2D>(this, *GetItemData().icon);
}
