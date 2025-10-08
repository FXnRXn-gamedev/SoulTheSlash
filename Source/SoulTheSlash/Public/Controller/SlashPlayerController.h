// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SlashPlayerController.generated.h"


class UInputAction;
class UInputMappingContext;
class ASlashPlayerCharacter;



UCLASS()
class SOULTHESLASH_API ASlashPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASlashPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

#pragma region Enhanced Input component
	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* CrawlAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* ParryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Input")
	UInputAction* DashAction;

#pragma endregion
	
private:
	// Input handlers
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void StartJumpInput();
	void StopJumpInput();
	void SprintInput();
	void SprintCompletedInput();
	void CrouchInput();
	void RollStartedInput();
	void CrawlInput();
	void CrawlStarted();
	void CrawlStopped();

	// Reference to controlled character
	UPROPERTY()
	ASlashPlayerCharacter* SlashCharacter;
	
	
};
