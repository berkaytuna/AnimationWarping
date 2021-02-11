// Written by Berkay Tuna, August 2020

#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_SpeedWarping.generated.h"

class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FIKFoot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneSocketTarget Root;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneSocketTarget Left;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneSocketTarget Right;
};

USTRUCT(BlueprintInternalUseOnly)
struct ANIMATIONWARPING_API FAnimNode_SpeedWarping : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY()

	FCompactPoseBoneIndex CachedLowerLimbIndex_L;
	FCompactPoseBoneIndex CachedLowerLimbIndex_R;

	FCompactPoseBoneIndex CachedUpperLimbIndex_L;
	FCompactPoseBoneIndex CachedUpperLimbIndex_R;

	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneReference Pelvis;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FIKFoot IKFoot;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneSocketTarget LeftFoot;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneSocketTarget RightFoot;

	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneSocketTarget LeftJointTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skeletal Control", meta = (PinHiddenByDefault))
	FVector LeftJointTargetLocation;
	UPROPERTY(EditAnywhere, Category = "Skeletal Control")
	FBoneSocketTarget RightJointTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skeletal Control", meta = (PinHiddenByDefault))
	FVector RightJointTargetLocation;

	float Speed;
	UPROPERTY(EditAnywhere, Category = "Calculations")
	float InterpolationAlpha;

	AActor* Actor;
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinShownByDefault))
	float BaseSpeed;
	UPROPERTY(EditAnywhere, Category = "Calculations")
	float MinBaseSpeed;

	UPROPERTY(EditAnywhere, Category = "Calculations")
	float SpeedScalingForDebug;

	UPROPERTY(EditAnywhere, Category = "Calculations")
	float MaxStretchLength;

	UPROPERTY(EditAnywhere, Category = "Calculations")
	float Stiffness;

	UPROPERTY(EditAnywhere, Category = "Calculations", meta = (PinShownByDefault))
	FVector2D AlphaXY;

	FAnimNode_SpeedWarping();

	// FAnimNode_Base
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;

	// FAnimNode_SkeletalControlBase
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;

private:

	// FAnimNode_SkeletalControlBase
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};