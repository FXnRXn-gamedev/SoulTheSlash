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

