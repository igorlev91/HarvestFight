#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MainMenu.generated.h"


UCLASS()
class PROTOTYPE2_API UWidget_MainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Host;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* Join;

	UPROPERTY(VisibleAnywhere)
	class UPrototypeGameInstance* GameInstance;
};
