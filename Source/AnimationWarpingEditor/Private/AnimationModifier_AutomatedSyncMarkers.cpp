// Written by Berkay Tuna, August 2020

#include "AnimationModifier_AutomatedSyncMarkers.h"
#include "AnimationWarping.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimCurveCompressionSettings.h"

UAnimationModifier_AutomatedSyncMarkers::UAnimationModifier_AutomatedSyncMarkers()
{
	
}

void UAnimationModifier_AutomatedSyncMarkers::AddSyncMarkers(UAnimSequence* AnimSequence, FName BoneName, FName MarkerName, FName NotifyTrackName)
{
	FTransform ShortestTM;
	UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimSequence, BoneName, 0, false, ShortestTM);
	float Shortest = ShortestTM.GetLocation().Z;

	int32 NumFrames;
	UAnimationBlueprintLibrary::GetNumFrames(AnimSequence, NumFrames);

	int32 FrameToPutSyncMarker;

	for (int i = 0; i < NumFrames; i++) {

		FTransform DistanceTM;
		UAnimationBlueprintLibrary::GetBonePoseForFrame(AnimSequence, BoneName, i, false, DistanceTM);
		float Distance = DistanceTM.GetLocation().Z;

		if (Distance < Shortest) {

			Shortest = Distance;
			FrameToPutSyncMarker = i;
		}
	}

	float Time;
	UAnimationBlueprintLibrary::GetTimeAtFrame(AnimSequence, FrameToPutSyncMarker, Time);

	UAnimationBlueprintLibrary::AddAnimationSyncMarker(AnimSequence, MarkerName, Time, NotifyTrackName);
}

void UAnimationModifier_AutomatedSyncMarkers::OnRevert_Implementation(UAnimSequence* AnimSequence)
{
	UAnimationBlueprintLibrary::RemoveAnimationSyncMarkersByName(AnimSequence, LeftFoot.MarkerName);
	UAnimationBlueprintLibrary::RemoveAnimationSyncMarkersByName(AnimSequence, RightFoot.MarkerName);
}

void UAnimationModifier_AutomatedSyncMarkers::OnApply_Implementation(UAnimSequence* AnimSequence)
{
	AddSyncMarkers(AnimSequence, LeftFoot.IKBone, LeftFoot.MarkerName, LeftFoot.NotifyTrackName);
	AddSyncMarkers(AnimSequence, RightFoot.IKBone, RightFoot.MarkerName, RightFoot.NotifyTrackName);

	UAnimationBlueprintLibrary::FinalizeBoneAnimation(AnimSequence);
}