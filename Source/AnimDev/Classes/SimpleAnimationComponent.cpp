#include "SimpleAnimationComponent.h"
#include <Animation/AnimCompressionTypes.h>
#include <AnimationUtils.h>
DECLARE_CYCLE_STAT(TEXT("SimpleAnimationComponent::Tick"), STAT_SimpleAnimTick, STATGROUP_Anim);

USimpleAnimationComponent::USimpleAnimationComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USimpleAnimationComponent::BeginPlay()
{
    Super::BeginPlay();
    if(!Animation) {
        SetComponentTickEnabled(false);
        return;
    }

    if(!Target) {
        Target = GetOwner()->GetRootComponent();
    }
    if(nullptr != Animation->CompressedData.BoneCompressionCodec) {
        SetComponentTickEnabled(true);
        BaseTransform = Target->GetRelativeTransform();
    }
#if WITH_EDITOR
    else if(nullptr != Animation->GetDataModel()) {
        SetComponentTickEnabled(true);
        BaseTransform = Target->GetRelativeTransform();
    }
#endif
    else {
        SetComponentTickEnabled(false);
    }
}

void USimpleAnimationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    SCOPE_CYCLE_COUNTER(STAT_SimpleAnimTick);
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    CurrentTime += DeltaTime * PlayRate;
    while(Animation->GetPlayLength() < CurrentTime) {
        CurrentTime -= Animation->GetPlayLength();
    }
#if WITH_EDITOR
    if(nullptr != Animation->CompressedData.BoneCompressionCodec) {
        ExtractCompressed();
    } else {
        ExtractRaw();
    }
#else
    ExtractCompressed();
#endif
}

void USimpleAnimationComponent::SetTransform(const FTransform& InTransform)
{
    FVector Translation = BaseTransform.GetTranslation() + InTransform.GetTranslation();
    FQuat Rotation = BaseTransform.GetRotation() * InTransform.GetRotation();
    FVector Scale = BaseTransform.GetScale3D() * InTransform.GetScale3D();
    Target->SetRelativeLocationAndRotation(Translation, Rotation, false, nullptr, ETeleportType::None);
    Target->SetRelativeScale3D(Scale);
}

void USimpleAnimationComponent::ExtractCompressed()
{
    FBoneContainer BoneContainer;
    FCompactPose CompactPose;
    CompactPose.InitBones(1);
    FTransform Transform;
    Animation->GetBoneTransform(Transform, 0, CurrentTime, false);
    SetTransform(Transform);
}

#if WITH_EDITOR
void USimpleAnimationComponent::ExtractRaw()
{
    UAnimDataModel* AnimDataModel = Animation->GetDataModel();
    const FBoneAnimationTrack& BoneAnimationTrack = AnimDataModel->GetBoneTrackByIndex(0);
    FTransform Transform;
    FAnimationUtils::ExtractTransformFromTrack(CurrentTime, AnimDataModel->GetNumberOfFrames(), Animation->GetPlayLength(), BoneAnimationTrack.InternalTrackData, EAnimInterpolationType::Linear, Transform);
    SetTransform(Transform);
}
#endif
