#include "ItemDragDrop.h"
#include "ItemUI.h"

UItemDragDrop* UItemDragDrop::CreateDragDrop(UItemUI* item)
{
	UItemDragDrop* dragDrop = NewObject<UItemDragDrop>();
	dragDrop->SetDraggedItem(item);
	dragDrop->DefaultDragVisual = item->GetRootWidget();
	dragDrop->DefaultDragVisual->SetRenderScale(FVector2D(0.75));
	return dragDrop;
}