// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "OceanityGame/Libraries/ShipLibrary.h"
#include "ProjectileBase.generated.h"

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();

	/** Components */
	// Projectile Movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	// Projectile Hitbox
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ProjectileHitbox;

	// Projectile Particle System
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* ProjectileParticleSystem;

	//* Projectile Info */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Projectile")
	FProjectileProperty ProjectileProperty;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when ProjectileHitbox overlaps with another component
	UFUNCTION()
	void OnProjectileHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called to spawn impact particle system
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void NetMulti_SpawnParticleSystem(UNiagaraSystem* ParticleSystem, FVector Location);
};
