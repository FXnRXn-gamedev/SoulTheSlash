// FXnRXn copyright notice


#include "AnimInstance/SlashCharacteAnimInstanceBase.h"
#include "KismetAnimationLibrary.h"
#include "Characters/Player/SlashPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void USlashCharacteAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ASlashPlayerCharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	bIsJumping = false;
	Speed = 0.0f;
	Velocity = FVector::ZeroVector;
	DirectionAngle = 0.0f;
}

void USlashCharacteAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	

	//UE_LOG(LogTemp, Log, TEXT(": %f"), DirectionAngle);
}

void USlashCharacteAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	

	if (OwnerCharacter)
	{
		// Moving Direction 
		Velocity = OwnerCharacter->GetVelocity();
		DirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());

		// Player Current Speed
		Speed = FVector(Velocity.X, Velocity.Y, 0.f).Size();

		//
		
		
	}

	if (OwnerMovementComp)
	{
		bIsJumping = OwnerMovementComp->IsFalling();
		bIsAccelerating = OwnerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;
		acceleration = OwnerMovementComp->GetCurrentAcceleration();
	}

	UpdateCharacterMovementData();
	UpdateJumpFallData();
	UpdateVelocityData();
	UpdateJumpFallRecoveryDataThreadSafe(DeltaSeconds);
	
}

void USlashCharacteAnimInstanceBase::UpdateVelocityData()
{
	if (OwnerCharacter)
	{
		WorldRotation = OwnerCharacter->GetActorRotation();
		WorldVelocity2D = FVector(Velocity.X, Velocity.Y, 0.f);
		LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);

		float VelocitySquared = LocalVelocity2D.SizeSquared();
		bool bIsNearlyZero = FMath::IsNearlyEqual(VelocitySquared, 0.0f, 0.000001f);
		HasVelocity = !bIsNearlyZero;
		if (HasVelocity)
		{
			LocalVelocityDirection = SelectCardinalDirectionFromAngle(DirectionAngle);//FMath::Atan2(LocalVelocity2D.Y, LocalVelocity2D.X) * (180.0f / PI)
		}
		else
		{
			LocalVelocityDirection = EAnimCardinalDirection::Forward;
		}
	}
}

void USlashCharacteAnimInstanceBase::UpdateCharacterMovementData()
{
	if (OwnerCharacter == nullptr || OwnerMovementComp == nullptr) return;
	
	IsSprinting = OwnerCharacter->bIsSprinting;
	IsCrouching = OwnerCharacter->bIsCrouching;
	IsJumping = false;
	IsFalling = false;
	if (OwnerMovementComp)
	{
		if (Velocity.Z > 0.0f)
		{
			IsJumping = true;
			//TimeToReachJumpApex = OwnerMovementComp->GetJumpApexTime();
		}
		else
		{
			IsFalling = true;
		}
		//GroundDistance = OwnerMovementComp->CurrentFloor.FloorDist;
	}
	
}

void USlashCharacteAnimInstanceBase::UpdateJumpFallData()
{
	if (IsJumping)
	{
		TimeToReachJumpApex = (0.0f - OwnerMovementComp->Velocity.Z) / OwnerMovementComp->GetGravityZ();
	}
	else
	{
		TimeToReachJumpApex = 0.0f;
	}
}

void USlashCharacteAnimInstanceBase::UpdateJumpFallRecoveryDataThreadSafe(float DeltaTime)
{
	if (IsFalling)
	{
		TimeOfFalling += DeltaTime;
	}
	else
	{
		if (IsJumping)
		{
			TimeOfFalling = 0.0f;
		}
	}
}

EAnimCardinalDirection USlashCharacteAnimInstanceBase::SelectCardinalDirectionFromAngle(float Angle)
{
	ABSAngle = FMath::Abs(Angle);

	if (ABSAngle <= 65.0f)
	{
		return EAnimCardinalDirection::Forward;
	}
	else if (ABSAngle >= 115.0f)
	{
		return EAnimCardinalDirection::Backward;
	}
	else if (Angle > 0.0f)
	{
		return EAnimCardinalDirection::Right;
	}
	else
	{
		return EAnimCardinalDirection::Left;
	}
	
}


