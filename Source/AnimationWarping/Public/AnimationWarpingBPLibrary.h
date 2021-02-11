// Written by Berkay Tuna, August 2020

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimationWarpingBPLibrary.generated.h"

UCLASS()
class UAnimationWarpingBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Math")
	static float GetInputDirectionInDegrees(float ForwardInput, float RightInput);
};
