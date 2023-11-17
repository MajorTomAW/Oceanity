#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

struct FShipComponentProperty;

// This class does not need to be modified.
UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OCEANITYGAME_API IPlayerInterface
{
	GENERATED_BODY()

public:
	// Called to update the ship component, use in lobby only
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EquipComponent(const FShipComponentProperty NewShipComponent);
};