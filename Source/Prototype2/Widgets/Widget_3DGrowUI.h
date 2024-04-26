

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_3DGrowUI.generated.h"

USTRUCT(BlueprintType)
struct FFlowerData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 StarValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Count;
};

UCLASS()
class PROTOTYPE2_API UWidget_3DGrowUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void SetStarCount(int32 _Count);

	UFUNCTION()
	void SetFlowerTypes(TArray<FFlowerData> _FlowerDatas);

	UFUNCTION()
	void SetGrowTimer(float _Progress, bool _Gold = false);

protected:

	UPROPERTY(EditAnywhere)
	TArray<class UTexture2D*> StarTextures;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWidget_Flower> FlowerWidget;
	
	UPROPERTY(VisibleAnywhere)
	TArray<class UWidget_Flower*> FlowerWidgets;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* I_Stars;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* Timer;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UHorizontalBox* H_Flowers;

protected:
	UPROPERTY(VisibleAnywhere)
	class UMaterialInstanceDynamic* TimerMat;
};
