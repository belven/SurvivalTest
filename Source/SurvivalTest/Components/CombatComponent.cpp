#include "CombatComponent.h"

#include "SurvivalTest/BaseAIController.h"
#include "SurvivalTest/HelperFunctions.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Items/ProjectileWeapon.h"
#include "SurvivalTest/Tasks/EquipmentSwapTask.h"
#include "SurvivalTest/Tasks/ReloadTask.h"
#include "SurvivalTest/Tasks/TaskManagerComponent.h"

DEFINE_LOG_CATEGORY(CombatComponentLog)

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

UCombatComponent* UCombatComponent::CreateCombatComponent(ABaseAIController* controller, ABaseCharacter* character)
{
	UCombatComponent* combatComponent = CreateCombatComponent_INTERNAL(controller, character);
	controller->OnUseTool.AddUniqueDynamic(combatComponent, &UCombatComponent::OnPrimaryAction);
	controller->OnStopUsingTool.AddUniqueDynamic(combatComponent, &UCombatComponent::OnPrimaryActionReleased);
	controller->OnReload.AddUniqueDynamic(combatComponent, &UCombatComponent::Reload);
	character->OnWeaponEquipped.AddUniqueDynamic(controller, &ABaseAIController::WeaponEquipped);
	return combatComponent;
}

UCombatComponent* UCombatComponent::CreateCombatComponent(ABasePlayerController* controller, ABaseCharacter* character)
{
	UCombatComponent* combatComponent = CreateCombatComponent_INTERNAL(controller, character);
	TObjectPtr<UInputComponent> inputComponent = combatComponent->GetOwner()->InputComponent;
	controller->OnUIChangedState.AddUniqueDynamic(combatComponent, &UCombatComponent::OnUIStateChanged);

	inputComponent->BindAction("PrimaryWeapon", IE_Pressed, combatComponent, &UCombatComponent::OnPrimaryWeapon);
	inputComponent->BindAction("SecondaryWeapon", IE_Pressed, combatComponent, &UCombatComponent::OnSecondaryWeapon);
	inputComponent->BindAction("Sidearm", IE_Pressed, combatComponent, &UCombatComponent::OnSidearm);
	inputComponent->BindAction("Reload", IE_Pressed, combatComponent, &UCombatComponent::Reload);
	inputComponent->BindAction("PrimaryAction", IE_Pressed, combatComponent, &UCombatComponent::OnPrimaryAction);
	inputComponent->BindAction("PrimaryAction", IE_Released, combatComponent, &UCombatComponent::OnPrimaryActionReleased);
	return combatComponent;
}

UCombatComponent* UCombatComponent::CreateCombatComponent_INTERNAL(AController* controller, ABaseCharacter* character)
{
	UCombatComponent* combatComponent = NewObject<UCombatComponent>(controller);
	combatComponent->SetBaseCharacter(character);
	character->OnWeaponEquipped.AddUniqueDynamic(combatComponent, &UCombatComponent::WeaponEquipped);
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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("UCombatComponent OnOutOfAmmo"));
}

void UCombatComponent::WeaponReady()
{
	if (performAction) {
		useEquipment = true;
	}
}

void UCombatComponent::WeaponEquipped(UWeapon* oldWeapon)
{
	UWeapon* weapon = mCurrentWeapon();

	if (oldWeapon)
	{
		oldWeapon->OnWeaponReady.RemoveAll(this);

		if (oldWeapon->IsProjectileWeapon())
		{
			UProjectileWeapon* pw = Cast<UProjectileWeapon>(oldWeapon);
			pw->OnOutOfAmmo.RemoveAll(this);
		}
	}

	if (weapon)
	{
		if (weapon->IsProjectileWeapon())
		{
			rangedWeapon = Cast<UProjectileWeapon>(weapon);
			rangedWeapon->OnOutOfAmmo.AddUniqueDynamic(this, &UCombatComponent::OutOfAmmo);
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
		useEquipment = false;

		if (GetBaseCharacter()->GetTaskManager()->PerformTask(reloadTask, false))
		{
			UE_LOG(CombatComponentLog, Log, TEXT("Character %s Reloading"), *baseCharacter->GetCharacterName());
		}
		else
		{
			UE_LOG(CombatComponentLog, Warning, TEXT("Character %s Failed Reloading"), *baseCharacter->GetCharacterName());
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