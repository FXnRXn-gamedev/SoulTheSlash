// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/SlashCharacteAnimInstanceBase.h"
#include "SlashPlayerAnimInstance.generated.h"

class USlashCharacteAnimInstanceBase;


UCLASS()
class SOULTHESLASH_API USlashPlayerAnimInstance : public USlashCharacteAnimInstanceBase
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category= "---Slash---|AnimInstance",meta=(BlueprintThreadSafe))
	USlashCharacteAnimInstanceBase* OwningCharacterAnimInstance;
	
};
