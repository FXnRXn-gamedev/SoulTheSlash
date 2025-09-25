// FXnRXn copyright notice


#include "Notify/AnimNotify_FallLand.h"
#include "Characters/Player/SlashPlayerCharacter.h"

void UAnimNotify_FallLand::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// Cast the owner to your character class
		ASlashPlayerCharacter* OwnerCharacter = Cast<ASlashPlayerCharacter>(MeshComp->GetOwner());
		if (OwnerCharacter)
		{
			// Call a function on your character
			OwnerCharacter->HandleNotify_FallLand();
		}
	}
}
