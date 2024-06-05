

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetUtility.h"
#include "Widget_MainMenu.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_MainMenu : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	
	void RemoveLoadingScreen();
	void ShowLoadingScreen();

	/* Public Variables */
public:
	/* Reference to game instance */
	UPROPERTY(VisibleAnywhere)
	class UPrototypeGameInstance* GameInstanceReference;

	/* Buttons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Host Button", meta = (BindWidget))
	class UButton* HostButton;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* JoinButton;

	/* Widget Variables related to button pulsing/size changes */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	//bool bIsButtonHovered;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	//int32 ButtonIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	FVector2D ButtonScale;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	//class UButton* ButtonToPulse;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Variables")
	//float PulseTime;


};
