// Written by Berkay Tuna, August 2020

#include "AnimGraphNode_OrientationWarping.h"
#include "AnimationWarpingEditor.h"
#include "AnimNodeEditModes.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_OrientationWarping::UAnimGraphNode_OrientationWarping()
{

}

FText UAnimGraphNode_OrientationWarping::GetTooltipText() const
{
    return LOCTEXT("OrientationWarping", "OrientationWarping");
}

FText UAnimGraphNode_OrientationWarping::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("OrientationWarping", "OrientationWarping");
}

FString UAnimGraphNode_OrientationWarping::GetNodeCategory() const
{
    return TEXT("OrientationWarping");
}

#undef LOCTEXT_NAMESPACE