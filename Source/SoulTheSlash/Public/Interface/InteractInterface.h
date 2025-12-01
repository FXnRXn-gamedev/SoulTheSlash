// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};


class IInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "---Slash---|Interface|Interaction")
	void Interact(AActor* Actor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "---Slash---|Interface|Interaction")
	bool CanInteract(AActor* Actor);
};
