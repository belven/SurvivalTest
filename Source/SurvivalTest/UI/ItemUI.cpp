#include "ItemUI.h"

#include "InventoryUI.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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
	if (!disabled) {
		if (InMouseEvent.GetPressedButtons().Contains(EKeys::RightMouseButton))
		{
			if (itemData.type == EItemType::Consumable && GetInstanceItemData().amount > 0)
			{
				UBaseGameInstance* baseGameInstance = itemContainer->GetGame();
				FConsumableData cd = baseGameInstance->GetConsumableData(itemData.ID);
				ABasePlayerController* basePlayerController = Cast<ABasePlayerController>(GetOwningPlayer());

				basePlayerController->GetBaseCharacter()->Consume(cd.consumableType, cd.value);

				FInstanceItemData oldData = GetInstanceItemData();
				instanceItemData.amount--;

				if (GetInstanceItemData().amount > 0)
				{
					GetItemContainer()->UpdateItemData(GetItemContainer(), instanceItemData, oldData);
				}
				else
				{
					GetItemContainer()->RemoveInstanceItem(GetItemContainer(), instanceItemData);
				}
			}
		}
		else if (InMouseEvent.GetPressedButtons().Contains(EKeys::LeftMouseButton) && itemData.ID != UItemStructs::InvalidInt)
		{
			if (InMouseEvent.GetModifierKeys().IsLeftShiftDown())
			{
				ABasePlayerController* basePlayerController = Cast<ABasePlayerController>(GetOwningPlayer());
				UItemContainer* playerInventory = basePlayerController->GetBaseCharacter()->GetInventory();

				if (GetInstanceItemData().containerInstanceID != playerInventory->GetInstanceContainerData().ID)
				{
					playerInventory->TransferItem(GetItemContainer(), GetInstanceItemData(), UItemStructs::InvalidInt);
				}
				else
				{
					TArray<UItemContainer*> containers = basePlayerController->GetInventoryWidget()->GetContainers();

					if (!containers.IsEmpty())
					{
						for (UItemContainer* ic : containers)
						{
							FInstanceItemData iid = ic->TransferItem(GetItemContainer(), GetInstanceItemData(), UItemStructs::InvalidInt);

							if (iid.amount == 0)
							{
								break;
							}
						}
					}
				}
			}
			else if (InMouseEvent.GetModifierKeys().IsLeftControlDown())
			{
				GetItemContainer()->SplitItem(instanceItemData);
			}
			else
			{
				return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
			}
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FString UItemUI::GetItemAmount()
{
	return GetInstanceItemData().amount == 1 ? "" : FString::FromInt(GetInstanceItemData().amount);
}

UTexture2D* UItemUI::GetItemIcon()
{
	if (GetItemData().icon.IsEmpty())
		return nullptr;

	return LoadObject<UTexture2D>(this, *GetItemData().icon);
}

void UItemUI::SetImage(UImage* image)
{
	if (!GetItemData().icon.IsEmpty())
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
	SetInstanceItemData(GetInstanceItemData().CreateEmptyCopy());
	SetItemData(FItemData());
}

bool UItemUI::IsItemValid()
{
	return GetInstanceItemData().isValid();
}
