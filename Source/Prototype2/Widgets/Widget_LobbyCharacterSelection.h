// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/PrototypeGameInstance.h"
#include "Math/Vector.h"
#include "Widget_LobbyCharacterSelection.generated.h"

/**
 * 
 */

/*UENUM(BlueprintType)
enum class ECharacters : uint8
{
	COW,
	PIG,
	CHICKEN,
	DUCK
};

UENUM(BlueprintType)
enum class ECharacterColours : uint8
{
	RED,
	BLUE,
	GREEN,
	YELLOW
};*/

UCLASS()
class PROTOTYPE2_API UWidget_LobbyCharacterSelection : public UUserWidget
{
	GENERATED_BODY()

public:
	class APrototype2Gamestate* GameStateRef;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PlayerImage;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PlayerColourImage;

	// -- Character image textures --
	// Cow
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture_CowRed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture_CowBlue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture_CowGreen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture_CowYellow;

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> CowTextures;
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> ChickenTextures;
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> PigTextures;
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> DuckTextures;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacters IdealCharacter{ECharacters::COW};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterColours IdealCharacterColour{ECharacterColours::RED};
	

	virtual void NativeOnInitialized() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterLeft();

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterRight();

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterColourLeft();

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterColourRight();

	UFUNCTION(BlueprintCallable)
	void UpdateCharacterImage();

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterColour(bool _right);

	UFUNCTION(BlueprintCallable)
	void ChangeCharacter(bool _right);

	UFUNCTION(BlueprintCallable)
	void CheckForTakenSkin(bool _right);

	UFUNCTION(BlueprintCallable)
	void CheckForTakenCharacter(bool _right);

	void UpdateCharacterColourImage(ECharacters _desiredCharacter);
};
