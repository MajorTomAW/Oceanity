// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "ProjectileLibrary.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ProjectileDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* AirBurstExplosion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* ImpactExplosion;
	
};

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UProjectileLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
