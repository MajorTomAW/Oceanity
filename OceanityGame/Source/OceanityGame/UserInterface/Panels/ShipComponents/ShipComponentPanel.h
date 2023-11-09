// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ShipComponentPanel.generated.h"

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

	void LinkShipComponent(const FShipComponentProperty* NewShipComponentProperty);
	void ComparePrice(int32 Gold);

	UFUNCTION(BlueprintImplementableEvent)
	void SetIsComponentOwned(bool bOwned);

protected:
	virtual void NativeConstruct() override;
	FShipComponentProperty* ShipComponentProperty;
};
