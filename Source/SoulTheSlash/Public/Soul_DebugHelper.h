#pragma once

namespace Soul_DebugHelper
{
	static void DebugPrint(const FString& Message, const FColor& Color = FColor::MakeRandomColor(), int32 Line = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Line, 5.0f, Color, Message);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}
}