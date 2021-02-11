// Written by Berkay Tuna, August 2020

#pragma once

#include "AnimationModifier.h"
#include "AnimationBlueprintLibrary.h"
#include "AnimationModifier_AutomatedSyncMarkers.generated.h"

USTRUCT()
struct FSyncMarkerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Sync Marker Data")
	FName IKBone;
	UPROPERTY(EditAnywhere, Category = "Sync Marker Data")
	FName MarkerName;
	UPROPERTY(EditAnywhere, Category = "Sync Marker Data")
	FName NotifyTrackName;
};

UCLASS(Blueprintable, Experimental, config = Editor, defaultconfig, DisplayName = "Automated Sync Markers")
class ANIMATIONWARPINGEDITOR_API UAnimationModifier_AutomatedSyncMarkers : public UAnimationModifier
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Left Foot")
	FSyncMarkerData LeftFoot;
	UPROPERTY(EditAnywhere, Category = "Right Foot")
	FSyncMarkerData RightFoot;

	UAnimationModifier_AutomatedSyncMarkers();

	virtual void OnApply_Implementation(UAnimSequence* AnimSequence) override;
	virtual void OnRevert_Implementation(UAnimSequence* AnimSequence) override;

private:

	void AddSyncMarkers(UAnimSequence* AnimSequence, FName BoneName, FName MarkerName, FName NotifyTrackName);
};