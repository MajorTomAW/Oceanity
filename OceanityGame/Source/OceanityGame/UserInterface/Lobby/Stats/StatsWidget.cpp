// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.


#include "StatsWidget.h"

#include "OceanityGame/UserInterface/Panels/Stats/StatsPanel.h"

void UStatsWidget::UpdateStatsList(TArray<FComponentStats> Stats) const
{
	UE_LOG(LogTemp, Warning, TEXT("StatsList Updating"));
	StatsList->ClearChildren();
	const TSubclassOf<UStatsPanel> StatsPanelClass =  StaticLoadClass(UStatsPanel::StaticClass(), nullptr, TEXT("/Game/Blueprints/UserInterface/Lobby/Panels/W_StatsPanel.W_StatsPanel_C"));

	for (FComponentStats ComponentStats : Stats)
	{
		UStatsPanel* StatsPanel = CreateWidget<UStatsPanel>(GetWorld(), StatsPanelClass);
		StatsPanel->LinkComponentStats(ComponentStats);
		StatsList->AddChild(StatsPanel);
	}
	UE_LOG(LogTemp, Warning, TEXT("StatsList Updated"));
}
