// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SlashMacrosLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SOULTHESLASH_API USlashMacrosLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "---Slash---|Flow Control")
	static void OpenGate(bool& bIsGateOpen);

	UFUNCTION(BlueprintCallable, Category = "---Slash---|Flow Control")
	static void CloseGate(bool& bIsGateOpen);

	UFUNCTION(BlueprintCallable, Category = "---Slash---|Flow Control")
	static void ToggleGate(bool& bIsGateOpen);

	UFUNCTION(BlueprintCallable, Category = "---Slash---|Flow Control")
	static bool PassThroughGate(bool bIsGateOpen);

	UFUNCTION(BlueprintCallable, Category = "---Slash---|Flow Control")
	static void Gate(bool bCondition, bool& bIsGateOpen);

	
};
