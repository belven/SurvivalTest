#include "CombatComponent.h"

#include "SurvivalTest/BaseAIController.h"
#include "SurvivalTest/HelperFunctions.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Items/ProjectileWeapon.h"
#include "SurvivalTest/Tasks/EquipmentSwapTask.h"
#include "SurvivalTest/Tasks/ReloadTask.h"
#include "SurvivalTest/Tasks/TaskManagerComponent.h"

UCombatComponent::UCombatComponent() : Super(FObjectInitializer::Get()), equipmentSwapTask(nullptr), reloadTask(nullptr), RotateValue(0), performAction(false), useEquipment(false), isReloading(false), rangedWeapon(nullptr), baseCharacter(nullptr), baseGameInstance(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (useEquipment && mCurrentWeapon())
	{
		mCurrentWeapon()->UseWeapon(GetBaseCharacter()->GetControlRotation());
	}
}

UCombatComponent* UCombatComponent::CreateCombatComponent(AController* controller, ABaseCharacter* character)
{	
	UCombatComponent* combatComponent = NewObject<UCombatComponent>(controller);
	combatComponent->SetBaseCharacter(character);

	character->OnWeaponEquipped.AddUniqueDynamic(combatComponent, &UCombatComponent::WeaponEquipped);

	TObjectPtr<UInputComponent> inputComponent = combatComponent->GetOwner()->InputComponent;

	if (ABaseAIController* aiController = Cast<ABaseAIController>(controller))
	{
		aiController->OnUseTool.AddUniqueDynamic(combatComponent, &UCombatComponent::OnPrimaryAction);
		aiController->OnStopUsingTool.AddUniqueDynamic(combatComponent, &UCombatComponent::OnPrimaryActionReleased);
		aiController->OnReload.AddUniqueDynamic(combatComponent, &UCombatComponent::Reload);
	}
	else 
	{
		if (ABasePlayerController* basePlayerController = Cast<ABasePlayerController>(controller))
		{
			basePlayerController->OnUIChangedState.AddUniqueDynamic(combatComponent, &UCombatComponent::OnUIStateChanged);
		}

		inputComponent->BindAction("PrimaryWeapon", IE_Pressed, combatComponent, &UCombatComponent::OnPrimaryWeapon);
		inputComponent->BindAction("SecondaryWeapon", IE_Pressed, combatComponent, &UCombatComponent::OnSecondaryWeapon);
		inputComponent->BindAction("Sidearm", IE_Pressed, combatComponent, &UCombatComponent::OnSidearm);
		inputComponent->BindAction("Reload", IE_Pressed, combatComponent, &UCombatComponent::Reload);
		inputComponent->BindAction("PrimaryAction", IE_Pressed, combatComponent, &UCombatComponent::OnPrimaryAction);
		inputComponent->BindAction("PrimaryAction", IE_Released, combatComponent, &UCombatComponent::OnPrimaryActionReleased);
	}

	combatComponent->RegisterComponent();
	return combatComponent;
}

void UCombatComponent::OnUIStateChanged(bool state)
{
	uiState = state;

	if (IsUIOpen())
	{
		performAction = false;
		useEquipment = false;
	}
}

void UCombatComponent::OutOfAmmo()
{
	Reload();
}

void UCombatComponent::ReloadComplete()
{
	if (performAction) {
		useEquipment = true;
	}
}

void UCombatComponent::WeaponEquipped(UWeapon* oldWeapon)
{
	ABaseAIController* aiController = Cast<ABaseAIController>(GetBaseCharacter()->GetController());
		
	if (oldWeapon && oldWeapon->IsProjectileWeapon())
	{
		UProjectileWeapon* pw = Cast<UProjectileWeapon>(oldWeapon);
		pw->OnOutOfAmmo.RemoveAll(this);
		pw->OnReloadComplete.RemoveAll(this);

		if (aiController) 
		{
			pw->OnReloadComplete.RemoveAll(aiController);
			pw->OnWeaponReady.RemoveAll(aiController);
		}
	}

	UWeapon* weapon = mCurrentWeapon();

	if (weapon && weapon->IsProjectileWeapon())
	{
		rangedWeapon = Cast<UProjectileWeapon>(weapon);
		rangedWeapon->OnOutOfAmmo.AddUniqueDynamic(this, &UCombatComponent::OutOfAmmo);
		rangedWeapon->OnReloadComplete.AddUniqueDynamic(this, &UCombatComponent::ReloadComplete);

		if (aiController) 
		{
			rangedWeapon->OnReloadComplete.AddUniqueDynamic(aiController, &ABaseAIController::ReloadComplete);
			rangedWeapon->OnWeaponReady.AddUniqueDynamic(aiController, &ABaseAIController::WeaponReady);
		}
	}

	if (performAction)
	{
		useEquipment = true;
	}
}

void UCombatComponent::Reload()
{
	if (rangedWeapon)
	{
		if (GetBaseCharacter()->GetTaskManager()->PerformTask(reloadTask, false))
		{
			useEquipment = false;
		}
	}
}

void UCombatComponent::OnPrimaryActionReleased()
{
	if (!IsUIOpen())
	{
		performAction = false;
		useEquipment = false;
	}
}

void UCombatComponent::OnPrimaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetInventory()->GetPrimaryWeaponSlot(), EGearType::Weapon);
}

void UCombatComponent::OnSecondaryWeapon()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetInventory()->GetSecondaryWeaponSlot(), EGearType::Weapon);
}

void UCombatComponent::OnSidearm()
{
	EquipWeaponAtSlot(GetBaseCharacter()->GetInventory()->GetSidearmWeaponSlot(), EGearType::Sidearm);
}

void UCombatComponent::OnPrimaryAction()
{
	if (!IsUIOpen())
	{
		performAction = true;
		useEquipment = true;
	}
}

void UCombatComponent::EquipWeaponAtSlot(int32 slot, EGearType type)
{
	UWeapon* equippedWeapon = GetBaseCharacter()->GetInventory()->GetEquippedWeapon();

	if (equippedWeapon == NULL || (equippedWeapon != NULL
		&& equippedWeapon->GetInstanceItemData().slot != slot))
	{
		if (!equipmentSwapTask)
		{
			equipmentSwapTask = NewObject<UEquipmentSwapTask>();
		}

		equipmentSwapTask->SetSlot(slot);
		useEquipment = false;
		GetBaseCharacter()->GetTaskManager()->PerformTask(equipmentSwapTask, false);
	}
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	reloadTask = NewObject<UReloadTask>();
	SetComponentTickEnabled(true);

	if (baseCharacter->GetEquippedWeapon())
	{
		WeaponEquipped(nullptr);
	}
}