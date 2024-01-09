

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Math/Vector.h"
#include "Widget_LobbyCharacterSelection.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_LobbyCharacterSelection : public UUserWidget
{
	GENERATED_BODY()

	/* Public functions */
public:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	/* Updates the UI character image based on the selected character */
	UFUNCTION(BlueprintCallable)
	void UpdateCharacterImage();

	/**
	 * @brief Changes the character colour
	 * @param _bIsTowardsRight 
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeCharacterColour(bool _bIsTowardsRight);

	/**
	 * @brief Changes the character model
	 * @param _bIsTowardsRight 
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeCharacter(bool _bIsTowardsRight);

	/**
	 * @brief Checks through other players to see if current skin (colour) is taken
	 * @param _bIsTowardsRight 
	 */
	UFUNCTION(BlueprintCallable)
	void CheckForTakenSkin(bool _bIsTowardsRight);

	/**
	 * @brief Checks through other players to see if current character skin is taken
	 * @param _bIsTowardsRight 
	 */
	UFUNCTION(BlueprintCallable)
	void CheckForTakenCharacter(bool _bIsTowardsRight);

	/* Public Variables */
public:
	UPROPERTY()
	class APrototype2Gamestate* GameStateRef;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PlayerImage;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UImage* PlayerColourImage;

	/* Character image textures */
	/* Cow */
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
	FVector4 IdealCharacterColour{0.428690,0.102242,0.102242,1.000000};
};
