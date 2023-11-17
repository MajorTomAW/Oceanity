// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipComponentPanel.h"

#include "OceanityGame/Libraries/ShipLibrary.h"
#include "OceanityGame/UserInterface/Lobby/LobbyWidget.h"
#include "OceanityGame/UserInterface/Lobby/Stats/StatsWidget.h"

void UShipComponentPanel::NativeConstruct()
{
	Super::NativeConstruct();
	ComponentButton->OnClicked.AddDynamic(this, &UShipComponentPanel::OnComponentButtonClicked);
}

void UShipComponentPanel::OnComponentButtonClicked()
{
	if (WidgetOwner && ShipComponentProperty)
	{
		TArray<FComponentStats> ComponentStats;
		UE_LOG(LogTemp, Warning, TEXT("OnComponentButton Clicked: ComponentType: %i"), ShipComponentProperty->ComponentType);
		switch (ShipComponentProperty->ComponentType)
		{
		case EShipComponentType::Turret:
			{
				const FTurretComponentProperty* TurretComponentProperty = WidgetOwner->TurretComponents->FindRow<FTurretComponentProperty>(ThisRowName, "");

				// Component stats
				ComponentStats.Add(FComponentStats("Damage", "", TurretComponentProperty->ProjectileProperty.Damage, 100.f));
				ComponentStats.Add(FComponentStats("Fire Rate", "s",TurretComponentProperty->ShootCooldown, 10.f));
				ComponentStats.Add(FComponentStats("Bullet Speed", "m/s", TurretComponentProperty->ProjectileProperty.Speed, 15000.f));
				ComponentStats.Add(FComponentStats("Knock-back", "m/s", TurretComponentProperty->ProjectileProperty.ImpactImpulse / 100.f, 100000.f / 100.f));
				break;
			}
			case EShipComponentType::Engine:
			{
				const FEngineComponentProperty* EngineComponentProperty = WidgetOwner->EngineComponents->FindRow<FEngineComponentProperty>(ThisRowName, "");
				
				ComponentStats.Add(FComponentStats("Acceleration", TEXT("m/s\u00B2"), EngineComponentProperty->Acceleration / 10.f, 100.f));
				ComponentStats.Add(FComponentStats("Max Speed", "m/s", EngineComponentProperty->MaxAcceleration, 2000.f));
				ComponentStats.Add(FComponentStats("Backwards", "%", EngineComponentProperty->BackwardsMultiplier * 100.f, 100.f));
				ComponentStats.Add(FComponentStats("Turn Rate", "m/s", EngineComponentProperty->TurnSpeedForce, 50.f));
				break;
			}
			case EShipComponentType::Hull:
			{
				const FHullComponentProperty* HullComponentProperty = WidgetOwner->HullComponents->FindRow<FHullComponentProperty>(ThisRowName, "");

				ComponentStats.Add(FComponentStats("Health", "", HullComponentProperty->Health, 1000.f));
				ComponentStats.Add(FComponentStats("Mass", "kg", HullComponentProperty->Mass, 1000.f));
				ComponentStats.Add(FComponentStats("Shield", "%", HullComponentProperty->DamageReduction, 100.f));
				break;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("OnComponentButton Clicked: ComponentStats Updating Now!"));
		WidgetOwner->StatsWidget->UpdateStatsList(ComponentStats);
		UE_LOG(LogTemp, Warning, TEXT("OnComponentButton Clicked: Selecting New Component Panel Now!"));
		WidgetOwner->SelectNewComponentPanel(*this);
		WidgetOwner->OnComponentSelected(*ShipComponentProperty);
	}
}

void UShipComponentPanel::LinkShipComponent(const FShipComponentProperty* NewShipComponentProperty, const FName RowName)
{
	ThisRowName = RowName;
	ShipComponentProperty = const_cast<FShipComponentProperty*>(NewShipComponentProperty);
	ComponentCost->SetText(FText::FromString(FString::FromInt(ShipComponentProperty->ComponentCost)));
	ComponentName->SetText(FText::FromString(ShipComponentProperty->ComponentName));
	ComponentImage->GetDynamicMaterial()->SetTextureParameterValue(TEXT("Texture"), ShipComponentProperty->ComponentIcon);
}

void UShipComponentPanel::ComparePrice(int32 Gold)
{
	if (ShipComponentProperty->ComponentCost <= Gold)
	{
		// White if can afford
		ComponentCost->SetColorAndOpacity(FLinearColor(0.956f, 0.896f, 0.723f));
	}
	else
	{
		// Red if can't afford
		ComponentCost->SetColorAndOpacity(FLinearColor(0.352f, 0.051f, 0.051f));
	}
}

void UShipComponentPanel::SetOwner(ULobbyWidget* NewOwner)
{
	WidgetOwner = NewOwner;
}
