// Copyright © 2023 MajorT. All rights reserved.


#include "StatsWidget.h"

void UStatsWidget::UpdateStatsList(TArray<FComponentStats> Stats) const
{
	UE_LOG(LogTemp, Warning, TEXT("StatsList Updating"));
	StatsList->ClearChildren();

	for (const FComponentStats ComponentStats : Stats)
	{
		if (UStatsPanel* StatsPanel = CreateWidget<UStatsPanel>(GetWorld(), StatsPanelClass))
		{
			StatsPanel->LinkComponentStats(ComponentStats);
			StatsList->AddChild(StatsPanel);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("StatsList Updated"));
}
