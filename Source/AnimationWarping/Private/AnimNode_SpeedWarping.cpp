// Written by Berkay Tuna, August 2020

#include "AnimNode_SpeedWarping.h"
#include "Animation/AnimInstanceProxy.h"
#include "DrawDebugHelpers.h"
#include "TwoBoneIK.h"
#include "AnimationRuntime.h"

FAnimNode_SpeedWarping::FAnimNode_SpeedWarping() :
	MaxStretchLength(80.0f),
	BaseSpeed(300.0f),
	MinBaseSpeed(25.0f),
	Stiffness(3.0f),
	AlphaXY(1.0f, 1.0f),
	CachedLowerLimbIndex_L(INDEX_NONE),
	CachedUpperLimbIndex_L(INDEX_NONE),
	LeftJointTargetLocation(FVector::ZeroVector),
	CachedLowerLimbIndex_R(INDEX_NONE),
	CachedUpperLimbIndex_R(INDEX_NONE),
	RightJointTargetLocation(FVector::ZeroVector),

	InterpolationAlpha(0.01f)

{
}

void FAnimNode_SpeedWarping::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_SkeletalControlBase::Initialize_AnyThread(Context);

	IKFoot.Root.Initialize(Context.AnimInstanceProxy);

	LeftFoot.Initialize(Context.AnimInstanceProxy);
	IKFoot.Left.Initialize(Context.AnimInstanceProxy);
	LeftJointTarget.Initialize(Context.AnimInstanceProxy);

	RightFoot.Initialize(Context.AnimInstanceProxy);
	IKFoot.Right.Initialize(Context.AnimInstanceProxy);
	RightJointTarget.Initialize(Context.AnimInstanceProxy);

	Mesh = Context.AnimInstanceProxy->GetSkelMeshComponent();
	Actor = Mesh->GetOwner();

}

void FAnimNode_SpeedWarping::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	FAnimNode_SkeletalControlBase::CacheBones_AnyThread(Context);


}

bool FAnimNode_SpeedWarping::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	if (!Pelvis.IsValidToEvaluate(RequiredBones) & !IKFoot.Root.IsValidToEvaluate(RequiredBones) & !IKFoot.Left.IsValidToEvaluate(RequiredBones) & !IKFoot.Right.IsValidToEvaluate(RequiredBones)) {
		return false;
	}

	if (CachedUpperLimbIndex_L == INDEX_NONE || CachedLowerLimbIndex_L == INDEX_NONE || CachedUpperLimbIndex_R == INDEX_NONE || CachedLowerLimbIndex_R == INDEX_NONE)
	{
		return false;
	}


	if (!LeftFoot.IsValidToEvaluate(RequiredBones) || !RightFoot.IsValidToEvaluate(RequiredBones))
	{
		return false;
	}


	if (!LeftJointTarget.IsValidToEvaluate(RequiredBones) && !RightJointTarget.IsValidToEvaluate(RequiredBones))
	{
		return false;
	}


	else {
		return true;
	}
}

void FAnimNode_SpeedWarping::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	Pelvis.Initialize(RequiredBones);
	IKFoot.Root.BoneReference.Initialize(RequiredBones);
	IKFoot.Right.BoneReference.Initialize(RequiredBones);
	IKFoot.Left.BoneReference.Initialize(RequiredBones);

	LeftFoot.BoneReference.Initialize(RequiredBones);
	RightFoot.BoneReference.Initialize(RequiredBones);
	LeftJointTarget.BoneReference.Initialize(RequiredBones);
	RightJointTarget.BoneReference.Initialize(RequiredBones);

	FCompactPoseBoneIndex IKBoneCompactPoseIndex_L = LeftFoot.BoneReference.GetCompactPoseIndex(RequiredBones);
	CachedLowerLimbIndex_L = FCompactPoseBoneIndex(INDEX_NONE);
	CachedUpperLimbIndex_L = FCompactPoseBoneIndex(INDEX_NONE);
	if (IKBoneCompactPoseIndex_L != INDEX_NONE)
	{
		CachedLowerLimbIndex_L = RequiredBones.GetParentBoneIndex(IKBoneCompactPoseIndex_L);
		if (CachedLowerLimbIndex_L != INDEX_NONE)
		{
			CachedUpperLimbIndex_L = RequiredBones.GetParentBoneIndex(CachedLowerLimbIndex_L);
		}
	}

	FCompactPoseBoneIndex IKBoneCompactPoseIndex_R = RightFoot.BoneReference.GetCompactPoseIndex(RequiredBones);
	CachedLowerLimbIndex_R = FCompactPoseBoneIndex(INDEX_NONE);
	CachedUpperLimbIndex_R = FCompactPoseBoneIndex(INDEX_NONE);
	if (IKBoneCompactPoseIndex_R != INDEX_NONE)
	{
		CachedLowerLimbIndex_R = RequiredBones.GetParentBoneIndex(IKBoneCompactPoseIndex_R);
		if (CachedLowerLimbIndex_R != INDEX_NONE)
		{
			CachedUpperLimbIndex_R = RequiredBones.GetParentBoneIndex(CachedLowerLimbIndex_R);
		}
	}
}

void FAnimNode_SpeedWarping::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	BaseSpeed = BaseSpeed < MinBaseSpeed ? MinBaseSpeed : BaseSpeed;

	UWorld* World = Output.AnimInstanceProxy->GetAnimInstanceObject()->GetWorld();
	FBoneContainer BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	float NewSpeed = Actor->GetVelocity().Size();

	Speed = (NewSpeed - Speed) * InterpolationAlpha + Speed;

	FCompactPoseBoneIndex IKFootRootIndex = IKFoot.Root.BoneReference.GetCompactPoseIndex(BoneContainer);
	FTransform IKFootRootCSTM = Output.Pose.GetComponentSpaceTransform(IKFootRootIndex);
	FVector IKFootRootPos = IKFootRootCSTM.GetTranslation();

	FCompactPoseBoneIndex IKFootLeftIndex = IKFoot.Left.BoneReference.GetCompactPoseIndex(BoneContainer);
	FTransform IKFootLeftCSTM = Output.Pose.GetComponentSpaceTransform(IKFootLeftIndex);
	FVector IKFootLeftPos = IKFootLeftCSTM.GetTranslation();

	FCompactPoseBoneIndex IKFootRightIndex = IKFoot.Right.BoneReference.GetCompactPoseIndex(BoneContainer);
	FTransform IKFootRightCSTM = Output.Pose.GetComponentSpaceTransform(IKFootRightIndex);
	FVector IKFootRightPos = IKFootRightCSTM.GetTranslation();

	FCompactPoseBoneIndex PelvisIndex = Pelvis.GetCompactPoseIndex(BoneContainer);
	FTransform PelvisCSTM = Output.Pose.GetComponentSpaceTransform(PelvisIndex);
	FVector PelvisPos = PelvisCSTM.GetTranslation();

	FVector ScalePivotLeft = FVector(IKFootRootPos.X, IKFootRootPos.Y, IKFootLeftPos.Z);
	FVector ScalePivotRight = FVector(IKFootRootPos.X, IKFootRootPos.Y, IKFootRightPos.Z);

	float SpeedScaling = Speed / BaseSpeed;

	FVector NewIKFootLeftPos;
	FVector NewIKFootRightPos;

	if (SpeedScalingForDebug != 0.0f) {
		NewIKFootLeftPos = (IKFootLeftPos - ScalePivotLeft) * SpeedScalingForDebug + ScalePivotLeft;
		NewIKFootRightPos = (IKFootRightPos - ScalePivotRight) * SpeedScalingForDebug + ScalePivotRight;
	}
	else {
		NewIKFootLeftPos = (IKFootLeftPos - ScalePivotLeft) * SpeedScaling + ScalePivotLeft;
		NewIKFootRightPos = (IKFootRightPos - ScalePivotRight) * SpeedScaling + ScalePivotRight;
	}

	NewIKFootLeftPos.X = (NewIKFootLeftPos.X - IKFootLeftPos.X) * AlphaXY.X + IKFootLeftPos.X;
	NewIKFootLeftPos.Y = (NewIKFootLeftPos.Y - IKFootLeftPos.Y) * AlphaXY.Y + IKFootLeftPos.Y;

	NewIKFootRightPos.X = (NewIKFootRightPos.X - IKFootRightPos.X) * AlphaXY.X + IKFootRightPos.X;
	NewIKFootRightPos.Y = (NewIKFootRightPos.Y - IKFootRightPos.Y) * AlphaXY.Y + IKFootRightPos.Y;

	FVector PelvisFootLeftDif = NewIKFootLeftPos - PelvisPos;
	float PelvisFootLeftLength = pow((pow(PelvisFootLeftDif.X, 2) + pow(PelvisFootLeftDif.Y, 2) + pow(PelvisFootLeftDif.Z, 2)), 0.5);
	float DivisionCoefficientLeft = MaxStretchLength / PelvisFootLeftLength;
	DivisionCoefficientLeft = DivisionCoefficientLeft > 1.0 ? 1.0 : DivisionCoefficientLeft;
	FVector ResultLeftPos = DivisionCoefficientLeft * (PelvisFootLeftDif)+PelvisPos;

	FVector PelvisFootRightDif = NewIKFootRightPos - PelvisPos;
	float PelvisFootRightLength = pow((pow(PelvisFootRightDif.X, 2) + pow(PelvisFootRightDif.Y, 2) + pow(PelvisFootRightDif.Z, 2)), 0.5);
	float DivisionCoefficientRight = MaxStretchLength / PelvisFootRightLength;
	DivisionCoefficientRight = DivisionCoefficientRight > 1.0 ? 1.0 : DivisionCoefficientRight;
	FVector ResultRightPos = DivisionCoefficientRight * (PelvisFootRightDif)+PelvisPos;

	FVector FootLeftToPelvis = IKFootLeftPos - PelvisPos;
	float FootLeftToPelvisLength = pow((pow(FootLeftToPelvis.X, 2) + pow(FootLeftToPelvis.Y, 2) + pow(FootLeftToPelvis.Z, 2)), 0.5);
	FVector NewFootLeftToPelvis = NewIKFootLeftPos - PelvisPos;
	float NewFootLeftToPelvisLength = pow((pow(NewFootLeftToPelvis.X, 2) + pow(NewFootLeftToPelvis.Y, 2) + pow(NewFootLeftToPelvis.Z, 2)), 0.5);
	FVector NewFootLeftToPelvisNorm = NewFootLeftToPelvis / NewFootLeftToPelvisLength;
	float FootLeftLengthDelta = NewFootLeftToPelvisLength - FootLeftToPelvisLength;
	FVector NewPelvisPosByLeft = FootLeftLengthDelta * NewFootLeftToPelvisNorm;

	FVector FootRightToPelvis = IKFootRightPos - PelvisPos;
	float FootRightToPelvisLength = pow((pow(FootRightToPelvis.X, 2) + pow(FootRightToPelvis.Y, 2) + pow(FootRightToPelvis.Z, 2)), 0.5);
	FVector NewFootRightToPelvis = NewIKFootRightPos - PelvisPos;
	float NewFootRightToPelvisLength = pow((pow(NewFootRightToPelvis.X, 2) + pow(NewFootRightToPelvis.Y, 2) + pow(NewFootRightToPelvis.Z, 2)), 0.5);
	FVector NewFootRightToPelvisNorm = NewFootRightToPelvis / NewFootRightToPelvisLength;
	float FootRightLengthDelta = NewFootRightToPelvisLength - FootRightToPelvisLength;
	FVector NewPelvisPosByRight = FootRightLengthDelta * NewFootRightToPelvisNorm;

	FVector NewPelvisPos = ((NewPelvisPosByLeft + NewPelvisPosByRight) / Stiffness) + PelvisPos;

	IKFootLeftCSTM.SetTranslation(ResultLeftPos);
	IKFootRightCSTM.SetTranslation(ResultRightPos);

	PelvisCSTM.SetTranslation(NewPelvisPos);

	if (SpeedScalingForDebug != 0.0f) {
		DrawDebugSphere(World, IKFootLeftPos, 12, 8, FColor::Red, false, 0.0f, 0, 0);
		DrawDebugSphere(World, IKFootRightPos, 12, 8, FColor::Red, false, 0.0f, 0, 0);

		DrawDebugSphere(World, ScalePivotLeft, 12, 8, FColor::Yellow, false, 0.0f, 0, 0);
		DrawDebugSphere(World, ScalePivotRight, 12, 8, FColor::Yellow, false, 0.0f, 0, 0);

		DrawDebugSphere(World, NewIKFootLeftPos, 12, 8, FColor::Green, false, 0.0f, 0, 0);
		DrawDebugSphere(World, NewIKFootRightPos, 12, 8, FColor::Green, false, 0.0f, 0, 0);

		DrawDebugSphere(World, ResultLeftPos, 12, 8, FColor::Blue, false, 0.0f, 0, 0);
		DrawDebugSphere(World, ResultRightPos, 12, 8, FColor::Blue, false, 0.0f, 0, 0);

		DrawDebugSphere(World, PelvisPos, 12, 8, FColor::Red, false, 0.0f, 0, 0);
		DrawDebugSphere(World, NewPelvisPos, 12, 8, FColor::Green, false, 0.0f, 0, 0);
	}

	FCompactPoseBoneIndex IKBoneCompactPoseIndex_L = LeftFoot.BoneReference.GetCompactPoseIndex(BoneContainer);
	FCompactPoseBoneIndex IKBoneCompactPoseIndex_R = RightFoot.BoneReference.GetCompactPoseIndex(BoneContainer);

	FTransform LowerLimbCSTransform_L = Output.Pose.GetComponentSpaceTransform(CachedLowerLimbIndex_L);
	FTransform UpperLimbCSTransform_L = Output.Pose.GetComponentSpaceTransform(CachedUpperLimbIndex_L);
	FTransform EndBoneCSTransform_L = Output.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex_L);

	FTransform LowerLimbCSTransform_R = Output.Pose.GetComponentSpaceTransform(CachedLowerLimbIndex_R);
	FTransform UpperLimbCSTransform_R = Output.Pose.GetComponentSpaceTransform(CachedUpperLimbIndex_R);
	FTransform EndBoneCSTransform_R = Output.Pose.GetComponentSpaceTransform(IKBoneCompactPoseIndex_R);

	const FVector RootPos_L = UpperLimbCSTransform_L.GetTranslation();
	const FVector InitialJointPos_L = LowerLimbCSTransform_L.GetTranslation();
	const FVector InitialEndPos_L = EndBoneCSTransform_L.GetTranslation();

	const FVector RootPos_R = UpperLimbCSTransform_R.GetTranslation();
	const FVector InitialJointPos_R = LowerLimbCSTransform_R.GetTranslation();
	const FVector InitialEndPos_R = EndBoneCSTransform_R.GetTranslation();

	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, IKFootLeftCSTM, LeftFoot.BoneReference.GetCompactPoseIndex(BoneContainer), BCS_BoneSpace);
	FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, IKFootRightCSTM, RightFoot.BoneReference.GetCompactPoseIndex(BoneContainer), BCS_BoneSpace);

	FTransform EffectorTransform_L = IKFoot.Left.GetTargetTransform(IKFootLeftCSTM.GetLocation(), Output.Pose, Output.AnimInstanceProxy->GetComponentTransform());
	FTransform JointTargetTransform_L = LeftJointTarget.GetTargetTransform(LeftJointTargetLocation, Output.Pose, Output.AnimInstanceProxy->GetComponentTransform());

	FTransform EffectorTransform_R = IKFoot.Right.GetTargetTransform(IKFootRightCSTM.GetLocation(), Output.Pose, Output.AnimInstanceProxy->GetComponentTransform());
	FTransform JointTargetTransform_R = RightJointTarget.GetTargetTransform(RightJointTargetLocation, Output.Pose, Output.AnimInstanceProxy->GetComponentTransform());

	FVector	JointTargetPos_L = JointTargetTransform_L.GetTranslation();
	FVector	JointTargetPos_R = JointTargetTransform_R.GetTranslation();

	FVector DesiredPos_L = EffectorTransform_L.GetTranslation();
	FVector DesiredPos_R = EffectorTransform_R.GetTranslation();

	UpperLimbCSTransform_L.SetLocation(RootPos_L + (NewPelvisPos - PelvisPos));
	LowerLimbCSTransform_L.SetLocation(InitialJointPos_L + (NewPelvisPos - PelvisPos));
	EndBoneCSTransform_L.SetLocation(InitialEndPos_L + (NewPelvisPos - PelvisPos));

	UpperLimbCSTransform_R.SetLocation(RootPos_R + (NewPelvisPos - PelvisPos));
	LowerLimbCSTransform_R.SetLocation(InitialJointPos_R + (NewPelvisPos - PelvisPos));
	EndBoneCSTransform_R.SetLocation(InitialEndPos_R + (NewPelvisPos - PelvisPos));

	AnimationCore::SolveTwoBoneIK(UpperLimbCSTransform_L, LowerLimbCSTransform_L, EndBoneCSTransform_L, JointTargetPos_L, DesiredPos_L, false, 1.0f, 1.2f);
	AnimationCore::SolveTwoBoneIK(UpperLimbCSTransform_R, LowerLimbCSTransform_R, EndBoneCSTransform_R, JointTargetPos_R, DesiredPos_R, false, 1.0f, 1.2f);

	OutBoneTransforms.Add(FBoneTransform(PelvisIndex, PelvisCSTM));

	OutBoneTransforms.Add(FBoneTransform(CachedUpperLimbIndex_L, UpperLimbCSTransform_L));
	OutBoneTransforms.Add(FBoneTransform(CachedLowerLimbIndex_L, LowerLimbCSTransform_L));
	EndBoneCSTransform_L.SetRotation(EffectorTransform_L.GetRotation());
	OutBoneTransforms.Add(FBoneTransform(IKBoneCompactPoseIndex_L, EndBoneCSTransform_L));

	OutBoneTransforms.Add(FBoneTransform(CachedUpperLimbIndex_R, UpperLimbCSTransform_R));
	OutBoneTransforms.Add(FBoneTransform(CachedLowerLimbIndex_R, LowerLimbCSTransform_R));
	EndBoneCSTransform_R.SetRotation(EffectorTransform_R.GetRotation());
	OutBoneTransforms.Add(FBoneTransform(IKBoneCompactPoseIndex_R, EndBoneCSTransform_R));

	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, IKFootLeftCSTM, LeftFoot.BoneReference.GetCompactPoseIndex(BoneContainer), BCS_BoneSpace);
	OutBoneTransforms.Add(FBoneTransform(IKFootLeftIndex, IKFootLeftCSTM));

	FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, IKFootRightCSTM, RightFoot.BoneReference.GetCompactPoseIndex(BoneContainer), BCS_BoneSpace);
	OutBoneTransforms.Add(FBoneTransform(IKFootRightIndex, IKFootRightCSTM));
}