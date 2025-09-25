// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_RightFoot.generated.h"

/**
 * 
 */
UCLASS()
class SOULTHESLASH_API UAnimNotify_RightFoot : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
