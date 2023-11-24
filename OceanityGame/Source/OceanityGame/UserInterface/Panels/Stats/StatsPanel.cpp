// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.


#include "StatsPanel.h"

void UStatsPanel::LinkComponentStats(FComponentStats Stats)
{
	ComponentStats = Stats;
	if (FMath::RoundToInt(ComponentStats.Value) * 10 == ComponentStats.Value * 10)
	{
		Title->SetText(FText::FromString(ComponentStats.Name + ": " + FString::FromInt(FMath::RoundToInt(ComponentStats.Value)) + ComponentStats.Unit));	
	}
	else
	{
		Title->SetText(FText::FromString(ComponentStats.Name + ": " + FString::SanitizeFloat(ComponentStats.Value) + ComponentStats.Unit));
	}
	const float Percentage = ComponentStats.Value / ComponentStats.MaxValue;
	StatsBar->SetPercent(Percentage);
}
