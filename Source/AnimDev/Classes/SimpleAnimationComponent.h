#pragma once
/**
 */

#include <Animation/AnimSequence.h>

#include "SimpleAnimationComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Animation, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class ANIMDEV_API USimpleAnimationComponent: public UActorComponent
{
    GENERATED_BODY()
public:
    USimpleAnimationComponent(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TObjectPtr<UAnimSequence> Animation = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TObjectPtr<USceneComponent> Target = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float PlayRate = 1.0f;

private:
    void SetTransform(const FTransform& Transform);
    void ExtractCompressed();
#if WITH_EDITOR
    void ExtractRaw();
#endif
    float CurrentTime = 0.0f;
    FTransform BaseTransform = FTransform::Identity;
};
