// FXnRXn copyright notice


#include "Characters/Player/SlashPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AnimInstance/SlashCharacteAnimInstanceBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/PlayerStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/SlashPlayerHUDWidget.h"
#include "Widgets/SlashPlayerStatWidget.h"
#include "DrawDebugHelpers.h"

#pragma region Unreal Engine Callbacks

ASlashPlayerCharacter::ASlashPlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(32.0f, 88.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Create combat component
	StatComponent = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("StatComponent"));
	
	// Create camera boom
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(), TEXT("spine_03")); //RootComponent
	SpringArm->SocketOffset = FVector(0.0f, 0.0f, 65.0f);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 50.0f;

	// Create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CurrentState = ECharacterState::Idle;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;					
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 800.0f;
	GetCharacterMovement()->MaxAcceleration = 1250.0f;
	GetCharacterMovement()->GravityScale = 1.25f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void ASlashPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// Create and setup widget
	SetupPlayerWidget();
	SetupPlayerAnimation();

	// Cache anim instance
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		PlayerAnimInstance = Cast<USlashCharacteAnimInstanceBase>(GetMesh()->GetAnimInstance());
		
	}
}

void ASlashPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, FString::Printf(TEXT("Stamina: %f"), StatComponent->CurrentStamina));
	// }
	if (IsPlayerMoving())
	{
		if (CurrentState != ECharacterState::Jumping)
		{
			SetCharacterState(ECharacterState::Moving);
		}
	}
	else
	{
		if (CurrentState != ECharacterState::Jumping)
		{
			SetCharacterState(ECharacterState::Idle);
			// When stopping movement, enable movement-based rotation and disable camera-facing rotation
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		}
	}
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	}
	else if (bIsJumping)
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}


	FindOutGroundDistance();
	
}

#pragma endregion 

#pragma region Locomotion

void ASlashPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		if (CurrentState == ECharacterState::Idle)
		{
			SetCharacterState(ECharacterState::Moving);

			// When starting to move, disable movement-based rotation and enable camera-facing rotation
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

			//---> Edited
			// GetCharacterMovement()->bOrientRotationToMovement = true;
			// GetCharacterMovement()->bUseControllerDesiredRotation = false;
			// GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

		}
	}
}

void ASlashPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void ASlashPlayerCharacter::StartJump()
{
	SetCharacterState(ECharacterState::Jumping);
	Jump();
}

void ASlashPlayerCharacter::StopJump()
{
	SetCharacterState(ECharacterState::Idle);
	StopJumping();
}

void ASlashPlayerCharacter::Sprint()
{
	if (StatComponent)
	{
		if (StatComponent->CheckPlayerHasEnoughStamina(5.0f) && IsPlayerMoving())
		{
			//GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			StatComponent->DecreaseStamina(StatComponent->StaminaDegenRate);
			bIsSprinting = true;
		}
		else
		{
			bIsSprinting = false;
			//GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		}
	}
	
}

void ASlashPlayerCharacter::SprintCompleted()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	bIsSprinting = false;
	// Start stamina regeneration after sprinting
	if (StatComponent)
		StatComponent->HandleStaminaRegeneration(true, 2.0f);
}

void ASlashPlayerCharacter::PlayerCrouch()
{
	if (bIsCrouching)
	{
		UnCrouch();
		bIsCrouching = false;
		//GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
	else
	{
		Crouch();
		bIsCrouching = true;
		//GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	}
}

#pragma endregion

#pragma region UI

void ASlashPlayerCharacter::SetupPlayerWidget()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	// Create HUD Widget first
	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<USlashPlayerHUDWidget>(PlayerController, PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	// Create Stat Widget and add it to HUD
	if (PlayerStatWidgetClass && PlayerHUDWidget)
	{
		PlayerStatWidget = CreateWidget<USlashPlayerStatWidget>(PlayerController, PlayerStatWidgetClass);
		if (PlayerStatWidget)
		{
			PlayerStatWidget->SetPlayerStatComponent(StatComponent);
			PlayerHUDWidget->SetPlayerStatWidget(PlayerStatWidget);
		}
	}
}

#pragma endregion 

#pragma region Animation

void ASlashPlayerCharacter::SetupPlayerAnimation()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UnarmedAnimLayerClass)
		{
			MeshComp->LinkAnimClassLayers(UnarmedAnimLayerClass);
		}
	}
}

#pragma endregion


bool ASlashPlayerCharacter::IsPlayerMoving()
{
	FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
	if (LastInputVector.Size() > 0.001f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ASlashPlayerCharacter::SetCharacterState(ECharacterState NewState)
{
	CurrentState = NewState;
}

void ASlashPlayerCharacter::FindOutGroundDistance()
{
	// Get the world instance
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation() + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()); // Or any desired start point
	FVector EndLocation = GetActorLocation() - FVector(0.0f, 0.0f, 1000.0f); // Example: 1000 units downward
	float SphereRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	ECollisionChannel TraceChannel = ECC_Visibility; // Or your custom channel

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore the actor performing the trace
	QueryParams.bTraceComplex = true; // Trace against complex collision if needed

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity, // For sphere traces, rotation is usually identity
		TraceChannel,
		FCollisionShape::MakeSphere(SphereRadius),
		QueryParams
	);

	if (bHit)
	{
		if (PlayerAnimInstance)
		{
			PlayerAnimInstance->GroundDistance = HitResult.Distance;
		}
	}

	if (bIsDebug)
	{
		// Optional: Draw debug sphere
		DrawDebugSphere(
			GetWorld(),
			StartLocation,
			SphereRadius,
			12, // Number of segments
			FColor::Red,
			false, // Persistent
			5.f, // Duration
			0, // Depth priority
			1.f // Thickness
		);
		DrawDebugLine(
			GetWorld(),
			StartLocation,
			EndLocation,
			FColor::Green,
			false,
			5.f,
			0,
			1.f
		);
	}

	
}

#pragma region Notify Handlers

void ASlashPlayerCharacter::HandleNotify_CameraShake()
{
	if (SprintCameraShakeClass)
	{
		FVector ShakeLocation = GetActorLocation(); // Or any desired location
		float InnerRadius = 0.0f;
		float OuterRadius = 1000.0f;
		float Falloff = 1.0f;

		UGameplayStatics::PlayWorldCameraShake(this, SprintCameraShakeClass, ShakeLocation, InnerRadius, OuterRadius, Falloff);
	}
}

void ASlashPlayerCharacter::HandleNotify_FallLand()
{
	if (FallLandCameraShakeClass)
	{
		FVector ShakeLocation = GetActorLocation(); // Or any desired location
		float InnerRadius = 0.0f;
		float OuterRadius = 1000.0f;
		float Falloff = 1.0f;

		UGameplayStatics::PlayWorldCameraShake(this, FallLandCameraShakeClass, ShakeLocation, InnerRadius, OuterRadius, Falloff);
	}
}

#pragma endregion 
