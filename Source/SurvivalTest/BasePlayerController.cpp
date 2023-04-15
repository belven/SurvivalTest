#include "BasePlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Items/Weapon.h"
#include "Items/WeaponCreator.h"
#include "UI/InventoryUI.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/BaseCharacter.h"
#include "Components/TimelineComponent.h"

class ABaseCharacter;

ABasePlayerController::ABasePlayerController() : Super()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> inventoryWidgetClassFound(TEXT("WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/InventoryUI_BP.InventoryUI_BP_C'"));

	if (inventoryWidgetClassFound.Class != nullptr)
	{
		inventoryWidgetClass = inventoryWidgetClassFound.Class;
	}

	//leanTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LeanTimeline"));
	//leanTimeline->CreationMethod = EComponentCreationMethod::Native;
}

void ABasePlayerController::TimelineCallback()
{
	TimelineValue = leanTimeline.GetPlaybackPosition();
	CurveFloatValue = leanCurve->GetFloatValue(TimelineValue);
	FVector cameraPosition;

	if (leanDirection == 0)
	{
		cameraPosition = FMath::Lerp(GetBaseCharacter()->GetFirstPersonCameraComponent()->GetRelativeLocation(), GetBaseCharacter()->cameraCenter, TimelineValue);
	}
	else if (leanDirection == 1)
	{
		cameraPosition = FMath::Lerp(GetBaseCharacter()->GetFirstPersonCameraComponent()->GetRelativeLocation(), GetBaseCharacter()->rightLean, TimelineValue);
	}
	else
	{
		cameraPosition = FMath::Lerp(GetBaseCharacter()->GetFirstPersonCameraComponent()->GetRelativeLocation(), GetBaseCharacter()->leftLean, TimelineValue);
	}

	GetBaseCharacter()->GetFirstPersonCameraComponent()->SetRelativeLocation(cameraPosition);
}

void ABasePlayerController::TimelineFinishedCallback()
{
	//leanTimeline->Deactivate();
}

void ABasePlayerController::ContainersUpdated()
{
	if (inventoryWidget && inventoryWidget->GetVisibility() == ESlateVisibility::Visible)
		inventoryWidget->GenerateInventory();
}

void ABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	leanTimeline.TickTimeline(DeltaTime);


	if (performAction && GetBaseCharacter()->GetEquippedWeapon() != nullptr)
	{
		GetBaseCharacter()->GetEquippedWeapon()->UseWeapon(PlayerCameraManager->GetCameraRotation().Vector());
	}
}

void ABasePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	baseCharacter = Cast<ABaseCharacter>(aPawn);
	baseCharacter->OnContainersUpdated.AddUniqueDynamic(this, &ABasePlayerController::ContainersUpdated);

	InputComponent->BindAction("Jump", IE_Pressed, GetCharacter(), &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, GetCharacter(), &ACharacter::StopJumping);
	InputComponent->BindAxis("Turn Right / Left Mouse", GetCharacter(), &APawn::AddControllerYawInput);
	InputComponent->BindAxis("Look Up / Down Mouse", GetCharacter(), &APawn::AddControllerPitchInput);
}

void ABasePlayerController::OnPrimaryActionReleased()
{
	performAction = false;
}

void ABasePlayerController::OnPrimaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetSlotForGear(EGearType::Primary_Weapon), EGearType::Primary_Weapon);
}

void ABasePlayerController::OnSecondaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetSlotForGear(EGearType::Secondary_Weapon), EGearType::Secondary_Weapon);
}

void ABasePlayerController::OnSidearm()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetSlotForGear(EGearType::Sidearm), EGearType::Sidearm);
}

void ABasePlayerController::EquipWeaponAtSlot(int32 slot, EGearType type)
{
	FInstanceItemData iid = GetBaseCharacter()->GetInventory()->GetInstanceItemAtSlot(slot);

	UWeapon* equippedWeapon = GetBaseCharacter()->GetEquippedWeapon();

	// TODO need to check if we are quipping the same weapon
	if (iid.ID != UItemStructs::InvalidInt && (!equippedWeapon || equippedWeapon->GetWeaponData().gearType != type))
	{
		GetBaseCharacter()->SetEquippedWeapon(UWeaponCreator::CreateWeapon(iid.itemID, GetWorld()));
	}
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

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

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
		inventoryWidget->AddToViewport();
		inventoryWidget->SetBaseGameInstance(GameInstance(GetWorld()));
		inventoryWidget->SetController(this);
	}
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

void ABasePlayerController::LoadInventories()
{
	if (inventoryWidget)
	{
		if (inventoryWidget->GetVisibility() == ESlateVisibility::Hidden)
		{
			inventoryWidget->SetVisibility(ESlateVisibility::Visible);
			inventoryWidget->GenerateInventory();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, inventoryWidget);
		}
		else
		{
			inventoryWidget->SetVisibility(ESlateVisibility::Hidden);
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
		}
		ShowCursor();
	}
}
