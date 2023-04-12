#include "BasePlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Items/Weapon.h"
#include "Items/WeaponCreator.h"
#include "UI/InventoryUI.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/BaseCharacter.h"

class ABaseCharacter;

ABasePlayerController::ABasePlayerController() : Super()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> inventoryWidgetClassFound(TEXT("WidgetBlueprint'/Game/FirstPerson/Blueprints/UI/InventoryUI_BP.InventoryUI_BP_C'"));

	if (inventoryWidgetClassFound.Class != nullptr)
	{
		inventoryWidgetClass = inventoryWidgetClassFound.Class;
	}
}

void ABasePlayerController::ContainersUpdated()
{
	if (inventoryWidget && inventoryWidget->GetVisibility() == ESlateVisibility::Visible)
		inventoryWidget->GenerateInventory();
}

void ABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);


	if (performAction && GetBaseCharacter()->GetEquippedWeapon() != nullptr) {
		GetBaseCharacter()->GetEquippedWeapon()->UseWeapon(GetBaseCharacter()->GetActorRotation().Vector());
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
	if(iid.ID != UItemStructs::InvalidInt && (!equippedWeapon || equippedWeapon->GetWeaponData().gearType != type))
	{
		GetBaseCharacter()->SetEquippedWeapon(UWeaponCreator::CreateWeapon(iid.itemID, GetWorld()));
	}
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ABasePlayerController::OnPrimaryAction);
	InputComponent->BindAction("PrimaryAction", IE_Released, this, &ABasePlayerController::OnPrimaryActionReleased);
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
	if (inventoryWidget->GetVisibility() == ESlateVisibility::Hidden) {
		performAction = true;
	}
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

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
