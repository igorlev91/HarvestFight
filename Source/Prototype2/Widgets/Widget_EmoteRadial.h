

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Widget_EmoteRadial.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_EmoteRadial : public UUserWidget
{
	GENERATED_BODY()
	
	/* Public Functions */
public:
	virtual void NativeOnInitialized() override;

	/* Public Variables */
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APrototype2Gamestate* GameStateReference;
};
