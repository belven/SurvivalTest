#include "ItemUI.h"

#include "Components/Image.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "Widgets/Images/SImage.h"

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

FString UItemUI::GetItemAmount()
{
	return instanceItemData.amount == 1 ? "" : FString::FromInt(instanceItemData.amount);
}

UTexture2D* UItemUI::GetItemIcon()
{
	if (GetItemData().icon.Equals(""))
		return nullptr;

	return LoadObject<UTexture2D>(this, *GetItemData().icon);
}

void UItemUI::SetImage(UImage* image)
{
	if(!GetItemData().icon.Equals(""))
	{
		FSlateBrush imageB;
		imageB.ImageSize = FVector2D(imageSize);
		imageB.SetResourceObject(GetItemIcon());
		image->SetBrush(imageB);
		image->SetColorAndOpacity(FLinearColor::White);
	}
}

void UItemUI::ClearItemData()
{
	FInstanceItemData iid;
	iid.slot = GetInstanceItemData().slot;
	SetInstanceItemData(iid);
	SetItemData(FItemData());
}