// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "OceanityGame/Libraries/UILibrary.h"
#include "StatsPanel.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UStatsPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* StatsBar;

private:
	FComponentStats ComponentStats;

public:
	void LinkComponentStats(const FComponentStats& Stats);
};
