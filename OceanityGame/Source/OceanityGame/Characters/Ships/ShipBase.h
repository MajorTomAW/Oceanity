// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OceanityGame/Libraries/ShipLibrary.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "OceanityGame/Characters/CharacterBase.h"
#include "ShipBase.generated.h"

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
	UCameraComponent* OutsideCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* InsideCamera;
	
	/** Enhanced Movement*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Movement")
	UInputMappingContext* MovementContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Movement")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Movement")
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|Movement")
	UInputAction* LookAction;

	/** Ship Info */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship")
	FShipInfo ShipInfo;

protected:
	/** Values */
	UPROPERTY(Replicated)
	float CurrentVelocity = 0.f;

	UPROPERTY(Replicated)
	float InputVelocity = 0.f;

	ECameraState CameraState = ECameraState::Outside;

	// Buffer for when the ship is at 0 velocity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions")
	float ZeroBuffer = 50.f;

	// Ship backwards velocity multiplier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions")
	float BackwardsMultiplier = 0.75f;

	// Camera sensitivity multiplayer outside
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions|Sensitivity")
	float CamSensitivityMultiplier_Outside = 1.5f;

	// Camera sensitivity multiplayer inside
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions|Sensitivity")
	float CamSensitivityMultiplier_Inside = 0.75f;

	// Camera sensitivity multiplayer scoped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Ship|DeveloperOptions|Sensitivity")
	float CamSensitivityMultiplier_Scoped = 0.25f;

	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Enhanced Movement */
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
		
	void Shoot();

	void Accelerate(float Value);
	void Turn(float Value);

	void CalculateVelocity(float Value);
	
	/** Server */
	UFUNCTION(Server, Reliable)
	void Server_CalculateVelocity(float Value);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Function to replicate variables to clients
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
