#include "RecipeUI.h"

#include "ItemUI.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/BasePlayerController.h"

UBaseGameInstance* URecipeUI::GetBaseGameInstance()
{
	if (baseGameInstance == NULL) {
		ABasePlayerController* basePlayerController = Cast<ABasePlayerController>(GetOwningPlayer());
		baseGameInstance = basePlayerController->GetBaseGameInstance();
	}

	return baseGameInstance;
}

void URecipeUI::AddItemToBox(FInputOutputData iod, UHorizontalBox* box)
{
	FItemData id = GetBaseGameInstance()->GetTableManager()->GetItemData(iod.inputOutputID);
	FInstanceItemData iid(id.ID, iod.amount);
	iid.ID = 30000;
	UItemUI* itemUI = CreateWidget<UItemUI>(this, itemUIClass);
	itemUI->UpdateItemData(iid, id, nullptr);
	itemUI->SetDisabled(true);
	box->AddChildToHorizontalBox(itemUI);
}

void URecipeUI::SetRecipe(const FFullRecipe& newRecipe)
{
	recipe = newRecipe;

	if (itemUIClass == NULL)
	{
		itemUIClass = LoadClass<UItemUI>(this, TEXT("/ Script / UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/ItemUI_BP.ItemUI_BP_C'"));
	}

	if (GetRecipeInputContainer() && GetRecipeOutputContainer()) {
		for (FInputOutputData iod : recipe.inputs)
		{
			if (iod.type == EInputOutputType::Item) {
				AddItemToBox(iod, GetRecipeInputContainer());
			}
		}

		for (FInputOutputData iod : recipe.outputs)
		{
			if (iod.type == EInputOutputType::Item) {
				AddItemToBox(iod, GetRecipeOutputContainer());
			}
		}

		UpdateRecpie();
	}
}

void URecipeUI::SetSelected(bool selectedState)
{
	selected = selectedState;

	if (selected)
	{
		Select();
	}
	else
	{
		Deselect();
	}

	OnRecipeSelectionChanged.Broadcast(this);
}

FReply URecipeUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetPressedButtons().Contains(EKeys::LeftMouseButton))
	{
		SetSelected(!selected);
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
