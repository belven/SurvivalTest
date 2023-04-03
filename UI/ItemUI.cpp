#include "ItemUI.h"

void UItemUI::UpdateItemData(FInstanceItemData inInstanceData, FItemData inItemData)
{
	SetItemData(inItemData);
	SetInstanceItemData(inInstanceData);
	UpdateItemDetails();
}
