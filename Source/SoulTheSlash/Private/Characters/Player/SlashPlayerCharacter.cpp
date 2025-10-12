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
#include "Macros/SlashMacrosLibrary.h"


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

	CheckIfCrawlMode();
	
	
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

	if (!GetCharacterMovement()->IsFalling())
	{
		if (bCrawlMode)
		{
			GetCharacterMovement()->RotationRate = FRotator(0.0f, 85.0f, 0.0f);
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
	else if (bCrawlMode)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrawlSpeed;
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

#pragma endregion


#pragma region Roll

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

#pragma endregion

#pragma region Crawl

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
				const float DelayDuration = 0.35f;
				if (GWorld)
				{
					FTimerHandle CrawlDelayTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(CrawlDelayTimerHandle, this, &ASlashPlayerCharacter::OnCrawlDelayCompleted, DelayDuration, false);
				}

				if (CrawlToStandtAnimMontage && PlayerAnimInstance)
				{
					StateComponent->MovementInput = false;
					PlayerAnimInstance->Montage_Play(CrawlToStandtAnimMontage, 1.0f);
					FOnMontageEnded MontageEndedDelegate;
					MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
					{
						if (!bInterrupted)
						{
							// Montage completed successfully
							// Your completion logic here
							StateComponent->MovementInput = true;
						}
					});
					PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, CrawlToStandtAnimMontage);

				}
			}
			else
			{
				GetCharacterMovement()->SetCrouchedHalfHeight(CrawlCapsuleHalfHeight);
				Crouch();
				const float DelayDuration = 0.24f;
				if (GWorld)
				{
					FTimerHandle CrawlDelayTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(CrawlDelayTimerHandle,
						[this]()
					{
							bCrawlMode = true;

							//--> Disable strafe and aiming
					},
					DelayDuration,
					false);
				}
			}
		}
	}
	
}

void ASlashPlayerCharacter::CheckIfCrawlMode()
{
	if (bCrawlMode)
	{
		//Calculate Direction
		const FVector Velocity = GetVelocity();
		const FRotator BaseRotation = GetActorRotation();
		DirectionCALC = UKismetAnimationLibrary::CalculateDirection(Velocity, BaseRotation);

		if (MoveActionValue.Size() == 0.0f)
		{
			
		}
		else
		{
			bool bDirectCalc1 = USlashMacrosLibrary::IsFloatInRange(DirectionCALC, 75.0f, 180.0f, true, true);
			bool bDirectCalc2 = USlashMacrosLibrary::IsFloatInRange(DirectionCALC, -180.0f, -75.0f, true, true);

			if (bDirectCalc1 || bDirectCalc2)
			{
				CrawlRotateRight = USlashMacrosLibrary::IsFloatInRange(DirectionCALC, 0.0f, 180.0f, true, true);
			}
		}
		
	}
}

void ASlashPlayerCharacter::ResetDoOnce()
{
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;

	if (bHasExecutedOnce)
	{
		// Add your specific logic here
		GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
		PlayCrawlMontage();
		// Set the boolean to false to prevent further execution
		bHasExecutedOnce = false; 
	}
}

void ASlashPlayerCharacter::PlayCrawlMontage()
{
	UAnimMontage* CrawlRotateMontage = CrawlRotateRight? CrawlRotateRightAnimMontage : CrawlRotateLeftAnimMontage;

	if (CrawlRotateMontage && PlayerAnimInstance)
	{
		PlayerAnimInstance->Montage_Play(CrawlRotateMontage, 1.0f);

		// Bind delegates before playing the montage
		FOnMontageBlendingOutStarted BlendingOutDelegate;
		BlendingOutDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
		{
			if (!bInterrupted)
			{
				// Montage completed successfully
				// Your completion logic here
				const float DelayDuration = 0.4f;
				if (GWorld)
				{
					FTimerHandle CrawlDelayTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(CrawlDelayTimerHandle,
						[this]()
					{
							ResetDoOnce();
					},
					DelayDuration,
					false);
				}
				
			}
			else
			{
				
			}
		});
		PlayerAnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, CrawlRotateMontage);
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
			NewEquippable.ActorRef = SpawnedItem;  // Store the actual actor instance, not the class
			NewEquippable.Equipped = element.Value.Equipped;

			EquippableSetup.Add(element.Key, NewEquippable);

			// Attach Item if needed
			if (!NewEquippable.Equipped)
			{
				AttachItemSocket(NewEquippable, SpawnedItem);
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

void ASlashPlayerCharacter::AttachItemSocket(FEquippableStruct EquippableStruct, AActor* Actor)
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





void ASlashPlayerCharacter::EquipItem(EItemTypeEnum ItemType)
{
	FEquippableStruct* FoundItem = EquippableSetup.Find(ItemType);

	// This switch block is the C++ equivalent of the "Switch on Socket_Enum" node
	FName HandToAttachTo;

	// Set up the attachment rules, matching the pins on the Blueprint node
	const FAttachmentTransformRules AttachmentRules
	(
		EAttachmentRule::SnapToTarget, // Location Rule
		EAttachmentRule::SnapToTarget, // Rotation Rule
		EAttachmentRule::KeepRelative, // Scale Rule
		true                           // bWeldSimulatedBodies
	);
	
	if (FoundItem)
	{
		switch (FoundItem->AttachSocket)
		{
		case ESocketEnum::WeaponBowBack_Socket:
			HandToAttachTo = FName("HoldItem_Bow_l");
			break;
		case ESocketEnum::WeaponCrossbowBack_Socket:
			break;
		case ESocketEnum::WeaponSword_Socket:
			HandToAttachTo = FName("HoldItem_Sword_r");
			break;
		case ESocketEnum::WeaponKnife_Socket:
			HandToAttachTo = FName("HoldItem_Sword_r");
			break;
		}

		FoundItem->ActorRef->AttachToComponent(GetMesh(), AttachmentRules, HandToAttachTo);

		SetEquipStatus(ItemType, true);
	}
}

void ASlashPlayerCharacter::UnequipItem(EItemTypeEnum ItemType)
{
	FEquippableStruct* FoundItem = EquippableSetup.Find(ItemType);
	if (FoundItem)
	{
		AttachItemSocket(*FoundItem, FoundItem->ActorRef);
		SetEquipStatus(ItemType, false);
	}
}

void ASlashPlayerCharacter::EquipUnequipItem(EItemTypeEnum ItemType)
{
	// Set the current equipping item type
	CurrentEquippingItemType = ItemType;

	
	
	FEquippableStruct* PrimaryItem = EquippableSetup.Find(EItemTypeEnum::Primary);
	FEquippableStruct* SecondaryItem = EquippableSetup.Find(EItemTypeEnum::Secondary);
	FEquippableStruct* Placeholder1Item = EquippableSetup.Find(EItemTypeEnum::Placeholder1);
	bIsPrimaryItemEquipped = (PrimaryItem->EquipHand == SecondaryItem->EquipHand && PrimaryItem->EquipHand == Placeholder1Item->EquipHand) && PrimaryItem->Equipped ;
	bIsSecondaryItemEquipped = (SecondaryItem->EquipHand == PrimaryItem->EquipHand && SecondaryItem->EquipHand == Placeholder1Item->EquipHand) && SecondaryItem->Equipped ;
	bIsPlaceholder1ItemEquipped = (Placeholder1Item->EquipHand == PrimaryItem->EquipHand && Placeholder1Item->EquipHand == SecondaryItem->EquipHand) && Placeholder1Item->Equipped ;

	
	// Prevent equipping/unequipping if an animation montage is currently playing
	if (!PlayerAnimInstance->Montage_IsPlaying(PlayerAnimInstance->GetCurrentActiveMontage()))
	{
		FEquippableStruct* FoundItem = EquippableSetup.Find(ItemType);
		if (FoundItem->ActorRef)
		{
			switch (ItemType)
			{
			case EItemTypeEnum::Primary:
				if (bIsPrimaryItemEquipped)
				{
					UAnimMontage* UnequipSecondaryMontage			= GetUnequipMontage(*SecondaryItem);
					if (UnequipSecondaryMontage && PlayerAnimInstance)
					{
						PlayerAnimInstance->Montage_Play(UnequipSecondaryMontage, 1.75f);
						CurrentEquippingItemType = EItemTypeEnum::Secondary;
						// Bind a delegate to the montage end event if needed
						PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
						
						FOnMontageEnded MontageEndedDelegate;
						MontageEndedDelegate.BindLambda([this, Placeholder1Item, PrimaryItem](UAnimMontage* Montage, bool bInterrupted)
						{
							if (!bInterrupted)
							{
								// Montage completed successfully
								// Your completion logic here

								UAnimMontage* UnequipPalceholder1Montage		= GetUnequipMontage(*Placeholder1Item);
								if (UnequipPalceholder1Montage && PlayerAnimInstance)
								{
									PlayerAnimInstance->Montage_Play(UnequipPalceholder1Montage, 1.75f);
									CurrentEquippingItemType = EItemTypeEnum::Placeholder1;
									// Bind a delegate to the montage end event if needed
									PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
									FOnMontageEnded MontageEndedDelegate;
									MontageEndedDelegate.BindLambda([this, PrimaryItem](UAnimMontage* Montage, bool bInterrupted)
									{
										if (!bInterrupted)
										{
											// Montage completed successfully
											// Your completion logic here
											UAnimMontage* EquipPrimaryMontage				= GetEquipMontage(*PrimaryItem);
											if (EquipPrimaryMontage && PlayerAnimInstance)
											{
												PlayerAnimInstance->Montage_Play(EquipPrimaryMontage, 1.75f);
												CurrentEquippingItemType = EItemTypeEnum::Primary;
												// Bind a delegate to the montage end event if needed
												PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnEquipNotifyBegin);
											}
								
										}
									});
									PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, UnequipPalceholder1Montage);
								}
							}
						});
						PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, UnequipSecondaryMontage);
					}
				}
				else
				{
					if (FoundItem->Equipped)
					{
						UAnimMontage* UnequipMontage = GetUnequipMontage(*FoundItem);
						if (UnequipMontage && PlayerAnimInstance)
						{
							PlayerAnimInstance->Montage_Play(UnequipMontage, 1.75f);
							// Bind a delegate to the montage end event if needed
							PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
						}
						
					}
					else
					{
						UAnimMontage* EquipMontage = GetEquipMontage(*FoundItem);
						if (EquipMontage && PlayerAnimInstance)
						{
							PlayerAnimInstance->Montage_Play(EquipMontage, 1.75f);
							// Bind a delegate to the montage end event if needed
							PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnEquipNotifyBegin);
						}
							
					}
					
				}
				break;
			case EItemTypeEnum::Secondary:
				if (bIsSecondaryItemEquipped)
				{
					UAnimMontage* UnequipPrimaryMontage				= GetUnequipMontage(*PrimaryItem);
					if (UnequipPrimaryMontage && PlayerAnimInstance)
					{
						PlayerAnimInstance->Montage_Play(UnequipPrimaryMontage, 1.75f);
						CurrentEquippingItemType = EItemTypeEnum::Primary;
						// Bind a delegate to the montage end event if needed
						PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
						FOnMontageEnded MontageEndedDelegate;
						MontageEndedDelegate.BindLambda([this, Placeholder1Item, SecondaryItem](UAnimMontage* Montage, bool bInterrupted)
						{
							if (!bInterrupted)
							{
								// Montage completed successfully
								// Your completion logic here
								UAnimMontage* UnequipPalceholder1Montage		= GetUnequipMontage(*Placeholder1Item);
								if (UnequipPalceholder1Montage && PlayerAnimInstance)
								{
									PlayerAnimInstance->Montage_Play(UnequipPalceholder1Montage, 1.75f);
									CurrentEquippingItemType = EItemTypeEnum::Placeholder1;
									// Bind a delegate to the montage end event if needed
									FOnMontageEnded MontageEndedDelegate;
									MontageEndedDelegate.BindLambda([this, SecondaryItem](UAnimMontage* Montage, bool bInterrupted)
									{
										if (!bInterrupted)
										{
											// Montage completed successfully
											// Your completion logic here
											UAnimMontage* EquipSecondaryMontage				= GetEquipMontage(*SecondaryItem);
											if (EquipSecondaryMontage && PlayerAnimInstance)
											{
												PlayerAnimInstance->Montage_Play(EquipSecondaryMontage, 1.75f);
												CurrentEquippingItemType = EItemTypeEnum::Secondary;
												// Bind a delegate to the montage end event if needed
												PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnEquipNotifyBegin);
											}
										}
									});
									PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, UnequipPalceholder1Montage);

									
									PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
								}
							}
						});
						PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, UnequipPrimaryMontage);
					}
					
					
				}
				else
				{
					if (FoundItem->Equipped)
					{
						UAnimMontage* UnequipMontage = GetUnequipMontage(*FoundItem);
						if (UnequipMontage && PlayerAnimInstance)
						{
							PlayerAnimInstance->Montage_Play(UnequipMontage, 1.75f);
							// Bind a delegate to the montage end event if needed
							PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
						}
					}
					else
					{
						UAnimMontage* EquipMontage = GetEquipMontage(*FoundItem);
						if (EquipMontage && PlayerAnimInstance)
						{
							PlayerAnimInstance->Montage_Play(EquipMontage, 1.75f);
							// Bind a delegate to the montage end event if needed
							PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnEquipNotifyBegin);
						}
							
					}
				}
				break;
			case EItemTypeEnum::Placeholder1:
				if (bIsPlaceholder1ItemEquipped)
				{
					UAnimMontage* UnequipPrimaryMontage				= GetUnequipMontage(*PrimaryItem);
					if (UnequipPrimaryMontage && PlayerAnimInstance)
					{
						PlayerAnimInstance->Montage_Play(UnequipPrimaryMontage, 1.75f);
						CurrentEquippingItemType = EItemTypeEnum::Primary;
						// Bind a delegate to the montage end event if needed
						PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
						FOnMontageEnded MontageEndedDelegate;
						MontageEndedDelegate.BindLambda([this, SecondaryItem, Placeholder1Item](UAnimMontage* Montage, bool bInterrupted)
						{
							if (!bInterrupted)
							{
								// Montage completed successfully
								// Your completion logic here
								UAnimMontage* UnequipSecondaryMontage			= GetUnequipMontage(*SecondaryItem);
								if (UnequipSecondaryMontage && PlayerAnimInstance)
								{
									PlayerAnimInstance->Montage_Play(UnequipSecondaryMontage, 1.75f);
									CurrentEquippingItemType = EItemTypeEnum::Secondary;
									// Bind a delegate to the montage end event if needed
									PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
									FOnMontageEnded MontageEndedDelegate;
									MontageEndedDelegate.BindLambda([this, Placeholder1Item](UAnimMontage* Montage, bool bInterrupted)
									{
										if (!bInterrupted)
										{
											// Montage completed successfully
											// Your completion logic here
											UAnimMontage* EquipPalceholder1Montage			= GetEquipMontage(*Placeholder1Item);
											if (EquipPalceholder1Montage && PlayerAnimInstance)
											{
												PlayerAnimInstance->Montage_Play(EquipPalceholder1Montage, 1.75f);
												CurrentEquippingItemType = EItemTypeEnum::Placeholder1;
												// Bind a delegate to the montage end event if needed
												PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnEquipNotifyBegin);
											}
										}
									});
									PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, UnequipSecondaryMontage);

								}
							}
						});
						PlayerAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, UnequipPrimaryMontage);

					}
					
					
				}
				else
				{
					if (FoundItem->Equipped)
					{
						UAnimMontage* UnequipMontage = GetUnequipMontage(*FoundItem);
						if (UnequipMontage && PlayerAnimInstance)
						{
							PlayerAnimInstance->Montage_Play(UnequipMontage, 1.75f);
							// Bind a delegate to the montage end event if needed
							PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnUnequipNotifyBegin);
						}
					}
					else
					{
						UAnimMontage* EquipMontage = GetEquipMontage(*FoundItem);
						if (EquipMontage && PlayerAnimInstance)
						{
							PlayerAnimInstance->Montage_Play(EquipMontage, 1.75f);
							// Bind a delegate to the montage end event if needed
							PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASlashPlayerCharacter::OnEquipNotifyBegin);
						}
							
					}
				}
				break;
			}
			
		}
		
	}
}


void ASlashPlayerCharacter::OnEquipNotifyBegin(FName NotifyName,
                                               const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	EquipItem(CurrentEquippingItemType);
}

void ASlashPlayerCharacter::OnUnequipNotifyBegin(FName NotifyName,
	const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	UnequipItem(CurrentEquippingItemType);
}

void ASlashPlayerCharacter::SetEquipStatus(EItemTypeEnum ItemType, bool bEquipped)
{
	FEquippableStruct* FoundItem = EquippableSetup.Find(ItemType);
	if (FoundItem)
	{
		FoundItem->Equipped = bEquipped;
		
		ASlashEquippableItemMaster* ActorInstance = FoundItem->ActorRef ? Cast<ASlashEquippableItemMaster>(FoundItem->ActorRef) : nullptr;
		if (ActorInstance)
		{
			ActorInstance->TogglePhysics(!bEquipped);
			ActorInstance->SetPivot(bEquipped);
		}
		switch (FoundItem->EquipHand)
		{
		case EEquipHandEnum::HoldItem_Sword_r:
			if (bEquipped)
			{
				bRightHandClosed = ActorInstance->ClosedFist;
			}
			else
			{
				bRightHandClosed = false;
			}
			
			break;
		case EEquipHandEnum::HoldItem_Bow_l:
			if (bEquipped)
			{
				bLeftHandClosed = ActorInstance->ClosedFist;
			}
			else
			{
				bLeftHandClosed = false;
			}
			
			break;
		}
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
