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
#include "GameFramework/PawnMovementComponent.h"
#include "Items/ProjectileWeapon.h"
#include "UI/HUDUI.h"

class ABaseCharacter;

ABasePlayerController::ABasePlayerController() : Super()
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

	if (leanDirection == 0)
	{
		cameraPosition = FMath::Lerp(GetBaseCharacter()->GetBaseCameraComponent()->GetRelativeLocation(), GetBaseCharacter()->cameraCenter, TimelineValue);
	}
	else if (leanDirection == 1)
	{
		cameraPosition = FMath::Lerp(GetBaseCharacter()->GetBaseCameraComponent()->GetRelativeLocation(), GetBaseCharacter()->rightLean, TimelineValue);
	}
	else
	{
		cameraPosition = FMath::Lerp(GetBaseCharacter()->GetBaseCameraComponent()->GetRelativeLocation(), GetBaseCharacter()->leftLean, TimelineValue);
	}

	GetBaseCharacter()->GetBaseCameraComponent()->SetRelativeLocation(cameraPosition);
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
		mCurrentWeapon()->UseWeapon(PlayerCameraManager->GetCameraRotation());
	}
}

void ABasePlayerController::EnemyHit(ABaseCharacter* enemy)
{
	if(mainHUD)
	{
		mainHUD->EnemyHit(enemy);
	}
}

void ABasePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	baseCharacter = Cast<ABaseCharacter>(aPawn);
	baseCharacter->OnContainersUpdated.AddUniqueDynamic(this, &ABasePlayerController::ContainersUpdated);
	baseCharacter->OnWeaponEquipped.AddUniqueDynamic(this, &ABasePlayerController::WeaponEquipped);
	baseCharacter->OnEnemyHit.AddUniqueDynamic(this, &ABasePlayerController::EnemyHit);

	if (baseCharacter->GetEquippedWeapon())
	{
		WeaponEquipped(nullptr);
	}

	InputComponent->BindAction("Jump", IE_Pressed, GetCharacter(), &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, GetCharacter(), &ACharacter::StopJumping);
	InputComponent->BindAxis("Turn Right / Left Mouse", GetCharacter(), &APawn::AddControllerYawInput);
	InputComponent->BindAxis("Look Up / Down Mouse", GetCharacter(), &APawn::AddControllerPitchInput);
}

void ABasePlayerController::OutOfAmmo()
{
	Reload();
}

void ABasePlayerController::ReloadComplete()
{

}

void ABasePlayerController::WeaponEquipped(UWeapon* oldWeapon)
{
	if (oldWeapon && oldWeapon->GetWeaponData().type == EWeaponType::Projectile)
	{
		UProjectileWeapon* pw = Cast<UProjectileWeapon>(oldWeapon);
		pw->OnOutOfAmmo.RemoveAll(this);
		pw->OnReloadComplete.RemoveAll(this);
	}

	UWeapon* weapon = GetBaseCharacter()->GetEquippedWeapon();
	
	if (weapon && weapon->GetWeaponData().type == EWeaponType::Projectile)
	{
		UProjectileWeapon* pw = Cast<UProjectileWeapon>(weapon);
		pw->OnOutOfAmmo.AddUniqueDynamic(this, &ABasePlayerController::OutOfAmmo);
		pw->OnReloadComplete.AddUniqueDynamic(this, &ABasePlayerController::ReloadComplete);
	}
}

void ABasePlayerController::Reload()
{
	UWeapon* weapon = GetBaseCharacter()->GetEquippedWeapon();

	if (weapon	&& weapon->GetWeaponData().type == EWeaponType::Projectile)
	{
		UProjectileWeapon* pw = Cast<UProjectileWeapon>(weapon);
		pw->Reload();
	}
}

bool ABasePlayerController::HasAmmoForWeapon()
{
	UWeapon* weapon = GetBaseCharacter()->GetEquippedWeapon();

	if (weapon)
	{
		if (weapon->GetWeaponData().type == EWeaponType::Projectile)
		{
			UProjectileWeapon* pw = Cast<UProjectileWeapon>(weapon);

			return GetBaseCharacter()->GetInventory()->GetItemAmount(pw->GetProjectileWeaponData().ammoID) > 0;
		}
		return true;
	}
	return false;
}

void ABasePlayerController::OnPrimaryActionReleased()
{
	performAction = false;
}

void ABasePlayerController::OnPrimaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetPrimaryWeaponSlot(), EGearType::Weapon);
}

void ABasePlayerController::OnSecondaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetSecondaryWeaponSlot(), EGearType::Weapon);
}

void ABasePlayerController::OnSidearm()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetSidearmWeaponSlot(), EGearType::Sidearm);
}

void ABasePlayerController::EquipWeaponAtSlot(int32 slot, EGearType type)
{
	FInstanceItemData iid = GetBaseCharacter()->GetInventory()->GetInstanceItemAtSlot(slot);

	UWeapon* equippedWeapon = mCurrentWeapon();
	
	if (iid.ID != UItemStructs::InvalidInt && (!equippedWeapon || equippedWeapon->GetInstanceWeaponData().instanceItemID != iid.ID))
	{
		GetBaseCharacter()->SetEquippedWeapon(UWeaponCreator::CreateWeapon(iid.itemID, GetWorld(), iid.ID));
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

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ABasePlayerController::Reload);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ABasePlayerController::Sprint);

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
		inventoryWidget->AddToViewport(1);
		inventoryWidget->SetBaseGameInstance(GameInstance(GetWorld()));
		inventoryWidget->SetController(this);
	}

	if(mainHUDClass)
	{
		mainHUD = CreateWidget<UHUDUI>(this, mainHUDClass);
		mainHUD->SetPlayer(GetBaseCharacter());
		mainHUD->SetOwningPlayer(this);
		mainHUD->AddToViewport();
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

void ABasePlayerController::OpenInventory()
{
	if (inventoryWidget->GetVisibility() == ESlateVisibility::Hidden)
	{
		inventoryWidget->SetVisibility(ESlateVisibility::Visible);
		inventoryWidget->GenerateInventory();
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