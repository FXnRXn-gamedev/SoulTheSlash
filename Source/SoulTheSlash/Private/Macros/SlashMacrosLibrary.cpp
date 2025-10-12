// FXnRXn copyright notice


#include "Macros/SlashMacrosLibrary.h"

void USlashMacrosLibrary::OpenGate(bool& bIsGateOpen)
{
	bIsGateOpen = true;
}

void USlashMacrosLibrary::CloseGate(bool& bIsGateOpen)
{
	bIsGateOpen = false;
}

void USlashMacrosLibrary::ToggleGate(bool& bIsGateOpen)
{
	bIsGateOpen = !bIsGateOpen;
}

bool USlashMacrosLibrary::PassThroughGate(bool bIsGateOpen)
{
	return bIsGateOpen;
}

void USlashMacrosLibrary::Gate(bool bCondition, bool& bIsGateOpen)
{
	bIsGateOpen = bCondition;
}

bool USlashMacrosLibrary::IsFloatInRange(float Value, float Min, float Max, bool bInclusiveMin, bool bInclusiveMax)
{
	bool bGreaterThanMin = bInclusiveMin ? (Value >= Min) : (Value > Min);
	bool bLessThanMax = bInclusiveMax ? (Value <= Max) : (Value < Max);

	return bGreaterThanMin && bLessThanMax;
}
