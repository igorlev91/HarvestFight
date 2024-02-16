

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerEmote.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_PlayerEmote : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void SetPlayerRef(APlayerState* _Player);
	APlayerState* GetPlayerRef();

	UFUNCTION()
	void PlayEmote(EEmote _Emote);

	UFUNCTION(BlueprintNativeEvent)
	void OnPlayEmote();
	
	/* Public Variables */
public:
	class APrototype2Gamestate* GameStateReference;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerState* OwningPlayer{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	class UImage* EmoteImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Emote8;
};
