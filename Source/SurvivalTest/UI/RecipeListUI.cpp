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
				Craft_Internal(recipe);
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

	//for (FInputOutputData iod : recipe.outputs)
	//{
	//	TArray<FInstanceItemData> freeSpaceItems = inventory->GetExistingItemsWithSpace(iod.inputOutputID);

	//	if (iod.type == EInputOutputType::Item && total < iod.amount)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Player missing item %s for recipe %s"), *GetBasePlayerController()->GetBaseGameInstance()->GetTableManager()->GetItemData(iod.inputOutputID).name, *recipe.recipe.name);
	//		result = false;
	//		break;
	//	}
	//}
	return result;
}


void URecipeListUI::Craft_Internal(URecipeUI* recipe)
{
	if (recipe)
	{
		if (CheckInventoryForRecipe(recipe->GetRecipe()))
		{
			ConsumeRecipeInputs(recipe);

			if (!GetTimerManager()->IsTimerActive(TimerHandle_ItemCrafted))
			{
				mSetTimerWorld(GetBasePlayerController()->GetWorld(), TimerHandle_ItemCrafted, &URecipeListUI::CraftComplete, recipe->GetRecipe().recipe.craftingTime);
			}

			inProgressRecipes.Add(recipe);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No recipe selected"));
	}
}

void URecipeListUI::Craft()
{
	Craft_Internal(selectedRecipe);
}

void URecipeListUI::CraftComplete()
{
	if (!inProgressRecipes.IsEmpty())
	{
		// TODO make better use of pop and top etc.
		ProduceOutputs(inProgressRecipes[0]);

		inProgressRecipes.RemoveAt(0);

		if (!inProgressRecipes.IsEmpty())
		{
			mSetTimerWorld(GetBasePlayerController()->GetWorld(), TimerHandle_ItemCrafted, &URecipeListUI::CraftComplete, inProgressRecipes[0]->GetRecipe().recipe.craftingTime);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No recipes left to craft"));
	}
}

float URecipeListUI::GetProgress()
{
	if (IsInProgress() && !inProgressRecipes.IsEmpty())
	{
		return  GetTimerManager()->GetTimerElapsed(TimerHandle_ItemCrafted) / inProgressRecipes[0]->GetRecipe().recipe.craftingTime;
	}

	return 0;
}

bool URecipeListUI::IsInProgress()
{
	return GetTimerManager()->IsTimerActive(TimerHandle_ItemCrafted);
}

FTimerManager* URecipeListUI::GetTimerManager()
{
	if (timerManager == NULL)
	{
		timerManager = &GetBasePlayerController()->GetWorld()->GetTimerManager();
	}
	return timerManager;
}

void URecipeListUI::ConsumeRecipeInputs(URecipeUI* recipe)
{
	for (FInputOutputData iod : recipe->GetRecipe().inputs) {
		FInstanceItemData iid;
		iid.itemID = iod.inputOutputID;
		iid.amount = iod.amount;
		GetBasePlayerController()->GetBaseCharacter()->GetInventory()->RemoveItem(iid);
	}
}

void URecipeListUI::ProduceOutputs(URecipeUI* recipe)
{
	for (FInputOutputData iod : recipe->GetRecipe().outputs) {
		TArray<int32> ids;
		FInstanceItemData iid;
		iid.itemID = iod.inputOutputID;
		iid.amount = iod.amount;
		GetBasePlayerController()->GetBaseCharacter()->GetInventory()->AddItem(iid, ids);
	}
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

void URecipeListUI::CancelCrafting(FFullRecipe cancelledRecipe)
{
	if (IsInProgress())
	{
		// GetRecipe(FInProgressCrafting.RecipeID)
		for (FInputOutputData iod : cancelledRecipe.inputs)
		{
			if (iod.type == EInputOutputType::Item)
			{
				TArray<int32> ids;
				FInstanceItemData iid;
				iid.itemID = iod.inputOutputID;
				iid.amount = iod.amount;
				// TODO Start with own internal inventory, else add to players inventory
				GetBasePlayerController()->GetBaseCharacter()->GetInventory()->AddItem(iid, ids);
			}
		}

		GetTimerManager()->ClearTimer(TimerHandle_ItemCrafted);
		inProgressRecipes.RemoveAt(0);

		if (!inProgressRecipes.IsEmpty())
		{
			mSetTimerWorld(GetBasePlayerController()->GetWorld(), TimerHandle_ItemCrafted, &URecipeListUI::CraftComplete, inProgressRecipes[0]->GetRecipe().recipe.craftingTime);
		}
	}
}
