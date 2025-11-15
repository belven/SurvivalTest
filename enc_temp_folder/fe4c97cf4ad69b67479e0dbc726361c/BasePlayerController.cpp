#include "BasePlayerController.h"

#include "HelperFunctions.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Items/Weapon.h"
#include "Items/WeaponCreator.h"
#include "UI/InventoryUI.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Events/RPGEventManager.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Items/ProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Tasks/EquipmentSwapTask.h"
#include "Tasks/ReloadTask.h"
#include "Tasks/TaskManagerComponent.h"
#include "BaseBuilding/BuildingPart.h"
#include "Tables/Items/InputOutputDataTable.h"
#include "Tables/Items/RecipeInputOutputDataTable.h"
#include "Tables/Items/RecipeTable.h"
#include "UI/HUDUI.h"

class ABaseCharacter;

ABasePlayerController::ABasePlayerController() : Super(FObjectInitializer::Get()), RotateValue(0), CurveFloatValue(0), TimelineValue(0), performAction(false), useEquipment(false),
isReloading(false), reloadTask(nullptr), equipmentSwapTask(nullptr), rangedWeapon(nullptr), leanCurve(nullptr),
mainHUD(nullptr), inventoryWidget(nullptr), baseCharacter(nullptr)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> inventoryWidgetClassFound(TEXT("WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/InventoryUI_BP.InventoryUI_BP_C'"));

	if (inventoryWidgetClassFound.Class != nullptr)
	{
		inventoryWidgetClass = inventoryWidgetClassFound.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> mainHUDClassFound(TEXT("WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/HUDUI_BP.HUDUI_BP_C'"));

	if (mainHUDClassFound.Class != nullptr)
	{
		mainHUDClass = mainHUDClassFound.Class;
	}
}

void ABasePlayerController::TimelineCallback()
{
	TimelineValue = leanTimeline.GetPlaybackPosition();
	CurveFloatValue = leanCurve->GetFloatValue(TimelineValue);
	FVector cameraPosition;

	FVector cameraRelativeLocation = GetBaseCharacter()->GetBaseCameraComponent()->GetRelativeLocation();

	if (leanDirection == 0)
	{
		cameraPosition = FMath::Lerp(cameraRelativeLocation, GetBaseCharacter()->cameraCenter, TimelineValue);
	}
	else if (leanDirection == 1)
	{
		cameraPosition = FMath::Lerp(cameraRelativeLocation, GetBaseCharacter()->rightLean, TimelineValue);
	}
	else
	{
		cameraPosition = FMath::Lerp(cameraRelativeLocation, GetBaseCharacter()->leftLean, TimelineValue);
	}

	GetBaseCharacter()->GetBaseCameraComponent()->SetRelativeLocation(cameraPosition);
}

void ABasePlayerController::TimelineFinishedCallback()
{
	//leanTimeline->Deactivate();
}

void ABasePlayerController::ContainersUpdated()
{
	// TODO This might be broken now...
	if (inventoryWidget && inventoryWidget->IsVisible()) {
		inventoryWidget->GenerateInventory();
	}
}

void ABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	leanTimeline.TickTimeline(DeltaTime);

	if (useEquipment && mCurrentWeapon())
	{
		mCurrentWeapon()->UseWeapon(PlayerCameraManager->GetCameraRotation());
	}
}

void ABasePlayerController::EnemyHit(ABaseCharacter* enemy)
{
	if (mainHUD)
	{
		mainHUD->EnemyHit(enemy);
	}
}

void ABasePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	baseCharacter = Cast<ABaseCharacter>(aPawn);
	baseCharacter->SetIsPlayer(true);
	baseCharacter->GetInventory()->OnContainersUpdated.AddUniqueDynamic(this, &ABasePlayerController::ContainersUpdated);
	baseCharacter->OnWeaponEquipped.AddUniqueDynamic(this, &ABasePlayerController::WeaponEquipped);
	baseCharacter->OnEnemyHit.AddUniqueDynamic(this, &ABasePlayerController::EnemyHit);
	baseCharacter->OnCharacterDied.AddUniqueDynamic(this, &ABasePlayerController::CharacterDied);

	if (baseCharacter->GetEquippedWeapon())
	{
		WeaponEquipped(nullptr);
	}

	reloadTask = NewObject<UReloadTask>();

	InputComponent->BindAction("Jump", IE_Pressed, GetCharacter(), &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, GetCharacter(), &ACharacter::StopJumping);
	InputComponent->BindAxis("Turn Right / Left Mouse", GetCharacter(), &APawn::AddControllerYawInput);
	InputComponent->BindAxis("Look Up / Down Mouse", GetCharacter(), &APawn::AddControllerPitchInput);
}

void ABasePlayerController::OutOfAmmo()
{
	Reload();
}

void ABasePlayerController::Craft()
{
	UTableManager* tableManager = GetBaseGameInstance()->GetTableManager();
	TArray<FFullRecipe> recipes;

	for (FRecipeData recipeData : tableManager->GetRecipeTableData()->GetData())
	{
		FFullRecipe fullRecipe;
		fullRecipe.recipe = recipeData;

		for (FRecipeInputOutputData riod : tableManager->GetRecipeInputOutputDataTableData()->GetData())
		{
			if (riod.recipeID == recipeData.ID)
			{
				for (FInputOutputData iod : tableManager->GetInputOutputDataTableData()->GetData()) {
					if (iod.ID == riod.inputOutputDataID) {
						switch (iod.inputOrOutput)
						{
						case EInputOrOutput::Input:
							fullRecipe.inputs.Add(iod);
							break;
						case EInputOrOutput::Output:
							fullRecipe.inputs.Add(iod);
							break;
						case EInputOrOutput::End:
							break;
						}
					}
				}
			}
		}

		recipes.Add(fullRecipe);
	}

	for (FFullRecipe fr : recipes)
	{
		for (FInputOutputData iod : fr.outputs) {
			TArray<int32> ids;
			FInstanceItemData iid;
			iid.itemID = iod.inputOutputID;
			iid.amount = iod.amount;
			GetBaseCharacter()->GetInventory()->AddItem(iid, ids);
		}
	}
}

void ABasePlayerController::ReloadComplete()
{
	if (performAction) {
		useEquipment = true;
	}
}

void ABasePlayerController::WeaponEquipped(UWeapon* oldWeapon)
{
	if (oldWeapon && oldWeapon->IsProjectileWeapon())
	{
		UProjectileWeapon* pw = Cast<UProjectileWeapon>(oldWeapon);
		pw->OnOutOfAmmo.RemoveAll(this);
		pw->OnReloadComplete.RemoveAll(this);
	}

	UWeapon* weapon = mCurrentWeapon();

	if (weapon && weapon->IsProjectileWeapon())
	{
		rangedWeapon = Cast<UProjectileWeapon>(weapon);
		rangedWeapon->OnOutOfAmmo.AddUniqueDynamic(this, &ABasePlayerController::OutOfAmmo);
		rangedWeapon->OnReloadComplete.AddUniqueDynamic(this, &ABasePlayerController::ReloadComplete);
	}

	if (performAction)
	{
		useEquipment = true;
	}
}

void ABasePlayerController::Reload()
{
	if (rangedWeapon)
	{
		if (GetBaseCharacter()->GetTaskManager()->PerformTask(reloadTask, false))
		{
			useEquipment = false;
		}
	}
}

void ABasePlayerController::OnPrimaryActionReleased()
{
	performAction = false;
	useEquipment = false;
}

void ABasePlayerController::OnPrimaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetInventory()->GetPrimaryWeaponSlot(), EGearType::Weapon);
}

void ABasePlayerController::OnSecondaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetInventory()->GetSecondaryWeaponSlot(), EGearType::Weapon);
}

void ABasePlayerController::OnSidearm()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetInventory()->GetSidearmWeaponSlot(), EGearType::Sidearm);
}

void ABasePlayerController::EquipWeaponAtSlot(int32 slot, EGearType type)
{
	if (!equipmentSwapTask)
	{
		equipmentSwapTask = NewObject<UEquipmentSwapTask>();
	}

	equipmentSwapTask->SetSlot(slot);
	useEquipment = false;
	GetBaseCharacter()->GetTaskManager()->PerformTask(equipmentSwapTask, false);
}

void ABasePlayerController::LeanRight()
{
	leanDirection = 1;
	leanTimeline.PlayFromStart();
}

void ABasePlayerController::LeanLeft()
{
	leanDirection = -1;
	leanTimeline.PlayFromStart();
}

void ABasePlayerController::LeanCenter()
{
	leanDirection = 0;
	leanTimeline.PlayFromStart();
}

void ABasePlayerController::Sprint()
{
	if (!GetBaseCharacter()->IsSprinting())
	{
		GetBaseCharacter()->StartSprinting();
	}
	else
	{
		GetBaseCharacter()->StopSprinting();
	}
}

void ABasePlayerController::Build()
{
	TArray<AActor*> actors;
	GetBaseCharacter()->GetOverlappingActors(actors, ABuildingPart::StaticClass());

	if (!actors.IsEmpty()) {
		for (AActor* actor : actors)
		{
			ABuildingPart* bp = Cast<ABuildingPart>(actor);
			CreateBuildingPart(bp, EOneDirection::North);
			CreateBuildingPart(bp, EOneDirection::NorthEast);
			CreateBuildingPart(bp, EOneDirection::NorthWest);
			CreateBuildingPart(bp, EOneDirection::South);
			CreateBuildingPart(bp, EOneDirection::SouthEast);
			CreateBuildingPart(bp, EOneDirection::SouthWest);
			CreateBuildingPart(bp, EOneDirection::East);
			CreateBuildingPart(bp, EOneDirection::West);
		}
	}
	else
	{

	}

}

void ABasePlayerController::CreateBuildingPart(ABuildingPart* bp, EOneDirection direction)
{
	FRotator rotation;
	FVector location;

	if (bp != NULL) {
		location = bp->GetActorLocation();
		rotation = bp->GetActorRotation();
	}

	location = MoveVectorByDirection(location, rotation, 120, direction);

	FString location_string = FString::SanitizeFloat(location.X) + "X" + FString::SanitizeFloat(location.Y) + "Y" + FString::SanitizeFloat(location.Z) + "Z";

	TMap<FString, ABuildingPart*> building_parts = GetBaseGameInstance()->GetBuildingParts();

	if (!building_parts.Contains(location_string))
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		ABuildingPart* newBP = GetBaseCharacter()->GetWorld()->SpawnActor<ABuildingPart>(ABuildingPart::StaticClass(), location, rotation, params);

		if (newBP) {
			building_parts.Add(location_string, newBP);

			FString name;

			switch (direction)
			{
			case EOneDirection::North:
				name = "North";
				break;
			case EOneDirection::East:
				name = "East";
				break;
			case EOneDirection::West:
				name = "West";
				break;
			case EOneDirection::South:
				name = "South";
				break;
			case EOneDirection::End:
				name = "End";
				break;
			case EOneDirection::NorthEast:
				name = "NorthEast";
				break;
			case EOneDirection::NorthWest:
				name = "NorthWest";
				break;
			case EOneDirection::SouthEast:
				name = "SouthEast";
				break;
			case EOneDirection::SouthWest:
				name = "SouthWest";
				break;
			}

			if (newBP) {
				newBP->SetActorLabel(name);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("building_parts building already exists"));
	}
}

FVector ABasePlayerController::MoveVectorByDirection(const FVector& centerLocation, const FRotator& currentRotation, float distance, EOneDirection direction)
{
	const FRotator ninety = FRotator(0, currentRotation.Yaw + 90, 0);
	FVector north = FVector(distance, 0, 0);
	FVector east = FVector(0, distance, 0);
	FVector out = centerLocation;

	out = currentRotation.RotateVector(out);
	north = currentRotation.RotateVector(north);
	east = currentRotation.RotateVector(east);

	switch (direction)
	{
	case EOneDirection::North:
		out += north;
		break;
	case EOneDirection::NorthWest:
		out += north;
		out -= east;
		break;
	case EOneDirection::NorthEast:
		out += east;
		out += north;
		break;
	case EOneDirection::East:
		north = ninety.RotateVector(north);
		out += north;
		break;
	case EOneDirection::West:
		north = ninety.RotateVector(north);
		out -= north;
		break;
	case EOneDirection::South:
		out -= north;
		break;
	case EOneDirection::SouthEast:
		out -= north;
		out += east;
		break;
	case EOneDirection::SouthWest:
		out -= north;
		out -= east;
		break;
	case EOneDirection::End:
		break;
	}

	return out;
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("Build", IE_Pressed, this, &ABasePlayerController::Build);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ABasePlayerController::Reload);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ABasePlayerController::Sprint);
	InputComponent->BindAction("Craft", IE_Released, this, &ABasePlayerController::Craft);

	InputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ABasePlayerController::OnPrimaryAction);
	InputComponent->BindAction("PrimaryAction", IE_Released, this, &ABasePlayerController::OnPrimaryActionReleased);

	InputComponent->BindAction("Lean Right", IE_Pressed, this, &ABasePlayerController::LeanRight);
	InputComponent->BindAction("Lean Left", IE_Pressed, this, &ABasePlayerController::LeanLeft);

	InputComponent->BindAction("Lean Right", IE_Released, this, &ABasePlayerController::LeanCenter);
	InputComponent->BindAction("Lean Left", IE_Released, this, &ABasePlayerController::LeanCenter);

	InputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &ABasePlayerController::OnPrimaryWeapon);
	InputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &ABasePlayerController::OnSecondaryWeapon);
	InputComponent->BindAction("Sidearm", IE_Pressed, this, &ABasePlayerController::OnSidearm);
	InputComponent->BindAction("Load Inventories", IE_Pressed, this, &ABasePlayerController::LoadInventories);
	InputComponent->BindAction("Show Cursor", IE_Pressed, this, &ABasePlayerController::ShowCursor);

	InputComponent->BindAxis("Move Forward / Backward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("Move Right / Left", this, &ABasePlayerController::MoveRight);
}

void ABasePlayerController::ShowCursor()
{
	bShowMouseCursor = !bShowMouseCursor;
}

void ABasePlayerController::OnPrimaryAction()
{
	// Don't fire if we're in our inventory
	if (inventoryWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		performAction = true;
		useEquipment = true;
	}
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat TimelineCallback;
	FOnTimelineEventStatic TimelineFinishedCallback;
	leanCurve = NewObject<UCurveFloat>();

	TimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
	TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
	leanTimeline.AddInterpFloat(leanCurve, TimelineCallback);
	leanTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

	if (inventoryWidgetClass)
	{
		inventoryWidget = CreateWidget<UInventoryUI>(this, inventoryWidgetClass);
		inventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		inventoryWidget->AddToViewport(1);
		inventoryWidget->SetBaseGameInstance(GetBaseGameInstance());
		inventoryWidget->SetController(this);
	}

	if (mainHUDClass)
	{
		mainHUD = CreateWidget<UHUDUI>(this, mainHUDClass);
		mainHUD->SetPlayer(GetBaseCharacter());
		mainHUD->SetOwningPlayer(this);
		mainHUD->AddToViewport();
	}
}

UBaseGameInstance* ABasePlayerController::GetBaseGameInstance()
{
	if (baseGameInstance == NULL)
	{
		baseGameInstance = GameInstance(GetBaseCharacter()->GetWorld());
	}

	return baseGameInstance;
}

void ABasePlayerController::CharacterDied(ABaseCharacter* deadCharacter)
{
	// TODO might need to stop drag actions here
	CloseInventory();
	DisableInput(this);
	performAction = false;
	GetBaseCharacter()->StopSprinting();
	GetBaseCharacter()->GetMovementComponent()->StopMovementImmediately();
	SetActorTickEnabled(false);
	GetBaseCharacter()->GetGame()->GetEventManager()->OnEventTriggered.RemoveAll(this);

	baseCharacter->GetInventory()->OnContainersUpdated.RemoveAll(this);
	baseCharacter->OnWeaponEquipped.RemoveAll(this);
	baseCharacter->OnEnemyHit.RemoveAll(this);
	baseCharacter->OnCharacterDied.RemoveAll(this);
	// TODO add death task
	//GetBaseCharacter()->GetTaskManager().
}

void ABasePlayerController::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		GetCharacter()->AddMovementInput(GetCharacter()->GetActorForwardVector(), Val);
	}
}

void ABasePlayerController::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		GetCharacter()->AddMovementInput(GetCharacter()->GetActorRightVector(), Val);
	}
}

void ABasePlayerController::OpenInventory()
{
	if (inventoryWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		inventoryWidget->SetVisibility(ESlateVisibility::Visible);
		inventoryWidget->GenerateInventory();

		for (IInteractable* i : GetBaseCharacter()->GetOverlappingInteractables())
		{
			// TODO make the interaction based on inventories only
			i->Interact(this);
		}

		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, inventoryWidget);
		bShowMouseCursor = true;
	}
}

void ABasePlayerController::CloseInventory()
{
	if (inventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		inventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		bShowMouseCursor = false;
	}
}

void ABasePlayerController::LoadInventories()
{
	if (inventoryWidget)
	{
		if (inventoryWidget->GetVisibility() == ESlateVisibility::Hidden)
		{
			OpenInventory();
		}
		else
		{
			CloseInventory();
		}
	}
}