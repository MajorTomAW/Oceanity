// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ShipComponentPanel.generated.h"

class ULobbyWidget;
struct FShipComponentProperty;

/**
 * 
 */
UCLASS()
class OCEANITYGAME_API UShipComponentPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ComponentImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ComponentCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ComponentButton;

	void LinkShipComponent(const FShipComponentProperty* NewShipComponentProperty, const FName RowName);
	void ComparePrice(int32 Gold);

	// Set owner
	UFUNCTION()
	void SetOwner(ULobbyWidget* NewOwner);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PanelEnabled(bool bEnabled);

	UFUNCTION(BlueprintImplementableEvent)
	void SetIsComponentOwned(bool bOwned);

	FShipComponentProperty* GetShipComponentProperty() const {return ShipComponentProperty;}

protected:
	virtual void NativeConstruct() override;
	FShipComponentProperty* ShipComponentProperty;

	UFUNCTION()
	void OnComponentButtonClicked();

private:
	ULobbyWidget* WidgetOwner;

public:
	FName ThisRowName;
};
