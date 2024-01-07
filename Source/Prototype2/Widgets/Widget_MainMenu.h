

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MainMenu.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_MainMenu : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* HostButton;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* JoinButton;
	
	UPROPERTY(VisibleAnywhere)
	class UPrototypeGameInstance* GameInstance;
};
