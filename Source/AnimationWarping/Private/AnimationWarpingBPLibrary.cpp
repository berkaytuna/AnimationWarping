// Written by Berkay Tuna, August 2020

#include "AnimationWarpingBPLibrary.h"
#include "AnimationWarping.h"

UAnimationWarpingBPLibrary::UAnimationWarpingBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

float UAnimationWarpingBPLibrary::GetInputDirectionInDegrees(float ForwardInput, float RightInput)
{
	float InputDirection;

	if ((ForwardInput == 0.0f) && (RightInput == 0.0f)) {
	InputDirection = 0.0f;
	}
	else if ((ForwardInput > 0.0f) && (RightInput == 0.0f)) {
		InputDirection = 0.0f;
	}
	else if ((ForwardInput == 0.0f) && (RightInput > 0.0f)) {
		InputDirection = 90.0f;
	}
	else if ((ForwardInput < 0.0f) && (RightInput == 0.0f)) {
		InputDirection = 180.0f;
	}
	else if ((ForwardInput == 0.0f) && (RightInput < 0.0f)) {
		InputDirection = -90.0f;
	}
	else if ((ForwardInput > 0.0f) && (RightInput > 0.0f)) {
		InputDirection = (180.f) / PI * atan(RightInput / ForwardInput);
	}
	else if ((ForwardInput < 0.0f) && (RightInput > 0.0f)) {
		InputDirection = 90.0f - (180.f) / PI * atan(ForwardInput / RightInput);
	}
	else if ((ForwardInput < 0.0f) && (RightInput < 0.0f)) {
		InputDirection = (180.f) / PI * atan(RightInput / ForwardInput) - 180.0f;
	}
	else if ((ForwardInput > 0.0f) && (RightInput < 0.0f)) {
		InputDirection = (180.f) / PI * atan(RightInput / ForwardInput);
	}

	return InputDirection;
}
