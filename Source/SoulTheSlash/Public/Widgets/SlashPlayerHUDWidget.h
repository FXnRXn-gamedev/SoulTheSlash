// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashPlayerHUDWidget.generated.h"

class USlashPlayerStatWidget;


UCLASS()
class SOULTHESLASH_API USlashPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "---Slash---|HUD")
	void SetPlayerStatWidget(USlashPlayerStatWidget* StatWidget);

protected:
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* StatWidgetContainer; // This should match your UMG panel name

private:
	UPROPERTY()
	USlashPlayerStatWidget* PlayerStatWidget;

	
};
