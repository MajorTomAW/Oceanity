// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OceanityGame/GameObjects/ShipComponents/ExecuteProperty/ExecuteProperty.h"
#include "ShipLibrary.generated.h"

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	Outside,
	Inside,
	Scoped
};

UENUM(BlueprintType)
enum class EShipComponentType : uint8
{
	Turret,
	Engine,
	Hull
};

/** Projectile Property */
USTRUCT(BlueprintType, DisplayName = "Projectile Property")
struct FProjectileProperty
{
	GENERATED_BODY()

	// Projectile damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.f;

	// Projectile speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.f;

	// Projectile impact impulse
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpactImpulse = 0.f;

	/** vfx/sfx */
	// Projectile impact effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* ImpactEffect = nullptr;

	// Projectile hit sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound = nullptr;

	// Projectile no hit sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* NoHitSound = nullptr;
};

/** Movement property */
USTRUCT(BlueprintType, DisplayName = "Ship Movement Property")
struct FShipMovementProperty
{
	GENERATED_BODY()

	// Delta time multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaTimeMultiplier = 100.f;
};

/** Parent component for turret, engine, hull, ... */
USTRUCT(BlueprintType, DisplayName = "Ship Component", meta=(IsPolymorphic = true))
struct FShipComponentProperty : public FTableRowBase
{
	GENERATED_BODY()

	// Component name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ComponentName = "";

	// Component id must be the same as the row name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ComponentId = "";
	
	// Component icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ComponentIcon = nullptr;

	// Component mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* ComponentMesh = nullptr;

	// Component type
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EShipComponentType ComponentType = EShipComponentType::Turret;

	// Component cost
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ComponentCost = 0;

	bool operator==(const FShipComponentProperty& Other) const
	{
		return ComponentName == Other.ComponentName;
	}
};

// Turret component
USTRUCT(BlueprintType, DisplayName = "Turret Component")
struct FTurretComponentProperty : public FShipComponentProperty
{
	GENERATED_BODY()

	FTurretComponentProperty()
	{
		ComponentType = EShipComponentType::Turret;
	}

	// Projectile property
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FProjectileProperty ProjectileProperty;

	// Execute Property
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UExecuteProperty> ExecuteProperty;

	// Turret shoot cooldown
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootCooldown = 0.f;
	
	// Turret projectile class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ProjectileClass = nullptr;

	bool operator==(const FTurretComponentProperty& Other) const
	{
		return ComponentName == Other.ComponentName;
	}
};

// Engine component
USTRUCT(BlueprintType, DisplayName = "Engine Component")
struct FEngineComponentProperty : public FShipComponentProperty
{
	GENERATED_BODY()

	FEngineComponentProperty()
	{
		ComponentType = EShipComponentType::Engine;
	}
		
	// Engine acceleration force
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 0.f;

	// Engine max acceleration
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAcceleration = 0.f;

	// Engine backwards multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BackwardsMultiplier = 1.f;

	// Engine acceleration helper range
	float AccelerationHelperRange = 0.f;

	// Boost when velocity is out of range of "InputBoostRange"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccelerationMultiplier = 1.0f;

	// Boost when velocity is in range of "InputBoostRange"
	float AccelerationHelperMultiplier = 2.0f;

	// Engine turn speed force
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnSpeedForce = 0.0f;

	// Engine turn speed multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnSpeedMultiplier = 1.0f;

	// Engine movement property
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FShipMovementProperty MovementProperty;
	
	bool bCanTurn = true;
	bool bCanAccelerate = true;

	bool operator==(const FEngineComponentProperty& Other) const
	{
		return ComponentName == Other.ComponentName;
	}
};

// Hull component
USTRUCT(BlueprintType, DisplayName = "Hull Component")
struct FHullComponentProperty : public FShipComponentProperty
{
	GENERATED_BODY()

	FHullComponentProperty()
	{
		ComponentType = EShipComponentType::Hull;
	}
	
	// Hull health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 0.f;

	// Hull mass
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass = 0.f;

	// Damage reduction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageReduction = 0.f;

	// Hull movement multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HullMovementMultiplier = 1.f;

	bool operator==(const FHullComponentProperty& Other) const
	{
		return ComponentName == Other.ComponentName &&
			   Health == Other.Health &&
			   Mass == Other.Mass;
	}
};

/** Ship property */
USTRUCT(BlueprintType, DisplayName = "Ship Property")
struct  FShipProperty : public FTableRowBase
{
	GENERATED_BODY()

	// Name of the ship
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = "";

	// Color of the ship
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::Blue;

	// Movement property
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FShipMovementProperty MovementProperty;

	// Hull component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHullComponentProperty HullComponent;

	// Engine component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEngineComponentProperty EngineComponent;

	// Turret component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTurretComponentProperty TurretComponent;
};

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UShipLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
