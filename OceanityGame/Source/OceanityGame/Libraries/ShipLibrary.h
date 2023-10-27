// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShipLibrary.generated.h"

USTRUCT(BlueprintType, DisplayName = "Movement")
struct FMovementInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationForce = 600000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAcceleration = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationHelperRange = 10.f;

	// Boost when velocity is out of range of "InputBoostRange"
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationMultiplier = 1.0f;

	// Boost when velocity is in range of "InputBoostRange"
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationHelperMultiplier = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TurnSpeedForce = 200000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TurnSpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaTimeMultiplier = 100.f;
	
	
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
	TSubclassOf<AActor> ShipClass = nullptr;

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
