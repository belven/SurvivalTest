#include "RecipeListUI.h"

#include "Components/VerticalBox.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/HelperFunctions.h"

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

ABasePlayerController* URecipeListUI::GetBasePlayerController()
{
	return UHelperFunctions::CastValue(basePlayerController, GetOwningPlayer());
}

void URecipeListUI::OnItemUpdated(const FInstanceItemData& inItem, const FInstanceItemData& oldItem)
{
	CheckInventory(oldItem.itemID);
	CheckInventory(inItem.itemID);

	//for (URecipeUI* recipe : GetRecipes())
	//{
	//	if (recipe->GetRecipe().recipe.type == ERecipeType::Automatic)
	//	{
	//		while (CheckInventoryForRecipe(recipe->GetRecipe())) 
	//		{
	//			// Check for recipes to enable / disable
	//		}
	//	}
	//}
}

URecipeListUI* URecipeListUI::CreateRecipeList(ABasePlayerController* controller, TArray<FFullRecipe> recipesToAdd)
{
	if (recipeListUIClass == NULL)
	{
		recipeListUIClass = LoadClass<UUserWidget>(controller, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/RecipeListUI_BP.RecipeListUI_BP_C'"));
		recipeUIClass = LoadClass<UUserWidget>(controller, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/RecipeUI_BP.RecipeUI_BP_C'"));
	}

	URecipeListUI* recipeListUI = CreateWidget<URecipeListUI>(controller, recipeListUIClass);
	controller->GetBaseCharacter()->GetInventory()->OnItemUpdated.AddUniqueDynamic(recipeListUI, &URecipeListUI::OnItemUpdated);

	if (recipeListUI->GetRecipeListContainer())
	{
		for (FFullRecipe fr : recipesToAdd)
		{
			recipeListUI->CreateRecipeUI(controller, fr);
		}
	}
	return recipeListUI;
}

URecipeUI* URecipeListUI::CreateRecipeUI(ABasePlayerController* controller, FFullRecipe fr)
{
	URecipeUI* recipeUI = CreateWidget<URecipeUI>(controller, recipeUIClass);
	recipeUI->SetRecipe(fr);
	recipeUI->OnRecipeSelectionChanged.AddUniqueDynamic(this, &URecipeListUI::RecipeSelectionChanged);
	GetRecipeListContainer()->AddChildToVerticalBox(recipeUI);
	recipes.Add(recipeUI);
	DetermineRecipeEnabledState(controller->GetBaseCharacter()->GetInventory(), recipeUI);
	return recipeUI;
}

// ReSharper disable once IdentifierTypo
void URecipeListUI::DetermineRecipeEnabledState(UInventory* inventory, URecipeUI* recipe, int32 itemChanged)
{
	if (itemChanged != -1) 
	{
		bool enabled = true;
		for (FInputOutputData iod : recipe->GetRecipe().inputs)
		{
			if (iod.type == EInputOutputType::Item)
			{
				if (inventory->GetItemAmount(iod.inputOutputID) < iod.amount)
				{
					enabled = false;
					break;
				}
			}
		}

		if (recipe->IsRecipeEnabled() != enabled) 
		{
			recipe->UpdateRecipeEnabled(enabled);
		}
	}
}

void URecipeListUI::CheckInventory(int32 itemChanged)
{
	UInventory* inventory = GetBasePlayerController()->GetBaseCharacter()->GetInventory();

	for (URecipeUI* recipe : GetRecipes())
	{
		DetermineRecipeEnabledState(inventory, recipe, itemChanged);
	}
}

bool URecipeListUI::CheckInventoryForRecipe(FFullRecipe recipe)
{
	bool result = true;
	UInventory* inventory = GetBasePlayerController()->GetBaseCharacter()->GetInventory();

	for (FInputOutputData iod : recipe.inputs)
	{
		int32 total = inventory->GetItemAmount(iod.inputOutputID);

		if (iod.type == EInputOutputType::Item && total < iod.amount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player missing item %s for recipe %s"), *GetBasePlayerController()->GetBaseGameInstance()->GetItemData(iod.inputOutputID).name, *recipe.recipe.name);
			result = false;
			break;
		}
	}
	return result;
}

FFullRecipe URecipeListUI::GetRecipe(int32 recipeID)
{
	FFullRecipe fr;

	for (URecipeUI* recipe : recipes)
	{
		if (recipe->GetRecipe().recipe.ID == recipeID)
		{
			fr = recipe->GetRecipe();
		}
	}
	return fr;
}
