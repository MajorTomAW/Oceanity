// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShipLibrary.generated.h"

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Outside,
	Inside,
	Scoped
};

/** Movement property */
USTRUCT(BlueprintType, DisplayName = "Ship Movement Property")
struct FShipMovementProperty
{
	GENERATED_BODY()

	// Delta time multiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeltaTimeMultiplier = 100.f;
};

/** Parent component for turret, engine, hull, ... */
USTRUCT(BlueprintType, DisplayName = "Ship Component", meta=(IsPolymorphic = true))
struct FShipComponents : public FTableRowBase
{
	GENERATED_BODY()

	// Component name
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ComponentName = "";
	
	// Component icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ComponentIcon = nullptr;

	// Component mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* ComponentMesh = nullptr;

	// Component cost
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int ComponentCost = 0;
};

// Turret component
USTRUCT(BlueprintType, DisplayName = "Turret Component")
struct FTurretComponent : public FShipComponents
{
	GENERATED_BODY()

	// Turret fire rate
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireRate = 0.f;

	// Turret damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 0.f;

	// Turret projectile class
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass = nullptr;
};

// Engine component
USTRUCT(BlueprintType, DisplayName = "Engine Component")
struct FEngineComponent : public FShipComponents
{
	GENERATED_BODY()
		
	// Engine acceleration force
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationForce = 600000.f;

	// Engine max acceleration
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAcceleration = 800.f;

	// Engine acceleration helper range
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationHelperRange = 10.f;

	// Boost when velocity is out of range of "InputBoostRange"
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationMultiplier = 1.0f;

	// Boost when velocity is in range of "InputBoostRange"
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AccelerationHelperMultiplier = 2.0f;

	// Engine turn speed force
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TurnSpeedForce = 200000.0f;

	// Engine turn speed multiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TurnSpeedMultiplier = 1.0f;

	// Engine movement property
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FShipMovementProperty MovementProperty;
	
	bool bCanTurn = true;
	bool bCanAccelerate = true;
};

// Hull component
USTRUCT(BlueprintType, DisplayName = "Hull Component")
struct FHullComponent : public FShipComponents
{
	GENERATED_BODY()

	// Hull health
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 0.f;

	// Hull mass
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Mass = 0.f;
};

/** Projectile Property */
USTRUCT(BlueprintType, DisplayName = "Projectile Property")
struct FProjectileProperty
{
	GENERATED_BODY()

	// Projectile damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 0.f;

	// Projectile speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed = 0.f;

	// Projectile impact impulse
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ImpactImpulse = 0.f;

	/** vfx/sfx */
	// Projectile impact effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNiagaraSystem* ImpactEffect = nullptr;

	// Projectile hit sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* HitSound = nullptr;

	// Projectile no hit sound
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* NoHitSound = nullptr;
};

/** Ship property */
USTRUCT(BlueprintType, DisplayName = "Ship Property")
struct  FShipProperty : public FTableRowBase
{
	GENERATED_BODY()

	// Name of the ship
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Name = "";

	// Color of the ship
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor Color = FLinearColor::Blue;

	// Movement property
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FShipMovementProperty MovementProperty;

	// Hull component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FHullComponent HullComponent;

	// Engine component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEngineComponent EngineComponent;

	// Turret component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTurretComponent TurretComponent;
};

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UShipLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
