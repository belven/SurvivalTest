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
	for (URecipeUI* recipe : GetRecipes())
	{
		if (recipe->GetRecipe().recipe.type == ERecipeType::Automatic)
		{
			while (CheckInventoryForRecipe(recipe->GetRecipe())) 
			{
				// Check for recipes to enable / disable
			}
		}
	}
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
	UInventory* inventory = GetBasePlayerController()->GetBaseCharacter()->GetInventory();
	for (URecipeUI* recipe : GetRecipes())
	{
		for (FInputOutputData iod : recipe->GetRecipe().inputs)
		{
			if (iod.type == EInputOutputType::Item && iod.inputOutputID == itemChanged && inventory->GetItemAmount(iod.inputOutputID) >= iod.amount)
			{
				// Set recipe enabled...
			}
		}
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
			UE_LOG(LogTemp, Warning, TEXT("Player missing item %s for recipe %s"), *GetBasePlayerController()->GetBaseGameInstance()->GetTableManager()->GetItemData(iod.inputOutputID).name, *recipe.recipe.name);
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
