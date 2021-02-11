// Written by Berkay Tuna, August 2020

#include "AnimGraphNode_SpeedWarping.h"
#include "AnimationWarpingEditor.h"
#include "AnimNodeEditModes.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_SpeedWarping::UAnimGraphNode_SpeedWarping()
{

}

FText UAnimGraphNode_SpeedWarping::GetTooltipText() const
{
    return LOCTEXT("SpeedWarping", "SpeedWarping");
}

FText UAnimGraphNode_SpeedWarping::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("SpeedWarping", "SpeedWarping");
}

FString UAnimGraphNode_SpeedWarping::GetNodeCategory() const
{
    return TEXT("SpeedWarping");
}

#undef LOCTEXT_NAMESPACE