// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OceanityGame/Libraries/ShipLibrary.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "OceanityGame/Characters/CharacterBase.h"
#include "ShipBase.generated.h"

class UNiagaraSystem;
class UExecuteProperty;

UCLASS()
class OCEANITYGAME_API AShipBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShipBase();

	/** Components */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* EngineLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* EngineRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ProjectileSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TurretBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Turret;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ShipBody;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ShipCabin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Engine;
	
	/** Enhanced Movement*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Input")
	UInputMappingContext* MovementContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Input")
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Input")
	UInputAction* ToggleViewAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Input")
	UInputAction* LookAction;

	/** Ship property */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "_Ship")
	FShipProperty ShipProperty;

protected:
	/** Values */
	UPROPERTY(Replicated)
	float CurrentVelocity = 0.f;

	UPROPERTY(Replicated)
	float InputVelocity = 0.f;

	UPROPERTY(Replicated)
	bool bAiming = false;

	UPROPERTY(Replicated)
	FRotator AimRotation;

	UExecuteProperty* TurretExecuteProperty;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions")
	ECameraState CameraState = ECameraState::Outside;

	// Buffer for when the ship is at 0 velocity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions")
	float ZeroBuffer = 50.f;

	// Ship backwards velocity multiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions")
	float BackwardsMultiplier = 0.75f;

	// Camera sensitivity multiplayer outside
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions|Sensitivity")
	float CamSensitivityMultiplier_Outside = 1.0f;

	// Camera sensitivity multiplayer inside
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions|Sensitivity")
	float CamSensitivityMultiplier_Inside = 0.3f;

	// Camera sensitivity multiplayer scoped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions|Sensitivity")
	float CamSensitivityMultiplier_Scoped = 0.1f;
	
public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to overwrite ship stats
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OverwriteShipStats(FShipProperty NewShipProperty);

	// Called to update ship meshes to all clients
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_UpdateShipMeshes(FShipProperty NewShipProperty);

	// Called to change Acceleration
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ChangeAcceleration(float MaxAcceleration, float AccelerationForce);

	// Called to change if player is aiming or not
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ChangeAiming(bool bNewAiming);

	// Called to tell the server to change the turret rotation
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ChangeTurretRotation(FRotator ControlRotation);
	
	// Called to change turret rotation and replicate to all clients
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void NetMulti_ChangeTurretRotation(FRotator ControlRotation);

	// Called to spawn particle system
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void NetMulti_SpawnParticleSystem(UNiagaraSystem* ParticleSystem, FVector Location, FRotator Rotation);

	/** Enhanced Movement */
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_Move(FVector2D InputValue);

	void CalculateVelocity(float Value);

	/** Input Actions */
	void Shoot();

	UFUNCTION(Server, Reliable)
	void Server_ShootProjectile();

	UFUNCTION(Client, Reliable)
	void Client_HideTurret(bool bHide);

	void Aim();
	void CancelAim();

	void ToggleView();

	void HandleToggleView(bool bInside);

	/** Functions */
	// Called to replicate control rotation to clients
	void CalculateControlRotation();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Function to replicate variables to clients
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Muzzle flash particle system
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Particles")
	UNiagaraSystem* MuzzleFlash;

private:
	UPROPERTY(Replicated)
	FRotator SavedControlRotation_Scoped;

	UPROPERTY(Replicated)
	FRotator SavedControlRotation_Outside;
};
