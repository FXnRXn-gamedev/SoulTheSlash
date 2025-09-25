// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SlashCharacteAnimInstanceBase.generated.h"

UENUM(BlueprintType)
enum class EAnimCardinalDirection : uint8
{
	Forward     UMETA(DisplayName = "Forward"),
	Backward     UMETA(DisplayName = "Backward"),
	Left      UMETA(DisplayName = "Left"),
	Right      UMETA(DisplayName = "Right")
};


UCLASS()
class SOULTHESLASH_API USlashCharacteAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance", meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const {return Speed; }

	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const {return Speed != 0.f; }

	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsNotMoving() const {return Speed == 0.f; }

	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FORCEINLINE float Direction() const {return DirectionAngle; }

	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsJumping() const {return bIsJumping; }

	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsOnGround() const {return !bIsJumping; }

	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsAccelerating() const {return bIsAccelerating; }
	
	UFUNCTION(BlueprintCallable, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FORCEINLINE FVector GetWorldVelocity() const {return Velocity; }

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FRotator WorldRotation;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FVector WorldVelocity2D;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	FVector LocalVelocity2D;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	bool HasVelocity;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	EAnimCardinalDirection LocalVelocityDirection;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	bool IsCrouching;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	bool IsSprinting;

	// Jumping
	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	bool IsJumping;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	float TimeToReachJumpApex;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	float GroundDistance;

	UPROPERTY(BlueprintReadWrite, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	float LandRecoveryAlpha;

	UPROPERTY(BlueprintReadWrite, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	float TimeOfFalling;
	

	UPROPERTY()
	class ASlashPlayerCharacter* OwnerCharacter;

	UPROPERTY()
	class UCharacterMovementComponent* OwnerMovementComp;


private:
	
	float Speed;
	FVector Velocity;
	float DirectionAngle;
	FVector acceleration;
	bool bIsJumping;
	bool bIsAccelerating;

	void UpdateVelocityData();
	void UpdateCharacterMovementData();
	void UpdateJumpFallData();
	void UpdateJumpFallRecoveryDataThreadSafe(float DeltaSeconds);

	EAnimCardinalDirection SelectCardinalDirectionFromAngle(float Angle);
	float ABSAngle;
	
	
};
