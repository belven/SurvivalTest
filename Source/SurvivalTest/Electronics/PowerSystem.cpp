#include "PowerSystem.h"

float APowerSystem::GetTotalPowerProvided()
{
	float total = 0;
	for (APowerSystem* ps : GetPowerSystems())
	{
		if (ps->ProvidesPower() && ps->IsActive())
			total += ps->GetPowerProvided();
	}
	return total;
}

float APowerSystem::GetTotalPowerConsumed()
{
	float total = 0;
	for (APowerSystem* ps : GetPowerSystems())
	{
		if (ps->ConsumesPower() && ps->IsActive())
			total += ps->GetPowerConsumed();
	}
	return total;
}

void APowerSystem::DetermineActiveState()
{
	if (isAutomatic && consumesPower) {
		float powerAvailable = GetPowerProvided();
		float powerUsed = GetPowerConsumed();

		if (powerConsumed < (powerAvailable - powerUsed))
		{
			SetActive(true);
		}
		else
		{
			SetActive(false);
		}
	}
}

void APowerSystem::SystemAdded(APowerSystem* system)
{
	AddPowerSystem(system);
	DetermineActiveState();
}

void APowerSystem::SystemRemoved(APowerSystem* system)
{
	RemovePowerSystem(system);
	DetermineActiveState();
}

void APowerSystem::SystemActiveChanged(APowerSystem* system)
{
	DetermineActiveState();
}

void APowerSystem::AddPowerSystem(APowerSystem* system)
{
	if (!systems.Contains(system)) {
		GetPowerSystems().Add(system);
		system->OnPowerSystemAdded.AddUniqueDynamic(this, &APowerSystem::SystemAdded);
		system->OnPowerSystemRemoved.AddUniqueDynamic(this, &APowerSystem::SystemRemoved);
		system->OnPowerSystemActiveChanged.AddUniqueDynamic(this, &APowerSystem::SystemActiveChanged);
		OnPowerSystemAdded.Broadcast(system);
	}
}

void APowerSystem::RemovePowerSystem(APowerSystem* system)
{
	if (!systems.Contains(system)) {
		system->OnPowerSystemAdded.RemoveAll(this);
		system->OnPowerSystemRemoved.RemoveAll(this);
		system->OnPowerSystemActiveChanged.RemoveAll(this);
		GetPowerSystems().Remove(system);
		OnPowerSystemRemoved.Broadcast(system);
	}
}

void APowerSystem::SetActive(bool active)
{
	if (isActive != active)
	{
		isActive = active;
		OnPowerSystemActiveChanged.Broadcast(this);
	}
}