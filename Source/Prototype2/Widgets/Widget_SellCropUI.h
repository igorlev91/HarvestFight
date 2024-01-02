// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_SellCropUI.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_SellCropUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* SellText;

	void SetCropValue(int _value);
};
