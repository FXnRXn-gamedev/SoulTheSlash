// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatComponent.generated.h"

// Dynamic multicast delegates allow for Blueprint and C++ communication
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, NewStamina, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDied);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULTHESLASH_API UPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerStatComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	//Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Attributes", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Attributes", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Attributes", meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Attributes", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Attributes", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRate = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Attributes", meta = (AllowPrivateAccess = "true"))
	float StaminaDegenRate = 0.1f;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "---Slash---|Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "---Slash---|Events")
	FOnStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "---Slash---|Events")
	FOnDied OnDied;
	
	
private:
	// Timers
	FTimerHandle StaminaRegenTimerHandle;
	FTimerHandle DelayTimerHandle;

	// Gate for stamina regeneration control
	bool bStaminaRegenGate = false;


public:
	// Public functions for interacting with attributes
	UFUNCTION(BlueprintCallable, Category = "---Slash---|Attributes")
	bool CheckPlayerHasEnoughStamina(float StaminaCost);
	
	UFUNCTION(BlueprintCallable, Category = "---Slash---|Attributes")
	void DecreaseStamina(float StaminaCost);

	UFUNCTION(BlueprintCallable, Category = "---Slash---|Attributes")
	void RegenerateStamina();

	UFUNCTION(BlueprintCallable, Category = "---Slash---|Attributes")
	void RefreshStamina(bool StartStaminaRegen);

	UFUNCTION(BlueprintCallable, Category = "---Slash---|Attributes")
	void HandleStaminaRegeneration(bool bStartRefresh, float Duration);
	

	
		
};
