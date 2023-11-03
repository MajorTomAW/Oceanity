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

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UUILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};


