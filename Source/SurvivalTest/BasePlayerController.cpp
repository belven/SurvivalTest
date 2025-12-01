#include "BasePlayerController.h"
#include "HelperFunctions.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "UI/InventoryUI.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Events/RPGEventManager.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CombatComponent.h"
#include "UI/HUDUI.h"

class ABaseCharacter;

ABasePlayerController::ABasePlayerController() : Super(FObjectInitializer::Get()), RotateValue(0), CurveFloatValue(0), TimelineValue(0), leanCurve(nullptr),
mainHUD(nullptr), inventoryWidget(nullptr), baseCharacter(nullptr), baseGameInstance(nullptr)
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

void ABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	//leanTimeline.TickTimeline(DeltaTime);
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
	baseCharacter->OnEnemyHit.AddUniqueDynamic(this, &ABasePlayerController::EnemyHit);
	baseCharacter->OnCharacterDied.AddUniqueDynamic(this, &ABasePlayerController::CharacterDied);

	InputComponent->BindAction("Jump", IE_Pressed, GetCharacter(), &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, GetCharacter(), &ACharacter::StopJumping);
	InputComponent->BindAxis("Turn Right / Left Mouse", GetCharacter(), &APawn::AddControllerYawInput);
	InputComponent->BindAxis("Look Up / Down Mouse", GetCharacter(), &APawn::AddControllerPitchInput);

	AddInstanceComponent(UCombatComponent::CreateCombatComponent(this, baseCharacter));
}


void ABasePlayerController::Craft()
{
	// TODO make crafting UI
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

	//InputComponent->BindAction("Build", IE_Pressed, this, &ABasePlayerController::Build);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &ABasePlayerController::Sprint);
	InputComponent->BindAction("Craft", IE_Released, this, &ABasePlayerController::Craft);

	InputComponent->BindAction("Lean Right", IE_Pressed, this, &ABasePlayerController::LeanRight);
	InputComponent->BindAction("Lean Left", IE_Pressed, this, &ABasePlayerController::LeanLeft);

	InputComponent->BindAction("Lean Right", IE_Released, this, &ABasePlayerController::LeanCenter);
	InputComponent->BindAction("Lean Left", IE_Released, this, &ABasePlayerController::LeanCenter);


	InputComponent->BindAction("Load Inventories", IE_Pressed, this, &ABasePlayerController::LoadInventories);
	InputComponent->BindAction("Show Cursor", IE_Pressed, this, &ABasePlayerController::ShowCursor);

	InputComponent->BindAxis("Move Forward / Backward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("Move Right / Left", this, &ABasePlayerController::MoveRight);
}

void ABasePlayerController::ShowCursor()
{
	bShowMouseCursor = !bShowMouseCursor;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//FOnTimelineFloat TimelineCallback;
	//FOnTimelineEventStatic TimelineFinishedCallback;
	//leanCurve = NewObject<UCurveFloat>();

	//TimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
	//TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
	//leanTimeline.AddInterpFloat(leanCurve, TimelineCallback);
	//leanTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

	if (inventoryWidgetClass)
	{
		inventoryWidget = CreateWidget<UInventoryUI>(this, inventoryWidgetClass);
		inventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		inventoryWidget->AddToViewport(1);
		inventoryWidget->SetupInventory(this, GetBaseGameInstance());
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
		baseGameInstance = GameInstance(GetWorld());
	}

	return baseGameInstance;
}

void ABasePlayerController::CharacterDied(ABaseCharacter* deadCharacter)
{
	// TODO might need to stop drag actions here
	CloseInventory();
	DisableInput(this);
	GetBaseCharacter()->StopSprinting();
	GetBaseCharacter()->GetMovementComponent()->StopMovementImmediately();
	SetActorTickEnabled(false);
	GetBaseCharacter()->GetGame()->GetEventManager()->OnEventTriggered.RemoveAll(this);

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