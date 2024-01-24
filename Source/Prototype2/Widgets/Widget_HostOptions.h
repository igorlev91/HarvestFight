

#pragma once

#include "CoreMinimal.h"
#include "WidgetUtility.h"
#include "Blueprint/UserWidget.h"
/*#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"*/
#include "Widget_HostOptions.generated.h"

/*USTRUCT(BlueprintType)
struct FIPAddressResponse
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString ip;
};*/

UCLASS()
class PROTOTYPE2_API UWidget_HostOptions : public UUserWidget, public IWidgetUtilityInterface
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnHostPressed();

	/*UFUNCTION(BlueprintCallable)
	void GetPublicIPAddress();
	void OnPublicIPAddressResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);*/

	UPROPERTY(BlueprintReadOnly)
	FString PublicIPAdress{};
	UPROPERTY(BlueprintReadOnly)
	FString PrivateIPAdress{};
	/* Public Variables */
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* IsTeamsButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* IsLanButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UEditableTextBox* PasswordInputField;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* StartButton;
};
