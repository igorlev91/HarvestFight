

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerName.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_PlayerName : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions*/
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdatePlayerName();
	void SetPlayerRef(APlayerState* _Player);
	APlayerState* GetPlayerRef();
	
	/* Public Variables*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* PlayerName; // Name for player
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerState* OwningPlayer{nullptr};
};
