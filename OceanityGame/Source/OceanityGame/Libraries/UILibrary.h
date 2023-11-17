// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UILibrary.generated.h"

UENUM(BlueprintType)
enum class EActiveComponentLayout : uint8
{
	TurretComponents,
	EngineComponents,
	HullComponents
};

/** UI Button colors */
const FSlateColor ActiveColor = FSlateColor(FLinearColor(FColor::FromHex("#00668EFF")));
const FSlateColor InactiveColor = FSlateColor(FLinearColor(FColor::FromHex("#FAF3DD80")));
const FSlateColor HoveredColor = FSlateColor(FLinearColor(FColor::FromHex("#FAF3DD99")));
const FSlateColor PressedColor = FSlateColor(FLinearColor(FColor::FromHex("#FAF3DD66")));

/**
 * Stats struct
 */
USTRUCT(BlueprintType)
struct FComponentStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Unit = "";
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue = 0.f;
};

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UUILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};


