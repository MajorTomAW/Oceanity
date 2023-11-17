// Copyright © 2023 MajorT, Shimizu Izumi. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "Slate/SlateBrushAsset.h"
#include "ButtonBase.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonClickedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonHovered);

UENUM(BlueprintType)
enum class EButtonType : uint8
{
	TextOnly,
	IconOnly,
	TextAndIcon
};

UCLASS()
class OCEANITYGAME_API UButtonBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextBlock;

	// Event dispatcher
	UPROPERTY( BlueprintAssignable, Category = "Button|Event" )
	FButtonClickedEvent OnClicked;

	UPROPERTY( BlueprintAssignable, Category = "Button|Event" )
	FButtonHovered OnHovered;

protected:
	virtual void NativeConstruct() override;

	virtual void NativePreConstruct() override;

public:
	/** User parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button")
	FText ButtonText = FText::FromString("Button");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button")
	int FontSize = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button|Style")
	EButtonType ButtonType = EButtonType::TextOnly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button|Style")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button|Style")
	bool bActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button|Style")
	bool bBlurBackground = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button|Style")
	USlateBrushAsset* NormalAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Button|Style")
	USlateBrushAsset* ActiveAsset;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="_Button", meta=(AllowedClasses="/Script/Engine.Font", DisplayName="Button Font"))
	TObjectPtr<const UObject> FontObject;

private:
	UFUNCTION()
	FORCEINLINE void OnButtonClicked() {OnClicked.Broadcast();}

	UFUNCTION()
	FORCEINLINE void OnButtonHovered() {OnHovered.Broadcast();}

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonActive(bool bNewActive);
};
