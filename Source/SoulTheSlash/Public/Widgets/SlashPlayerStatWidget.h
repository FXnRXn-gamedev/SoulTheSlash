// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashPlayerStatWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UPlayerStatComponent;


UCLASS()
class SOULTHESLASH_API USlashPlayerStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Health Stamina Widget")
	void SetPlayerStatComponent(UPlayerStatComponent* Component);

protected:
	// Widget Components
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerLevelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNameText;

	// Update Functions
	UFUNCTION()
	void UpdateHealth(float Health, float MaxHealth);

	UFUNCTION()
	void UpdateStamina(float Stamina, float MaxStamina);


private:
	UPROPERTY()
	UPlayerStatComponent* PlayerStatComponent;


	float PreviousHealthPercentage;
	float PreviousStaminaPercentage;
};
