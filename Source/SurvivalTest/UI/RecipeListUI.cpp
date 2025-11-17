#include "RecipeListUI.h"

#include "Components/VerticalBox.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"

TSubclassOf<UUserWidget> URecipeListUI::recipeListUIClass = NULL;
TSubclassOf<UUserWidget> URecipeListUI::recipeUIClass = NULL;


void URecipeListUI::RecipeSelectionChanged(URecipeUI* recipeUI)
{
	if (selectedRecipe != NULL)
	{
		selectedRecipe->Deselect();
	}

	selectedRecipe = recipeUI;
}

URecipeListUI* URecipeListUI::CreateRecipeList(ABasePlayerController* controller, TArray<FFullRecipe> recipesToAdd)
{
	if (recipeListUIClass == NULL)
	{
		recipeListUIClass = LoadClass<UUserWidget>(controller, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/RecipeListUI_BP.RecipeListUI_BP_C'"));
		recipeUIClass = LoadClass<UUserWidget>(controller, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/RecipeUI_BP.RecipeUI_BP_C'"));
	}

	URecipeListUI* recipeListUI = CreateWidget<URecipeListUI>(controller, recipeListUIClass);

	if (recipeListUI->GetRecipeListContainer())
	{
		for (FFullRecipe fr : recipesToAdd)
		{
			URecipeUI* recipeUI = CreateWidget<URecipeUI>(controller, recipeUIClass);
			recipeUI->SetRecipe(fr);
			recipeUI->OnRecipeSelectionChanged.AddUniqueDynamic(recipeListUI, &URecipeListUI::RecipeSelectionChanged);
			recipeListUI->GetRecipeListContainer()->AddChildToVerticalBox(recipeUI);
			recipeListUI->recipes.Add(recipeUI);

			UInventory* inventory = controller->GetBaseCharacter()->GetInventory();

			for (FInputOutputData iod : recipeUI->GetRecipe().inputs) 
			{
				if (iod.type == EInputOutputType::Item && inventory->GetItemAmount(iod.inputOutputID) >= iod.amount)
				{
					// Set recipe enabled...
				}
			}
		}
	}
	return recipeListUI;
}

void URecipeListUI::CheckInventory(int32 itemChanged) 
{
	ABasePlayerController* con = Cast<ABasePlayerController>(GetOwningPlayer());

	UInventory* inventory = con->GetBaseCharacter()->GetInventory();
	for (URecipeUI* recipe : GetRecipes())
	{
		for (FInputOutputData iod : recipe->GetRecipe().inputs)
		{
			if (iod.type == EInputOutputType::Item && iod.inputOutputID == itemChanged &&  inventory->GetItemAmount(iod.inputOutputID) >= iod.amount)
			{
				// Set recipe enabled...
			}
		}
	}
}

void URecipeListUI::Craft()
{
	if (selectedRecipe) 
	{
		ABasePlayerController* con = Cast<ABasePlayerController>(GetOwningPlayer());

		for (FInputOutputData iod : selectedRecipe->GetRecipe().inputs) {
			FInstanceItemData iid;
			iid.itemID = iod.inputOutputID;
			iid.amount = iod.amount;
			con->GetBaseCharacter()->GetInventory()->RemoveItem(iid);
		}

		for (FInputOutputData iod : selectedRecipe->GetRecipe().outputs) {
			TArray<int32> ids;
			FInstanceItemData iid;
			iid.itemID = iod.inputOutputID;
			iid.amount = iod.amount;
			con->GetBaseCharacter()->GetInventory()->AddItem(iid, ids);
		}
	}
}
