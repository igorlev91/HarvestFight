

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Tutorial.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_Tutorial : public UUserWidget
{
	GENERATED_BODY()

	/* Public Variables */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TutorialPage{}; // Current page number for tutorial
};
