// FXnRXn copyright notice


#include "AnimInstance/SlashPlayerAnimInstance.h"
#include "Characters/Player/SlashPlayerCharacter.h"

void USlashPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OwningCharacterAnimInstance = Cast<USlashCharacteAnimInstanceBase>(OwnerCharacter->GetMesh()->GetAnimInstance());
}
