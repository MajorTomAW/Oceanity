// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTurretExecute.h"

#include "OceanityGame/Characters/Ships/ShipBase.h"
#include "OceanityGame/GameObjects/Projectiles/ProjectileBase.h"

void UBaseTurretExecute::ExecuteProperty_Implementation(AShipBase* Parent, bool& Success)
{
	Super::ExecuteProperty_Implementation(Parent, Success);

	// Spawn parameters
	const FVector SpawnLocation = Parent->ProjectileSpawn->GetComponentLocation();
	const FRotator SpawnRotation = Parent->ProjectileSpawn->GetComponentRotation();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Parent;
	SpawnParameters.Instigator = Parent->GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn projectile
	const FTurretComponentProperty TurretComponent = Parent->ShipProperty.TurretComponent;
	if (AActor* Projectile = GetWorld()->SpawnActor<AActor>(TurretComponent.ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters))
	{
		// Set projectile info
		if (AProjectileBase* ProjectileBase = Cast<AProjectileBase>(Projectile))
		{
			ProjectileBase->ProjectileProperty = TurretComponent.ProjectileProperty;
			Parent->NetMulti_SpawnParticleSystem(Parent->MuzzleFlash, SpawnLocation, SpawnRotation);
		}
	}
}
