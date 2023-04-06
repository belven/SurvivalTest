#include "ItemUI.h"

void UItemUI::UpdateItemData(FInstanceItemData inInstanceData, FItemData inItemData)
{
	SetItemData(inItemData);
	SetInstanceItemData(inInstanceData);
	UpdateItemDetails();
}

UTexture2D* UItemUI::GetItemIcon()
{
	return LoadObject<UTexture2D>(this, *GetItemData().mesh);
}
