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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Host;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Join;

	//UPROPERTY(EditAnywhere, meta = (BindWidget))
	//class UCheckBox* LanCheck;
	
	UPROPERTY(VisibleAnywhere)
	class UPrototypeGameInstance* GameInstance;
};
