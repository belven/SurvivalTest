#include "CraftingDeviceUI.h"

#include "RecipeListUI.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/HelperFunctions.h"
#include "SurvivalTest/Items/ArmourCreator.h"

TSubclassOf<UUserWidget> UCraftingDeviceUI::craftingDeviceUIClass;

UCraftingDeviceUI* UCraftingDeviceUI::CreateCraftingDeviceUI(ABasePlayerController* controller, const FCraftingDevice& inCraftingDevice, const FInstanceCraftingDevice& inInstanceCraftingDevice)
{
	if (craftingDeviceUIClass == NULL)
	{
		craftingDeviceUIClass = LoadClass<UUserWidget>(controller, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/CraftingDeviceUI_BP.CraftingDeviceUI_BP_C'"));
	}

	UCraftingDeviceUI* craftingDeviceUI = CreateWidget<UCraftingDeviceUI>(controller, craftingDeviceUIClass);
	craftingDeviceUI->craftingDeviceData = inCraftingDevice;
	craftingDeviceUI->basePlayerController = controller;
	craftingDeviceUI->instanceCraftingDevice = inInstanceCraftingDevice;
	craftingDeviceUI->deviceRecipes = controller->GetBaseGameInstance()->GetTableManager()->GetRecipes(inCraftingDevice.ID);
	craftingDeviceUI->recipeListUI = URecipeListUI::CreateRecipeList(controller, craftingDeviceUI->deviceRecipes);
	craftingDeviceUI->inProgressRecipeListUI = URecipeListUI::CreateRecipeList(controller, {});

	if (craftingDeviceUI->GetRecipeUIPanelWidget() != NULL)
	{
		craftingDeviceUI->GetRecipeUIPanelWidget()->AddChild(craftingDeviceUI->recipeListUI);
		craftingDeviceUI->GetRecipeUIPanelWidget()->AddChild(craftingDeviceUI->inProgressRecipeListUI);
	}

	controller->GetBaseCharacter()->GetInventory()->OnItemUpdated.AddUniqueDynamic(craftingDeviceUI, &UCraftingDeviceUI::OnItemUpdated);
	return craftingDeviceUI;
}

void UCraftingDeviceUI::ConsumeRecipeInputs(FFullRecipe recipe)
{
	for (FInputOutputData iod : recipe.inputs) {
		FInstanceItemData iid;
		iid.itemID = iod.inputOutputID;
		iid.amount = iod.amount;
		GetBasePlayerController()->GetBaseCharacter()->GetInventory()->RemoveItem(iid);
	}
}

void UCraftingDeviceUI::ProduceOutputs(FFullRecipe recipe)
{
	for (FInputOutputData iod : recipe.outputs) {
		TArray<FInstanceItemData> ids;
		FItemsToAdd itemsToAdd(iod.inputOutputID, iod.amount);

		GetBasePlayerController()->GetBaseCharacter()->GetInventory()->AddItem(itemsToAdd, ids);

		UBaseGameInstance* baseGameInstance = GetBasePlayerController()->GetBaseGameInstance();
		FItemData id = baseGameInstance->GetTableManager()->GetItemData(itemsToAdd.itemID);

		if (itemsToAdd.amount == 0 && id.type == EItemType::Armour)
		{
			UArmourCreator::CreateArmourData(baseGameInstance, ids[0], id);
		}
		else if (itemsToAdd.amount == 0 && id.type == EItemType::Weapon)
		{
			FRangedWeaponData rwd = baseGameInstance->GetRangedWeaponData(id.ID);
			FProjectileWeaponData pwd = baseGameInstance->GetProjectileWeaponData(rwd.ID);
			baseGameInstance->GetTableManager()->CreateNewInstanceWeaponData(ids[0].ID, pwd);
		}
	}
}

float UCraftingDeviceUI::GetProgress()
{
	if (IsInProgress() && !craftingQueue.IsEmpty())
	{
		for (auto& item : craftingQueue)
		{
			return  GetTimerManager()->GetTimerElapsed(TimerHandle_ItemCrafted) / currentCraftingRecipe.recipe.craftingTime;
		}
	}

	return 0;
}

bool UCraftingDeviceUI::IsInProgress()
{
	return GetTimerManager()->IsTimerActive(TimerHandle_ItemCrafted);
}

void UCraftingDeviceUI::CraftComplete()
{
	if (!craftingQueue.IsEmpty())
	{
		// TODO make better use of pop and top etc.
		ProduceOutputs(currentCraftingRecipe);

		craftingQueue.RemoveAt(0);

		if (!craftingQueue.IsEmpty())
		{
			currentCraftingRecipe = GetRecipeByID(craftingQueue[0].RecipeID);
			mSetTimerWorld(GetBasePlayerController()->GetWorld(), TimerHandle_ItemCrafted, &UCraftingDeviceUI::CraftComplete, currentCraftingRecipe.recipe.craftingTime);
		}
	}
	else
	{
		currentCraftingRecipe = {};
	}
}

void UCraftingDeviceUI::CancelCrafting(FInProgressCrafting cancelledCraft)
{
	if (IsInProgress())
	{
		for (FInputOutputData iod : GetRecipeByID(cancelledCraft.RecipeID).inputs)
		{
			if (iod.type == EInputOutputType::Item)
			{
				TArray<FInstanceItemData> ids;
				FItemsToAdd itemsToAdd(iod.inputOutputID, iod.amount);
				// TODO Start with own internal inventory, else add to players inventory
				GetBasePlayerController()->GetBaseCharacter()->GetInventory()->AddItem(itemsToAdd, ids);
			}
		}

		GetTimerManager()->ClearTimer(TimerHandle_ItemCrafted);
		craftingQueue.RemoveAt(0);

		if (!craftingQueue.IsEmpty())
		{
			mSetTimerWorld(GetBasePlayerController()->GetWorld(), TimerHandle_ItemCrafted, &UCraftingDeviceUI::CraftComplete, currentCraftingRecipe.recipe.craftingTime);
		}
	}
}

FTimerManager* UCraftingDeviceUI::GetTimerManager()
{
	if (timerManager == NULL)
	{
		timerManager = &GetBasePlayerController()->GetWorld()->GetTimerManager();
	}
	return timerManager;
}

void UCraftingDeviceUI::OnItemUpdated(const FInstanceItemData& inItem, const FInstanceItemData& oldItem)
{
	for (FFullRecipe fullRecipe : deviceRecipes)
	{
		if (fullRecipe.recipe.type == ERecipeType::Automatic)
		{
			while (CheckInventoryForRecipe(fullRecipe))
			{
				Craft_Internal(fullRecipe);
			}
		}
	}
}

ABasePlayerController* UCraftingDeviceUI::GetBasePlayerController()
{
	return UHelperFunctions::CastValue(basePlayerController, GetOwningPlayer());
}

FFullRecipe UCraftingDeviceUI::GetRecipeByID(int32 ID)
{
	for (FFullRecipe recipe : deviceRecipes)
	{
		if (recipe.recipe.ID == ID)
		{
			return recipe;
		}
	}

	return {};
}

int32 UCraftingDeviceUI::GetQueuePosition()
{
	//if (craftingQueue.IsEmpty())
	//{
	//	return 0;
	//}
	//else {
	//	FInProgressCrafting ipc =
	//}
	return 0;
}


void UCraftingDeviceUI::CheckInventory(int32 itemChanged)
{
	UInventory* inventory = GetBasePlayerController()->GetBaseCharacter()->GetInventory();
	for (FFullRecipe recipe : deviceRecipes)
	{
		for (FInputOutputData iod : recipe.inputs)
		{
			if (iod.type == EInputOutputType::Item && iod.inputOutputID == itemChanged && inventory->GetItemAmount(iod.inputOutputID) >= iod.amount)
			{
				// Set recipe enabled...
			}
		}
	}
}

bool UCraftingDeviceUI::CheckInventoryForRecipe(const FFullRecipe& fullRecipe)
{
	bool result = true;
	UInventory* inventory = GetBasePlayerController()->GetBaseCharacter()->GetInventory();

	for (FInputOutputData iod : fullRecipe.inputs)
	{
		int32 total = inventory->GetItemAmount(iod.inputOutputID);

		if (iod.type == EInputOutputType::Item && total < iod.amount)
		{
			FString itemName = GetBasePlayerController()->GetBaseGameInstance()->GetTableManager()->GetItemData(iod.inputOutputID).name;
			UE_LOG(LogTemp, Warning, TEXT("Player missing item %s for recipe %s"), *itemName, *fullRecipe.recipe.name);
			result = false;
			break;
		}
	}
	return result;
}

void UCraftingDeviceUI::Craft()
{
	if (recipeListUI && recipeListUI->GetSelectedRecipe())
	{
		Craft_Internal(recipeListUI->GetSelectedRecipe()->GetRecipe());
	}
}

void UCraftingDeviceUI::Craft_Internal(const FFullRecipe& fullRecipe)
{
	ConsumeRecipeInputs(fullRecipe);

	if (!IsInProgress())
	{
		currentCraftingRecipe = fullRecipe;
		mSetTimerWorld(GetBasePlayerController()->GetWorld(), TimerHandle_ItemCrafted, &UCraftingDeviceUI::CraftComplete, fullRecipe.recipe.craftingTime);
	}

	craftingQueue.Add(FInProgressCrafting(-1, instanceCraftingDevice.ID, fullRecipe.recipe.ID, GetQueuePosition(), 0));

	inProgressRecipeListUI->CreateRecipeUI(GetBasePlayerController(), fullRecipe)->HideInputs();
}