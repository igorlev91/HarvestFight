

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Flower.generated.h"

UCLASS()
class PROTOTYPE2_API UWidget_Flower : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetFlowerType(int32 _StarValue);

	UFUNCTION()
	void SetFlowerCount(int32 _Amount);

protected:
	UPROPERTY(EditAnywhere)
	TArray<class UTexture2D*> FlowerTextures;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* I_Flower;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* Count;
};
