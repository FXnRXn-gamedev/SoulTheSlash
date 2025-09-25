// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULTHESLASH_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateComponent();
	void MovementInputHandler(float Duration, bool DisableInput);
	


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "---Slash---|Setting")
	bool MovementInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Gameplay Tags")
	FGameplayTag CurrentStateTag;
	
private:
	void OnRetriggerableDelayCompleted();
	void SetState(FGameplayTag NewStateTag);
	bool IsCurrentStateEqualToAnyOfTheseTags(const TArray<FGameplayTagContainer>& StateToCheckTags);
		
};
