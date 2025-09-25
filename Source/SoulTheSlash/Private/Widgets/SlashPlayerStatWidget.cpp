// FXnRXn copyright notice


#include "Widgets/SlashPlayerStatWidget.h"

#include "Component/PlayerStatComponent.h"
#include "Components/ProgressBar.h"

void USlashPlayerStatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	PreviousHealthPercentage = 1.0f;
	PreviousStaminaPercentage = 1.0f;
}

void USlashPlayerStatWidget::SetPlayerStatComponent(UPlayerStatComponent* Component)
{
	if (!Component) return;

	PlayerStatComponent = Component;

	// Bind to events
	PlayerStatComponent->OnHealthChanged.AddDynamic(this, &USlashPlayerStatWidget::UpdateHealth);
	PlayerStatComponent->OnStaminaChanged.AddDynamic(this, &USlashPlayerStatWidget::UpdateStamina);

	
	// Initial update
	UpdateHealth(PlayerStatComponent->CurrentHealth, PlayerStatComponent->MaxHealth);
	UpdateStamina(PlayerStatComponent->CurrentStamina, PlayerStatComponent->MaxStamina);
	
}

void USlashPlayerStatWidget::UpdateHealth(float Health, float MaxHealth)
{
	if (!HealthProgressBar) return;

	float HealthPercentage = MaxHealth > 0.0f ? Health / MaxHealth : 0.0f;
	HealthProgressBar->SetPercent(HealthPercentage);
}

void USlashPlayerStatWidget::UpdateStamina(float Stamina, float MaxStamina)
{
	if (!StaminaProgressBar ) return;

	float StaminaPercentage = MaxStamina > 0.0f ? Stamina / MaxStamina : 0.0f;
	StaminaProgressBar->SetPercent(StaminaPercentage);
}
