// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GameOverlay.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UGameOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
  /** Components */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
  UTextBlock* TimerDisplayText;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
  UProgressBar* Healthbar;
  


	virtual void NativeConstruct() override;

  /** Functions */

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowScope(bool bShow);
	
	UFUNCTION(BlueprintCallable)
	void UpdateTimer(float Seconds, float Minutes)
};
