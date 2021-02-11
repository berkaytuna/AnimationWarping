// Written by Berkay Tuna, August 2020

#include "AnimNode_OrientationWarping.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationRuntime.h"
//#include "DrawDebugHelpers.h"
//#include "Engine/Engine.h"

FAnimNode_OrientationWarping::FAnimNode_OrientationWarping() :
	RotationInterpolationAlpha(1.0f),
	bUseCounterRotation(true),
	LeanInterpolationAlpha(0.01f),
	LeanMultiplier(10.0f),
	MaxLean(6.0f),
	LeanDisableAlpha(0.1f)
{
}

void FAnimNode_OrientationWarping::GatherDebugData(FNodeDebugData& DebugData)
{
	FString DebugLine = DebugData.GetNodeName(this);
	DebugData.AddDebugItem(DebugLine);
	ComponentPose.GatherDebugData(DebugData);
}

bool FAnimNode_OrientationWarping::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	if (!Root.IsValidToEvaluate(RequiredBones)) 
	{
		return false;
	}

	else if (bUseCounterRotation && !CounterRotationBone.IsValidToEvaluate(RequiredBones))
	{
		return false;
	}

	else 
	{
		return true;
	}
}

void FAnimNode_OrientationWarping::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	Root.Initialize(RequiredBones);

	if (bUseCounterRotation)
	{
		CounterRotationBone.Initialize(RequiredBones);
	}
}

void FAnimNode_OrientationWarping::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{

	// Note: if i directly get the Output as FComponentSpacePoseContext without prior conversion to FPoseContext
	// (i.e. from another Skeletal Control Node), i lose the functionality of this node. Just Evaluate Output 
	// won`t work. That is why i am converting to Local Pose and then again back to Component Space Pose.
	// Notice that in this node i just used Output and not OutBoneTransforms when manupulating Bones.

	ComponentPose.EvaluateComponentSpace(Output);

	FPoseContext LocalOutput(Output.AnimInstanceProxy);

	FCSPose<FCompactPose>::ConvertComponentPosesToLocalPoses(MoveTemp(Output.Pose), LocalOutput.Pose);
	LocalOutput.Curve = MoveTemp(Output.Curve);

	Output = LocalOutput.AnimInstanceProxy;

	Output.Pose.InitPose(MoveTemp(LocalOutput.Pose));
	Output.Curve = MoveTemp(LocalOutput.Curve);



	if (bUseSingleDirection)
	{
		if (NodeDirection == EDirectionEnum::Fwd)
		{
			DirectionP = (Direction - DirectionP) * RotationInterpolationAlpha + DirectionP;
		}
		else if (NodeDirection == EDirectionEnum::Left)
		{
			DirectionP = (Direction + 90.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
		}
		else if (NodeDirection == EDirectionEnum::Right)
		{
			DirectionP = (Direction - 90.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
		}
		else if (NodeDirection == EDirectionEnum::Bwd)
		{
			if (Direction > 0.0f) {
				DirectionP = (Direction - 180.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
			}
			else {
				DirectionP = (Direction + 180.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
			}
		}
	}
	else if (bApplyDirectionalConsistency) 
	{
		if (abs(Direction) <= 45.0f) {
			DirectionP = (Direction - DirectionP) * RotationInterpolationAlpha + DirectionP;
		}
		else if ((Direction < -45.0f) && (Direction > -135.0f)) {
			DirectionP = (Direction + 90.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
		}
		else if ((Direction > 45.0f) && (Direction < 135.0f)) {
			DirectionP = (Direction - 90.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
		}
		else if (abs(Direction) >= 135.0f) {
			if (Direction > 0.0f) {
				DirectionP = (Direction - 180.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
			}
			else {
				DirectionP = (Direction + 180.0f - DirectionP) * RotationInterpolationAlpha + DirectionP;
			}
		}
	}
	else
	{
		if (abs(Direction - DirectionP) > 180.0f)
		{
			if (Direction > DirectionP /* DirectionP < 0.0f && Direction > 0.0f */ )
			{
				DirectionP += 360.0f;
			}
			else // if (DirectionP > 0.0f && Direction < 0.0f)
			{
				DirectionP -= 360.0f;
			}
		}

		DirectionP = (Direction - DirectionP) * RotationInterpolationAlpha + DirectionP;
	}



	FRotator NewRootRot = FRotator(0.0f, DirectionP, 0.0f);

	float NewCounterYaw;

	if (bUseCounterRotation)
	{
		NewCounterYaw = -DirectionP;
	}
	else
	{
		NewCounterYaw = 0.0f;
	}

	CounterYaw = (NewCounterYaw - CounterYaw) * RotationInterpolationAlpha + CounterYaw;
	FRotator NewCounterRotBoneRot = FRotator(0.0f, CounterYaw, 0.0f);



	if (bApplyLean)
	{
		float ControlYawDiff = ControlRotation.Yaw - PrevControlRot.Yaw;

		if (ControlYawDiff < -180.0f)
		{
			PrevControlRot.Yaw = PrevControlRot.Yaw - 360.0f;
		}
		else if (ControlYawDiff > 180.0f)
		{
			PrevControlRot.Yaw = PrevControlRot.Yaw + 360.0f;
		}

		ControlYawDiff = ControlRotation.Yaw - PrevControlRot.Yaw;
		ControlRotDiff = (ControlYawDiff - ControlRotDiff) * LeanInterpolationAlpha + ControlRotDiff;

		if (bResetNode && !PrevbResetNode) {

			bCanDoOnce = true;
		}

		PrevbResetNode = bResetNode;

		if (bCanDoOnce) {

			ControlRotDiff = 0.0f;

			bCanDoOnce = false;
		}

		PrevControlRot = ControlRotation;

		float TotalLean = ControlRotDiff * LeanMultiplier;

		if (ControlRotDiff > 0.0f)
		{
			TotalLean = TotalLean > MaxLean ? MaxLean : TotalLean;
		}
		else if (ControlRotDiff < 0.0f)
		{
			TotalLean = TotalLean < -MaxLean ? -MaxLean : TotalLean;
		}
		
		CurrentLean = (TotalLean - CurrentLean) * LeanDisableAlpha + CurrentLean;
	}
	else
	{
		CurrentLean = (0.0f - CurrentLean) * LeanDisableAlpha + CurrentLean;
	}

	NewRootRot = FRotator(CurrentLean, DirectionP, 0.0f);




	const FBoneContainer& BoneContainer = Output.AnimInstanceProxy->GetRequiredBones();

	FCompactPoseBoneIndex RootIndex = Root.GetCompactPoseIndex(BoneContainer);
	FTransform NewRootTM = Output.Pose.GetComponentSpaceTransform(RootIndex);

	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewRootTM, RootIndex, BCS_ComponentSpace);

	const FQuat NewRootQuat(NewRootRot);
	NewRootTM.SetRotation(NewRootQuat * NewRootTM.GetRotation());

	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewRootTM, RootIndex, BCS_ComponentSpace);

	Output.Pose.SetComponentSpaceTransform(RootIndex, NewRootTM);




	if (CounterYaw != 0)
	{
		FCompactPoseBoneIndex CounterRotBoneIndex = CounterRotationBone.GetCompactPoseIndex(BoneContainer);
		FTransform NewCounterRotBoneTM = Output.Pose.GetComponentSpaceTransform(CounterRotBoneIndex);

		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewCounterRotBoneTM, CounterRotBoneIndex, BCS_ComponentSpace);

		const FQuat NewCounterRotBoneQuat(NewCounterRotBoneRot);
		NewCounterRotBoneTM.SetRotation(NewCounterRotBoneQuat * NewCounterRotBoneTM.GetRotation());

		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewCounterRotBoneTM, CounterRotBoneIndex, BCS_ComponentSpace);

		Output.Pose.SetComponentSpaceTransform(CounterRotBoneIndex, NewCounterRotBoneTM);
	}

}