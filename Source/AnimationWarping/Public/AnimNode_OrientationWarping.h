// Written by Berkay Tuna, August 2020

#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_OrientationWarping.generated.h"

UENUM()
enum class EDirectionEnum : uint8
{
	Fwd,
	Left,
	Right,
	Bwd
};

USTRUCT(BlueprintInternalUseOnly)
struct ANIMATIONWARPING_API FAnimNode_OrientationWarping : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneReference Root;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control", meta = (PinHiddenByDefault))
	bool bUseCounterRotation;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control", meta = (editcondition = "bUseCounterRotation"))
	FBoneReference CounterRotationBone;	

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinShownByDefault))
	float Direction;
	float DirectionP;

	UPROPERTY(EditAnywhere, Category = "Calculations")
	bool bUseSingleDirection;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (editcondition = "bUseSingleDirection"))
	EDirectionEnum NodeDirection;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinHiddenByDefault, editcondition = "!bUseSingleDirection"))
	bool bApplyDirectionalConsistency;

	float PrevDirection;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinHiddenByDefault))
	float RotationInterpolationAlpha;

	UPROPERTY(EditAnywhere, Category = "Lean", meta = (PinHiddenByDefault))
	bool bApplyLean;
	float CurrentLean;
	UPROPERTY(EditAnywhere, Category = "Lean", meta = (DisplayName = "Lean Disable Interpolation Alpha"))
	float LeanDisableAlpha;

	UPROPERTY(EditAnywhere, Category = "Lean", meta = (PinHiddenByDefault))
	FRotator ControlRotation;
	FRotator PrevControlRot;

	float ControlRotDiff;

	UPROPERTY(EditAnywhere, Category = "Lean", meta = (editcondition = "bApplyLean"))
	float LeanInterpolationAlpha;

	UPROPERTY(EditAnywhere, Category = "Lean", meta = (editcondition = "bApplyLean"))
	float LeanMultiplier;

	UPROPERTY(EditAnywhere, Category = "Lean", meta = (editcondition = "bApplyLean"))
	float MaxLean;

	UPROPERTY(EditAnywhere, Category = "Lean", meta = (PinHiddenByDefault, editcondition = "bApplyLean"))
	bool bResetNode;

	float CounterYaw;

	bool bCanDoOnce;
	bool PrevbResetNode;

	FAnimNode_OrientationWarping();

	// FAnimNode_Base
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;

	// FAnimNode_SkeletalControlBase
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransfor) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;

private:

	// FAnimNode_SkeletalControlBase
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};