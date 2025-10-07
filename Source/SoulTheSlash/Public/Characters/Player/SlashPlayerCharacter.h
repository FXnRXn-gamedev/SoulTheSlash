// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Characters/SlashCharacterBase.h"
#include "InputActionValue.h"
#include "Controller/SlashPlayerController.h"
#include "SlashPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USlashPlayerStatWidget;
class USlashPlayerHUDWidget;
class USlashCharacteAnimInstanceBase;
class UAnimMontage;
class ASlashEquippableItemMaster;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Moving,
	Jumping,
	Crouching,
	Rolling,
	Dodging,
	Attacking,
	Blocking,
	Hit,
	Dead
};

UENUM(BlueprintType)
enum class EItemTypeEnum : uint8
{
	Primary			UMETA(DisplayName = "Primary"),
	Secondary		UMETA(DisplayName = "Secondary"),
	Placeholder1	UMETA(DisplayName = "Placeholder1")
	
};

UENUM(BlueprintType)
enum class ESocketEnum : uint8
{
	WeaponBowBack_Socket		UMETA(DisplayName = "WeaponBowBack_Socket"),
	WeaponCrossbowBack_Socket	UMETA(DisplayName = "WeaponCrossbowBack_Socket"),
	WeaponSword_Socket			UMETA(DisplayName = "WeaponSword_Socket"),
	WeaponKnife_Socket			UMETA(DisplayName = "WeaponKnife_Socket")
};

UENUM(BlueprintType)
enum class EEquipHandEnum : uint8
{
	HoldItem_Sword_r			UMETA(DisplayName = "HoldItem_Sword_r"),
	HoldItem_Bow_l				UMETA(DisplayName = "HoldItem_Bow_l")
};

USTRUCT(BlueprintType)
struct FEquippableStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Setting")
	TSubclassOf<ASlashEquippableItemMaster> ItemActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Setting")
	ESocketEnum AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Setting")
	EEquipHandEnum EquipHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Setting")
	TSubclassOf<AActor> ActorRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Setting")
	bool Equipped;
};


UCLASS()
class SOULTHESLASH_API ASlashPlayerCharacter : public ASlashCharacterBase
{
	GENERATED_BODY()

public:
	ASlashPlayerCharacter();

	// Public Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData")
	TMap<EItemTypeEnum, FEquippableStruct> EquippableSetup;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Public Setting")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Public Setting")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Public Setting")
	bool bIsJumping = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Public Setting")
	bool bIsRolling = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "---Slash---|Camera Shake")
	TSubclassOf<UCameraShakeBase> SprintCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "---Slash---|Camera Shake")
	TSubclassOf<UCameraShakeBase> FallLandCameraShakeClass;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

#pragma region Setting

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	bool bLeftHandClosed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	bool bRightHandClosed = false;
	
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

	UPROPERTY()
	FKey MovePressedKey;

#pragma endregion 

#pragma region State Management

	UPROPERTY(BlueprintReadOnly, Category = "---Slash---|State")
	ECharacterState CurrentState;
	
#pragma endregion

#pragma region Component/Refference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Component")
	class UPlayerStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|Component")
	class UStateComponent* StateComponent;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Roll")
	UAnimMontage* RollForwardAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Roll")
	UAnimMontage* RollBackwardAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Roll")
	UAnimMontage* RollLeftAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Roll")
	UAnimMontage* RollRightAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipSwordAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipBowAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipKnifeAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Unequip")
	TObjectPtr<UAnimMontage> UnequipSwordAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Unequip")
	TObjectPtr<UAnimMontage> UnequipBowAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Animation|Unequip")
	TObjectPtr<UAnimMontage> UnequipKnifeAnimMontage;
	
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
	void PerformRoll();

protected:
	// Equip/Unequip
	void InitializeEquippables();
	void AttachItemUnequipped(FEquippableStruct EquippableStruct, AActor* Actor);
	UAnimMontage* GetEquipMontage(FEquippableStruct EquippableStruct);
	UAnimMontage* GetUnequipMontage(FEquippableStruct EquippableStruct);
	void SetEquipStatus(EItemTypeEnum ItemType, bool bEquipped);
	
	// Create and setup widget
	void SetupPlayerWidget();

	// Setup Animation
	void SetupPlayerAnimation();
	
	//
	bool IsPlayerMoving();

	// State Management
	void SetCharacterState(ECharacterState NewState);

	void FindOutGroundDistance();

	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	

public:
	void GetActionKeyName(const UInputAction* InputAction);

	// Anim Notify
	void HandleNotify_CameraShake();
	void HandleNotify_FallLand();
	
};
