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
#include "KismetAnimationLibrary.h"
#include "Component/StateComponent.h"
#include "Item/SlashEquippableItemMaster.h"



#pragma region Unreal Engine Callbacks

ASlashPlayerCharacter::ASlashPlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(32.0f, 88.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Create combat component
	StatComponent = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("StatComponent"));

	// Create state component
	StateComponent = CreateDefaultSubobject<UStateComponent>(TEXT("StateComponent"));
	
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

	// Initialize equippable items
	InitializeEquippables();
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
	MoveActionValue = MovementVector;

	if (Controller != nullptr && StateComponent->MovementInput)
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
	if (MovePressedKey.GetFName() == FName("W"))
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
	if (bCrawlMode) return;

	if (!GetCharacterMovement()->IsFalling())
	{
		if (bIsCrouching)
		{
			UnCrouch();
			bIsCrouching = false;
		}
		else
		{
			GetCharacterMovement()->SetCrouchedHalfHeight(60.0f);
			Crouch();
			bIsCrouching = true;
		}
	}
	
}

void ASlashPlayerCharacter::PerformRoll()
{
	// Disable collision with enemies during roll
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	FString KeyName = MovePressedKey.GetDisplayName().ToString();
	bIsRolling = true;
	if (KeyName == "W")
	{
		if (PlayerAnimInstance && RollForwardAnimMontage)
		{
			PlayerAnimInstance->Montage_Play(RollForwardAnimMontage, 1.0f);
		}
		
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASlashPlayerCharacter::OnRollMontageCompleted);
		PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, RollForwardAnimMontage);
		
	}
	else if (KeyName == "S")
	{
		if (PlayerAnimInstance && RollBackwardAnimMontage)
		{
			PlayerAnimInstance->Montage_Play(RollBackwardAnimMontage, 1.0f);
		}
		
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASlashPlayerCharacter::OnRollMontageCompleted);
		PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, RollBackwardAnimMontage);
		
	}
	else if (KeyName == "A")
	{
		if (PlayerAnimInstance && RollLeftAnimMontage)
		{
			PlayerAnimInstance->Montage_Play(RollLeftAnimMontage, 1.0f);
		}
	
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASlashPlayerCharacter::OnRollMontageCompleted);
		PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, RollLeftAnimMontage);
	}
	else if (KeyName == "D")
	{
		if (PlayerAnimInstance && RollRightAnimMontage)
		{
			PlayerAnimInstance->Montage_Play(RollRightAnimMontage, 1.0f);
		}

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &ASlashPlayerCharacter::OnRollMontageCompleted);
		PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, RollRightAnimMontage);
		
	}
	
}

void ASlashPlayerCharacter::EndRoll()
{
	// Re-enable collision
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	
}

void ASlashPlayerCharacter::ToggleCrawlMode()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (bIsCrouching) // true
		{
			bCrawlMode = false;
		}
		else // false
		{
			if (bCrawlMode)
			{
				bCrawlMode = false;
			}
			else
			{
				GetCharacterMovement()->SetCrouchedHalfHeight(CrawlCapsuleHalfHeight);
				Crouch();
			}
			
			// if (bCrawlMode)
			// {
			// 	// Exit crawl mode
			// 	bCrawlMode = false;
			// 	UnCrouch();
			// 	bIsCrouching = false;
			// 	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
			// 	GetCharacterMovement()->bOrientRotationToMovement = true;
			// 	GetCharacterMovement()->bUseControllerDesiredRotation = false;
			// 	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
			// }
			// else
			// {
			// 	// Enter crawl mode
			// 	bCrawlMode = true;
			// 	Crouch();
			// 	bIsCrouching = true;
			// 	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
			// 	GetCharacterMovement()->bOrientRotationToMovement = false;
			// 	GetCharacterMovement()->bUseControllerDesiredRotation = true;
			// 	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
			// }
			
		}
	}

	if (!bCrawlMode)
	{
		const float DelayDuration = 0.35f;
		if (GWorld)
		{
			FTimerHandle CrawlDelayTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(CrawlDelayTimerHandle, this, &ASlashPlayerCharacter::OnCrawlDelayCompleted, DelayDuration, false);
		}
	}
}

void ASlashPlayerCharacter::OnCrawlDelayCompleted()
{
	UnCrouch();
}

#pragma endregion


#pragma region Equip/Unequip

void ASlashPlayerCharacter::InitializeEquippables()
{
	
	// Create a copy of the setup to avoid modifying while iterating
	TMap<EItemTypeEnum, FEquippableStruct> OriginalSetup = EquippableSetup;

	// Clear the original map to rebuild it with spawned items
	EquippableSetup.Empty();
	for (const auto& element : OriginalSetup)
	{
		if (!element.Value.ItemActor || !GetWorld())
		{
			// Keep the original entry if no ItemActor or no World
			EquippableSetup.Add(element.Key, element.Value);
			continue;
		}

		// Spawn Item
		FVector SpawnLocation = FVector::ZeroVector;
		FRotator SpawnRotation = FRotator::ZeroRotator;
        
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ASlashEquippableItemMaster* SpawnedItem = GetWorld()->SpawnActor<ASlashEquippableItemMaster>(
			element.Value.ItemActor, SpawnLocation, SpawnRotation, SpawnParams);

		if (SpawnedItem)
		{
			FEquippableStruct NewEquippable;
			NewEquippable.ItemActor = element.Value.ItemActor;
			NewEquippable.AttachSocket = element.Value.AttachSocket;
			NewEquippable.EquipHand = element.Value.EquipHand;
			NewEquippable.ActorRef = SpawnedItem->GetClass();  // Store the actual actor instance, not the class
			NewEquippable.Equipped = element.Value.Equipped;

			EquippableSetup.Add(element.Key, NewEquippable);

			// Attach Item if needed
			if (!NewEquippable.Equipped)
			{
				AttachItemUnequipped(NewEquippable, SpawnedItem);
			}
		}
		else
		{
			// If spawning failed, keep the original entry
			EquippableSetup.Add(element.Key, element.Value);
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn equippable item for key: %d"), (int32)element.Key);
		}

	}
}

void ASlashPlayerCharacter::AttachItemUnequipped(FEquippableStruct EquippableStruct, AActor* Actor)
{
	// This switch block is the C++ equivalent of the "Switch on Socket_Enum" node
	FName SocketToAttachTo;

	// Set up the attachment rules, matching the pins on the Blueprint node
	const FAttachmentTransformRules AttachmentRules
	(
		EAttachmentRule::SnapToTarget, // Location Rule
		EAttachmentRule::SnapToTarget, // Rotation Rule
		EAttachmentRule::KeepRelative, // Scale Rule
		true                           // bWeldSimulatedBodies
	);
	
	switch (EquippableStruct.AttachSocket)
	{
	case ESocketEnum::WeaponBowBack_Socket:
		SocketToAttachTo = FName("WeaponBowBack_Socket");
		Actor->AttachToComponent(GetMesh(), AttachmentRules, SocketToAttachTo);
		break;
	case ESocketEnum::WeaponCrossbowBack_Socket:
		SocketToAttachTo = FName("WeaponCrossbowBack_Socket");
		Actor->AttachToComponent(GetMesh(), AttachmentRules, SocketToAttachTo);
		break;
	case ESocketEnum::WeaponSword_Socket:
		SocketToAttachTo = FName("WeaponSword_Socket");
		Actor->AttachToComponent(GetMesh(), AttachmentRules, SocketToAttachTo);
		break;
	case ESocketEnum::WeaponKnife_Socket:
		SocketToAttachTo = FName("WeaponKnife_Socket");
		Actor->AttachToComponent(GetMesh(), AttachmentRules, SocketToAttachTo);
		break;
	}
}

UAnimMontage* ASlashPlayerCharacter::GetEquipMontage(FEquippableStruct EquippableStruct)
{

	switch (EquippableStruct.AttachSocket)
	{
	case ESocketEnum::WeaponBowBack_Socket:
		return EquipBowAnimMontage;
	case ESocketEnum::WeaponCrossbowBack_Socket:
		return nullptr;
	case ESocketEnum::WeaponSword_Socket:
		return EquipSwordAnimMontage;
	case ESocketEnum::WeaponKnife_Socket:
		return EquipKnifeAnimMontage;
	default:
		return nullptr;
	}
}

UAnimMontage* ASlashPlayerCharacter::GetUnequipMontage(FEquippableStruct EquippableStruct)
{

	switch (EquippableStruct.AttachSocket)
	{
	case ESocketEnum::WeaponBowBack_Socket:
		return UnequipBowAnimMontage;
	case ESocketEnum::WeaponCrossbowBack_Socket:
		return nullptr;
	case ESocketEnum::WeaponSword_Socket:
		return UnequipSwordAnimMontage;
	case ESocketEnum::WeaponKnife_Socket:
		return UnequipKnifeAnimMontage;
	default:
		return nullptr;
	}
}

void ASlashPlayerCharacter::SetEquipStatus(EItemTypeEnum ItemType, bool bEquipped)
{
	FEquippableStruct* FoundItem = EquippableSetup.Find(ItemType);
	if (FoundItem)
	{
		FoundItem->Equipped = bEquipped;
	}
	else
	{
		EquippableSetup.Add(ItemType, FEquippableStruct());
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

void ASlashPlayerCharacter::OnRollMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		// The montage was interrupted by another montage
		//UKismetSystemLibrary::PrintString(this, TEXT("Montage Interrupted!"));
		if (Montage && PlayerAnimInstance->Montage_IsPlaying(Montage))
		{
			PlayerAnimInstance->Montage_Stop(0.1f, Montage);
		}
	}
	else
	{
		// The montage completed successfully
		//UKismetSystemLibrary::PrintString(this, TEXT("Montage Completed."));
		bIsRolling = false;
		// FRotator ActorRotation = GetActorRotation();
		// FRotator BaseAimRotation = GetBaseAimRotation();
		//
		// FRotator NewRotation( BaseAimRotation.Pitch, BaseAimRotation.Yaw, ActorRotation.Roll );
		//
		// SetActorRotation(NewRotation);

		//SetCharacterState(ECharacterState::Idle);
	}

	EndRoll();
	
}

void ASlashPlayerCharacter::GetActionKeyName(const UInputAction* InputAction)
{
	if (!InputAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputAction is null"));
		return;
	}
	ASlashPlayerController* PC = Cast<ASlashPlayerController>(GetController());
	
	if (ULocalPlayer* LocalPlayer = GetWorld()->GetFirstPlayerController()->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			TArray<FKey> MappedKeys;
			MappedKeys = Subsystem->QueryKeysMappedToAction(InputAction);

			// Log all keys mapped to this action
			for (const FKey& Key : MappedKeys)
			{
				if (PC->IsInputKeyDown(Key))
				{
					MovePressedKey = Key;
					//UE_LOG(LogTemp, Warning, TEXT("%s"), *Key.GetDisplayName().ToString());
				}
				

			}
		}
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
