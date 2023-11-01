// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "OceanityGame/GameObjects/Projectiles/ProjectileBase.h"
#include "OceanityGame/Interfaces/ControllerInterface.h"
#include "OceanityGame/PlayerController/Game/GameController.h"

// Sets default values
AShipBase::AShipBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1200.f ;
	SpringArm->bUsePawnControlRotation = true;

	EngineLeft = CreateDefaultSubobject<UArrowComponent>(TEXT("EngineLeft"));
	EngineLeft->SetupAttachment(RootComponent);

	EngineRight = CreateDefaultSubobject<UArrowComponent>(TEXT("EngineRight"));
	EngineRight->SetupAttachment(RootComponent);

	ShipBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipBody"));
	ShipBody->SetupAttachment(RootComponent);
	ShipCabin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipCabin"));
	ShipCabin->SetupAttachment(ShipBody);
	TurretBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	TurretBase->SetupAttachment(ShipCabin);
	Turret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurrentGun"));
	Turret->SetupAttachment(TurretBase);
	
	ProjectileSpawn = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawn"));
	ProjectileSpawn->SetupAttachment(Turret);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Outside Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bAutoActivate = true;

	// Add Ship tag
	Tags.Add("Ship");
}

// Called when the game starts or when spawned
void AShipBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Add input mappings
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MovementContext, 0);
		}
	}

	// Set InputMode back to Game
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		const FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(false);
	}
}

/** Enhanced Movement */
// Movement
void AShipBase::Move(const FInputActionValue& Value)
{
	Server_Move(Value.Get<FVector2d>());
}

void AShipBase::Server_Move_Implementation(FVector2D InputValue)
{
	const float Velocity = GetVelocity().Length();
	const FEngineComponent EngineComponent = ShipProperty.EngineComponent;
	
	// Acceleration
	if (EngineComponent.bCanAccelerate && Velocity <= EngineComponent.MaxAcceleration && InputValue.Y != 0)
	{
		const float BoostRange = EngineComponent.AccelerationHelperRange;
		float ForceMultiplier = 0.f;

		// Check if the velocity is in the boost range
		if (Velocity < BoostRange && Velocity > -BoostRange)
		{
			ForceMultiplier = EngineComponent.AccelerationHelperMultiplier;
		}
		else
		{
			ForceMultiplier = EngineComponent.AccelerationMultiplier;
		}
		
		// UE_LOG the ForceMultiplier
		UE_LOG(LogTemp, Warning, TEXT("ForceMultiplier: %f"), ForceMultiplier);
		
		ForceMultiplier *= EngineComponent.AccelerationForce;
		const FVector Force = GetActorForwardVector() * InputValue.Y * GetWorld()->DeltaTimeSeconds * 100 * ForceMultiplier;

		GetCapsuleComponent()->AddForceAtLocation(Force, EngineRight->GetComponentLocation());
		GetCapsuleComponent()->AddForceAtLocation(Force, EngineLeft->GetComponentLocation());
	}

	// Turning
	if (EngineComponent.bCanTurn && Velocity != 0.f && InputValue.X != 0)
	{
		const UArrowComponent* Engine = InputValue.X == 1.f ? EngineLeft : EngineRight;
		const FVector Force = GetActorForwardVector() * GetWorld()->DeltaTimeSeconds * 100 * EngineComponent.TurnSpeedForce * EngineComponent.TurnSpeedMultiplier;

		// Add force to the engine to achieve a turning movement
		GetCapsuleComponent()->AddForceAtLocation(Force, Engine->GetComponentLocation());
		UE_LOG(LogTemp, Warning, TEXT("Force: %f"), Force.Length());
	}
}

void AShipBase::CalculateVelocity(float Value)
{
	/*if (!HasAuthority())
	{
		//Server_CalculateVelocity(Value);
	}
	
	// Calculate velocity
	const float TempVel = CurrentVelocity + (Value * ShipInfo.MovementInfo.Acceleration * GetWorld()->GetDeltaSeconds());

	// Clamp velocity
	CurrentVelocity = FMath::Clamp(TempVel, -ShipInfo.MovementInfo.MaxSpeed, ShipInfo.MovementInfo.MaxSpeed);

	// Zero Buffer
	if (CurrentVelocity < ZeroBuffer && CurrentVelocity > -ZeroBuffer)
	{
		InputVelocity = 0.f;
	}
	else
	{
		CurrentVelocity > 0.f ? InputVelocity = 1.f : InputVelocity = -1.f;
	}

	// Apply new max walk speed
	const float NewMaxSpeed = FMath::Abs(CurrentVelocity < 0.f ? CurrentVelocity * BackwardsMultiplier : CurrentVelocity);
	GetCharacterMovement()->MaxWalkSpeed = NewMaxSpeed;

	// Debug print out CurrentVelocity and InputVelocity
	// UE_LOG(LogTemp, Warning, TEXT("CurrentVelocity: %f, InputVelocity: %f"), GetCharacterMovement()->GetMaxSpeed(), InputVelocity);*/
}

void AShipBase::Server_ChangeAcceleration_Implementation(float MaxAcceleration, float AccelerationForce)
{
	ShipProperty.EngineComponent.MaxAcceleration = MaxAcceleration;
	ShipProperty.EngineComponent.AccelerationForce = AccelerationForce;
}

// Camera look
void AShipBase::Look(const FInputActionValue& Value)
{
	float Multiplier = 0.f;
	switch (CameraState)
	{
	case ECameraState::Outside:
		Multiplier = CamSensitivityMultiplier_Outside;
		break;
	case ECameraState::Inside:
		CalculateControlRotation();
		Multiplier = CamSensitivityMultiplier_Inside;
		break;
	case ECameraState::Scoped:
		CalculateControlRotation();
		Multiplier = CamSensitivityMultiplier_Scoped;
		break;
	}
	
	AddControllerPitchInput(Value.Get<FVector2d>().Y * Multiplier);
	AddControllerYawInput(Value.Get<FVector2d>().X * Multiplier);
}

// Aiming
void AShipBase::Server_ChangeAiming_Implementation(bool bNewAiming)
{
	bAiming = bNewAiming;

	// UE_LOG the bAiming
	UE_LOG(LogTemp, Warning, TEXT("bAiming: %s"), bAiming ? TEXT("true") : TEXT("false"));
}

// Shooting
void AShipBase::Shoot()
{
	Server_ShootProjectile();
}

void AShipBase::Server_ShootProjectile_Implementation()
{
	// Projectile spawn init
	const FVector SpawnLocation = ProjectileSpawn->GetComponentLocation();
	const FRotator SpawnRotation = ProjectileSpawn->GetComponentRotation();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn projectile
	const FTurretComponent TurretComponent = ShipProperty.TurretComponent;
	if (AActor* Projectile = GetWorld()->SpawnActor<AActor>(TurretComponent.ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters))
	{
		// Set projectile info
		if (AProjectileBase* ProjectileBase = Cast<AProjectileBase>(Projectile))
		{
			ProjectileBase->ProjectileProperty.Damage = TurretComponent.Damage;
			NetMulti_SpawnParticleSystem(MuzzleFlash, SpawnLocation, SpawnRotation);
		}
	}
}

void AShipBase::NetMulti_SpawnParticleSystem_Implementation(UNiagaraSystem* ParticleSystem, FVector Location, FRotator Rotation)
{
	// Spawn particle system
	UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlash, ProjectileSpawn, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
}

void AShipBase::Aim()
{
	// Camera change FOV to zoom in
	if (CameraState == ECameraState::Inside)
	{
		Camera->SetFieldOfView(50);
		CameraState = ECameraState::Scoped;
	}
}

void AShipBase::CancelAim()
{
	// Camera change FOV to zoom out
	if (CameraState == ECameraState::Scoped)
	{
		Camera->SetFieldOfView(90);
		CameraState = ECameraState::Inside;
	}
}

// Toggle View
void AShipBase::ToggleView()
{
	switch (CameraState)
	{
	case ECameraState::Outside:
		CameraState = ECameraState::Inside;
		SpringArm->TargetArmLength = 0.f;
		SpringArm->AddRelativeLocation(FVector(0.f, 0.f, -200.f));
		HandleToggleView(true);
		CalculateControlRotation();
		break;
	case ECameraState::Inside:
		CameraState = ECameraState::Outside;
		SpringArm->TargetArmLength = 1200.f;
		SpringArm->AddRelativeLocation(FVector(0.f, 0.f, 200.f));
		HandleToggleView(false);
		break;
	case ECameraState::Scoped:
		// Get out of scoped fov
		CancelAim();
		
		CameraState = ECameraState::Outside;
		SpringArm->TargetArmLength = 1200.f;
		SpringArm->AddRelativeLocation(FVector(0.f, 0.f, 200.f));
		HandleToggleView(false);
		break;
	}
}

void AShipBase::HandleToggleView(bool bInside)
{
	// Client
	Client_HideTurret(bInside);
	
	// Call show scope ui interface
	if (const IControllerInterface* ControllerInterface = Cast<IControllerInterface>(GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleToggleView"));
		ControllerInterface->Execute_ShowScopeUI(GetController(), bInside);
	}

	// Saved last control rotation
	if (bInside)
	{
		if (GetLocalRole() != ROLE_SimulatedProxy)
		{
			SavedControlRotation_Outside = GetControlRotation();

			// Set turret rotation to current control rotation yaw but keep pitch
			FRotator TurretRotation = Turret->GetComponentRotation();
			TurretRotation.Yaw = SavedControlRotation_Outside.Yaw;
			Turret->SetWorldRotation(FRotator(Turret->GetComponentRotation().Pitch, TurretRotation.Yaw, 0.f));
			GetController()->SetControlRotation(FRotator(Turret->GetComponentRotation().Pitch, TurretRotation.Yaw, 0.f));
		}
	}
	else
	{
		if (GetLocalRole() != ROLE_SimulatedProxy)
		{
			SavedControlRotation_Scoped = GetControlRotation();

			// Set control rotation to current turret rotation but only use yaw
			FRotator ControlRotation = SavedControlRotation_Outside;
			ControlRotation.Yaw = Turret->GetComponentRotation().Yaw;
			GetController()->SetControlRotation(ControlRotation);
		}
	}

	// Server
	Server_ChangeAiming(bInside);
}


void AShipBase::Client_HideTurret_Implementation(bool bHide)
{
	// Hide turret
	TurretBase->SetVisibility(!bHide);
	Turret->SetVisibility(!bHide);
}

/** Replicate Aiming */
void AShipBase::CalculateControlRotation()
{
	// Client prediction
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		TurretBase->SetWorldRotation(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		Turret->SetWorldRotation(FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.f));
	}
	
	// Replicate the rotation to the server
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		AimRotation = GetControlRotation();
		Server_ChangeTurretRotation(AimRotation);
	}
}

void AShipBase::Server_ChangeTurretRotation_Implementation(FRotator ControlRotation)
{
	if (bAiming && ControlRotation != FRotator::ZeroRotator)
	{
		AimRotation = ControlRotation;
		NetMulti_ChangeTurretRotation(AimRotation);
	}
}

void AShipBase::NetMulti_ChangeTurretRotation_Implementation(FRotator ControlRotation)
{
	// Apply rotation
	TurretBase->SetWorldRotation(FRotator(0.f, ControlRotation.Yaw, 0.f));
	Turret->SetWorldRotation(FRotator(ControlRotation.Pitch, ControlRotation.Yaw, 0.f));
}

// Called every frame
void AShipBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AShipBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShipBase::Move);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AShipBase::Shoot);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShipBase::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShipBase::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShipBase::CancelAim);

		// Toggle View
		EnhancedInputComponent->BindAction(ToggleViewAction, ETriggerEvent::Started, this, &AShipBase::ToggleView);
	}
}

// Replication
void AShipBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShipBase, CurrentVelocity);
	DOREPLIFETIME(AShipBase, InputVelocity);
	DOREPLIFETIME(AShipBase, bAiming);
	DOREPLIFETIME_CONDITION(AShipBase, AimRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShipBase, SavedControlRotation_Outside, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShipBase, SavedControlRotation_Scoped, COND_SkipOwner);
}

