// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Characters/SlashCharacterBase.h"
#include "InputActionValue.h"
#include "SlashPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USlashPlayerStatWidget;
class USlashPlayerHUDWidget;
class USlashCharacteAnimInstanceBase;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Moving,
	Jumping,
	Crouching,
	Dodging,
	Attacking,
	Blocking,
	Hit,
	Dead
};


UCLASS()
class SOULTHESLASH_API ASlashPlayerCharacter : public ASlashCharacterBase
{
	GENERATED_BODY()

public:
	ASlashPlayerCharacter();

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	bool bIsJumping = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "---Slash---|Camera Shake")
	TSubclassOf<UCameraShakeBase> SprintCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "---Slash---|Camera Shake")
	TSubclassOf<UCameraShakeBase> FallLandCameraShakeClass;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

#pragma region Setting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	bool bIsDebug = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	float NormalSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	float SprintSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	float CrouchSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	float JumpingMoveSpeed = 300.0f;

#pragma endregion 

#pragma region State Management

	UPROPERTY(BlueprintReadOnly, Category = "---Slash---|State")
	ECharacterState CurrentState;
	
#pragma endregion

#pragma region Component/Refference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Stat")
	class UPlayerStatComponent* StatComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "---Slash---|UI")
	TSubclassOf<USlashPlayerStatWidget> PlayerStatWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "---Slash---|UI")
	TSubclassOf<USlashPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
	USlashPlayerStatWidget* PlayerStatWidget;

	UPROPERTY()
	USlashPlayerHUDWidget* PlayerHUDWidget;

#pragma endregion 

#pragma region Camera component
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Camera")
	UCameraComponent* FollowCamera;

#pragma endregion

#pragma region Animation

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Animation")
	USlashCharacteAnimInstanceBase* PlayerAnimInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation")
	TSubclassOf<UAnimInstance> UnarmedAnimLayerClass;

	
#pragma endregion 

	

public:
	// Movement
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void Sprint();
	void SprintCompleted();
	void PlayerCrouch();

protected:
	// Create and setup widget
	void SetupPlayerWidget();

	// Setup Animation
	void SetupPlayerAnimation();
	
	//
	bool IsPlayerMoving();

	// State Management
	void SetCharacterState(ECharacterState NewState);

	void FindOutGroundDistance();

public:
	// Anim Notify
	void HandleNotify_CameraShake();
	void HandleNotify_FallLand();
	
};
