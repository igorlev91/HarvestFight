

#pragma once

#include "CoreMinimal.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Tutorial.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_Tutorial : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/* Public Variables */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TutorialPage{}; // Current page number for tutorial

	/* Widget Variables related to button pulsing/size changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	bool bIsButtonHovered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	int32 ButtonIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	FVector2D ButtonScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	class UButton* ButtonToPulse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	float PulseTime;
};
