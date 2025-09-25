// FXnRXn copyright notice


#include "Component/StateComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// // Initialize a single gameplay tag
	// MyGameplayTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Move"));
	//
	// // Initialize a gameplay tag container
	// MyGameplayTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Buff.Speed")));
	// MyGameplayTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Status.Debuff.Slow")));
	
}

void UStateComponent::MovementInputHandler(float Duration, bool DisableInput)
{
	if (DisableInput)
	{
		MovementInput = true;
	}
	else
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this; // The object that will receive the callback
		LatentInfo.UUID = GetUniqueID(); // A unique ID for this latent action
		LatentInfo.Linkage = 0; // Can be used to link multiple latent actions
		LatentInfo.ExecutionFunction = FName("OnRetriggerableDelayCompleted");

		UKismetSystemLibrary::RetriggerableDelay(this, Duration, LatentInfo);
	}

	
	
	
}

void UStateComponent::OnRetriggerableDelayCompleted()
{
	// This code will execute after the delay finishes
	MovementInput = false; // Re-enable movement input
}

void UStateComponent::SetState(FGameplayTag NewStateTag)
{
	CurrentStateTag = NewStateTag;
}

bool UStateComponent::IsCurrentStateEqualToAnyOfTheseTags(const TArray<FGameplayTagContainer>& StateToCheckTags)
{
	for (const FGameplayTagContainer& TagContainer : StateToCheckTags)
	{
		if (TagContainer.HasTag(CurrentStateTag))
		{
			// Current state matches one of the tags in the array
			return true;
		}
	}
	// Current state does not match any of the tags in the array
	return false;
}


