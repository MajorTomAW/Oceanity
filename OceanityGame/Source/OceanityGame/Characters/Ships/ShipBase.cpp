// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

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

void AShipBase::Server_ChangeAcceleration_Implementation(float MaxAcceleration, float AccelerationForce)
{
	ShipInfo.MovementInfo.MaxAcceleration = MaxAcceleration;
	ShipInfo.MovementInfo.AccelerationForce = AccelerationForce;
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
	const FMovementInfo MovementInfo = ShipInfo.MovementInfo;
	
	// Acceleration
	if (MovementInfo.bCanAccelerate && Velocity <= MovementInfo.MaxAcceleration && InputValue.Y != 0)
	{
		const float BoostRange = MovementInfo.AccelerationHelperRange;
		float ForceMultiplier = 0.f;

		// Check if the velocity is in the boost range
		if (Velocity < BoostRange && Velocity > -BoostRange)
		{
			ForceMultiplier = MovementInfo.AccelerationHelperMultiplier;
		}
		else
		{
			ForceMultiplier = MovementInfo.AccelerationMultiplier;
		}
		
		// UE_LOG the ForceMultiplier
		UE_LOG(LogTemp, Warning, TEXT("ForceMultiplier: %f"), ForceMultiplier);
		
		ForceMultiplier *= ShipInfo.MovementInfo.AccelerationForce;
		const FVector Force = GetActorForwardVector() * InputValue.Y * GetWorld()->DeltaTimeSeconds * 100 * ForceMultiplier;

		// Add force to both engines to achieve a linear movement
		GetCapsuleComponent()->AddForceAtLocation(Force, EngineLeft->GetComponentLocation());
		GetCapsuleComponent()->AddForceAtLocation(Force, EngineRight->GetComponentLocation());
	}

	// Turning
	if (ShipInfo.MovementInfo.bCanTurn && Velocity != 0.f && InputValue.X != 0)
	{
		const UArrowComponent* Engine = InputValue.X == 1.f ? EngineLeft : EngineRight;
		const FVector Force = GetActorForwardVector() * GetWorld()->DeltaTimeSeconds * 100 * MovementInfo.TurnSpeedForce * MovementInfo.TurnSpeedMultiplier;

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
		Multiplier = CamSensitivityMultiplier_Inside;
		break;
	case ECameraState::Scoped:
		Multiplier = CamSensitivityMultiplier_Scoped;
		break;
	}
	
	AddControllerPitchInput(Value.Get<FVector2d>().Y * Multiplier);
	AddControllerYawInput(Value.Get<FVector2d>().X * Multiplier);
}

void AShipBase::Shoot()
{
}

void AShipBase::Aim()
{
}

void AShipBase::ToggleView()
{
	switch (CameraState)
	{
	case ECameraState::Outside:
		CameraState = ECameraState::Inside;
		SpringArm->TargetArmLength = 0.f;
		break;
	case ECameraState::Inside:
		CameraState = ECameraState::Outside;
		SpringArm->TargetArmLength = 1200.f;
		break;
	case ECameraState::Scoped:
		CameraState = ECameraState::Outside;
		SpringArm->TargetArmLength = 1200.f;
		break;
	}
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

		// Toggle View
		EnhancedInputComponent->BindAction(ToggleViewAction, ETriggerEvent::Started, this, &AShipBase::ToggleView);
	}
}

void AShipBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShipBase, CurrentVelocity);
	DOREPLIFETIME(AShipBase, InputVelocity);
}

