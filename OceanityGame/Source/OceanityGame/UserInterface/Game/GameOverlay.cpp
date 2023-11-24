// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverlay.h"

void UGameOverlay::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameOverlay::UpdateTimer(float Seconds, float Minutes)
{
  FString SecondsText = FString::SanitizeFloat(Seconds);
  FString MinutesText = FString::SanitizeFloat(Minutes);
  
  if (Seconds < 10) SecondsText = "0" + SecondsText;
  if (Minutes < 10) MinutesText = "0" + MinutesText;
  
  const FText TimerText = FText::FromString(MinutesText + ":" + SecondsText);
  TimerDisplayText->SetText(TimerText);
}
