

#pragma once

UENUM(BlueprintType)
enum class EEmote : uint8
{
	Emote1,
	Emote2,
	Emote3,
	Emote4,
	Emote5,
	Emote6,
	Emote7,
	Emote8
};

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerEmote.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_PlayerEmote : public UUserWidget
{
	GENERATED_BODY()

	/* Public Functions */
public:
	void SetPlayerRef(APlayerState* _Player);
	APlayerState* GetPlayerRef();
	
	/* Public Variables */
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerState* OwningPlayer{nullptr};
	
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
