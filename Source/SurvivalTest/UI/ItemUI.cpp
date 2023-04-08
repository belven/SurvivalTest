#include "ItemUI.h"
#include "SurvivalTest/Items/ItemContainer.h"

void UItemUI::UpdateItemData(FInstanceItemData inInstanceData, FItemData inItemData, UItemContainer* inItemContainer)
{
	SetItemData(inItemData);
	SetInstanceItemData(inInstanceData);
	SetItemContainer(inItemContainer);
	UpdateItemDetails();
}

UTexture2D* UItemUI::GetItemIcon()
{
	return LoadObject<UTexture2D>(this, *GetItemData().mesh);
}
