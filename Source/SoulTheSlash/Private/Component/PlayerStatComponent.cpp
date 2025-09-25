// FXnRXn copyright notice


#include "Component/PlayerStatComponent.h"

#include "Macros/SlashMacrosLibrary.h"


UPlayerStatComponent::UPlayerStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
}



void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	
}

bool UPlayerStatComponent::CheckPlayerHasEnoughStamina(float StaminaCost)
{
	return CurrentStamina >= StaminaCost;
}

void UPlayerStatComponent::DecreaseStamina(float StaminaCost)
{
	//CurrentStamina -= StaminaCost;
	CurrentStamina = FMath::Clamp(CurrentStamina - StaminaCost, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UPlayerStatComponent::RegenerateStamina()
{
	//CurrentStamina += StaminaRegenRate;
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	if (CurrentStamina >= MaxStamina)
	{
		RefreshStamina(false);
	}
}

void UPlayerStatComponent::RefreshStamina(bool StartStaminaRegen)
{
	if (StartStaminaRegen)
	{
		GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &UPlayerStatComponent::RegenerateStamina, 1.0f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	}
}

void UPlayerStatComponent::HandleStaminaRegeneration(bool bStartRefresh, float Duration)
{
	if (bStartRefresh)
	{
		USlashMacrosLibrary::Gate(true, bStaminaRegenGate);
		if (USlashMacrosLibrary::PassThroughGate(bStaminaRegenGate))
		{
			GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, [this]() { RefreshStamina(true); }, Duration, false);
			//RefreshStamina(true);
		}
	}
	else
	{
		USlashMacrosLibrary::Gate(false, bStaminaRegenGate);
		RefreshStamina(false);

	}
}




