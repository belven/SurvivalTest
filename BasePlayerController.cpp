#include "BasePlayerController.h"
#include "GameFramework/Character.h"

void ABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ABasePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);	

	InputComponent->BindAction("Jump", IE_Pressed, GetCharacter(), &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, GetCharacter(), &ACharacter::StopJumping);
	InputComponent->BindAxis("Turn Right / Left Mouse", GetCharacter(), &APawn::AddControllerYawInput);
	InputComponent->BindAxis("Look Up / Down Mouse", GetCharacter(), &APawn::AddControllerPitchInput);
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	
	InputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ABasePlayerController::OnPrimaryAction);
	InputComponent->BindAxis("Move Forward / Backward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("Move Right / Left", this, &ABasePlayerController::MoveRight);
}

void ABasePlayerController::OnPrimaryAction()
{
	OnUseItem.Broadcast();
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
