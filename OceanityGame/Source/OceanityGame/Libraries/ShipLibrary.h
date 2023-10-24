// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShipLibrary.generated.h"

class AShipBase;

USTRUCT(BlueprintType, DisplayName = "Movement")
struct FMovementInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Acceleration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TurnSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxSpeed = 0.f;
	
	bool bCanTurn = true;
	bool bCanAccelerate = true;
	bool bCanShoot = true;
};

USTRUCT(BlueprintType, DisplayName = "Ship Info")
struct FShipInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Description = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Cost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Projectiles = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AShipBase> ShipClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMovementInfo MovementInfo;

	bool bLocked = false;
};

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Outside,
	Inside,
	Scoped
};

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UShipLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
