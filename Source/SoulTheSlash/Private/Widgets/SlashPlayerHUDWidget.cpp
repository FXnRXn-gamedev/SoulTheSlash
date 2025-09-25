// FXnRXn copyright notice


#include "Widgets/SlashPlayerHUDWidget.h"
#include "Widgets/SlashPlayerStatWidget.h"
#include "Components/PanelWidget.h"


void USlashPlayerHUDWidget::SetPlayerStatWidget(USlashPlayerStatWidget* StatWidget)
{
	if (StatWidget && StatWidgetContainer)
	{
		PlayerStatWidget = StatWidget;
		StatWidgetContainer->AddChild(StatWidget);
	}

}
