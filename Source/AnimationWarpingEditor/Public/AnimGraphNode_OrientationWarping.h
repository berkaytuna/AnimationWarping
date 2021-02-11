// Written by Berkay Tuna, August 2020

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_OrientationWarping.h"
#include "AnimGraphNode_OrientationWarping.generated.h"

struct FAnimNode_OrientationWarping;
/**
 *
 */
UCLASS()
class ANIMATIONWARPINGEDITOR_API UAnimGraphNode_OrientationWarping : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, Category = "Settings")
    FAnimNode_OrientationWarping Node;

    //~ Begin UEdGraphNode Interface.
    virtual FText GetTooltipText() const override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    //~ End UEdGraphNode Interface.

    //~ Begin UAnimGraphNode_Base Interface
	virtual FString GetNodeCategory() const override;
    //~ End UAnimGraphNode_Base Interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface

    UAnimGraphNode_OrientationWarping();
};