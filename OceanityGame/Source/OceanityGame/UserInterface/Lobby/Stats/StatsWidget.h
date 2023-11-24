// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "OceanityGame/UserInterface/Panels/Stats/StatsPanel.h"
#include "OceanityGame/Libraries/UILibrary.h"
#include "StatsWidget.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* StatsList;

	UFUNCTION(BlueprintCallable)
	void UpdateStatsList(TArray<FComponentStats> Stats) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UStatsPanel> StatsPanelClass;
};
