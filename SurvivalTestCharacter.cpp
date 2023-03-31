#include "SurvivalTestCharacter.h"
#include "SurvivalTestProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"

ASurvivalTestCharacter::ASurvivalTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	maxStats.health = 100.0f;
	maxStats.water = 100.0f;
	maxStats.hunger = 100.0f;
	maxStats.rest = 100.0f;

	// Seconds
	constexpr float minute = 60.0f;
	constexpr float hour = 60.0f * minute;
	constexpr float dayLengthSeconds = minute * 0.3;

	// Rate per Second
	maxStats.waterLossRate = maxStats.water / dayLengthSeconds;
	maxStats.hungerLossRate = maxStats.hunger / (dayLengthSeconds * 3.0f);
	maxStats.restLossRate = maxStats.rest / (dayLengthSeconds * 5.0f);

	currentStats.CopyStats(maxStats);
}

void ASurvivalTestCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASurvivalTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ASurvivalTestCharacter::OnPrimaryAction);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASurvivalTestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASurvivalTestCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
}

void ASurvivalTestCharacter::DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds)
{
	if (stat > 0) {
		stat -= drainRate * deltaSeconds;
	}
	else
	{
		stat = 0;
		currentStats.health -= healthDamage * deltaSeconds;
	}
}

void ASurvivalTestCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (currentStats.health > 0)
	{
		DrainStat(currentStats.water, currentStats.waterLossRate, 0.5f, DeltaSeconds);
		DrainStat(currentStats.rest, currentStats.restLossRate, 0.25f, DeltaSeconds);
		DrainStat(currentStats.hunger, currentStats.hungerLossRate, 0.1f, DeltaSeconds);	
	}
}

void ASurvivalTestCharacter::OnPrimaryAction()
{
	OnUseItem.Broadcast();
}

void ASurvivalTestCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASurvivalTestCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}