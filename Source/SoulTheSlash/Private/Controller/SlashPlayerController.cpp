// FXnRXn copyright notice


#include "Controller/SlashPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/Player/SlashPlayerCharacter.h"


ASlashPlayerController::ASlashPlayerController()
{
	SlashCharacter = nullptr;
}

void ASlashPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Cache reference to controlled character
	SlashCharacter = Cast<ASlashPlayerCharacter>(GetPawn());

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void ASlashPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Movement & Look
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashPlayerController::MoveInput);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashPlayerController::LookInput);
		}
        
		// Actions
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashPlayerController::StartJumpInput);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASlashPlayerController::StopJumpInput);
		}
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASlashPlayerController::SprintInput);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASlashPlayerController::SprintCompletedInput);
		}

		if (CrouchAction)
		{
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ASlashPlayerController::CrouchInput);
		}

		if (RollAction)
		{
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &ASlashPlayerController::RollStartedInput);
		}

		if (CrawlAction)
		{
			EnhancedInputComponent->BindAction(CrawlAction, ETriggerEvent::Started, this, &ASlashPlayerController::CrawlInput);
		}


		// EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &AWukongHero::LightAttack);
		// EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &AWukongHero::HeavyAttack);

		// Combat actions
		// EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &AWukongHero::OnRoll);
		// EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Triggered, this, &AWukongHero::OnParry);
		// EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AWukongHero::OnBlockStarted);
		// EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AWukongHero::OnBlockCompleted);
		// EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AWukongHero::OnDash);
		
	}

}

void ASlashPlayerController::MoveInput(const FInputActionValue& Value)
{
	if (SlashCharacter)
	{
		SlashCharacter->Move(Value);
		SlashCharacter->GetActionKeyName(MoveAction);
	}
}

void ASlashPlayerController::LookInput(const FInputActionValue& Value)
{
	if (SlashCharacter)
	{
		SlashCharacter->Look(Value);
	}

}

void ASlashPlayerController::StartJumpInput()
{
	if (SlashCharacter)
	{
		SlashCharacter->StartJump();
	}

}

void ASlashPlayerController::StopJumpInput()
{
	if (SlashCharacter)
	{
		SlashCharacter->StopJump();
	}

}

void ASlashPlayerController::SprintInput()
{
	if (SlashCharacter)
	{
		SlashCharacter->Sprint();
	}
}

void ASlashPlayerController::SprintCompletedInput()
{
	if (SlashCharacter)
	{
		SlashCharacter->SprintCompleted();
	}
}

void ASlashPlayerController::CrouchInput()
{
	if (SlashCharacter)
	{
		SlashCharacter->PlayerCrouch();
	}
}

void ASlashPlayerController::RollStartedInput()
{
	if (SlashCharacter)
	{
		SlashCharacter->PerformRoll();
	}
}

void ASlashPlayerController::CrawlInput()
{
	if (SlashCharacter && SlashCharacter->bCrawlMode)
	{
		CrawlStopped();
	}
	else
	{
		CrawlStarted();
	}
}

void ASlashPlayerController::CrawlStarted()
{
	if (SlashCharacter)
	{
		SlashCharacter->ToggleCrawlMode();
	}
}

void ASlashPlayerController::CrawlStopped()
{
	if (SlashCharacter)
	{
		SlashCharacter->ToggleCrawlMode();
	}
}


