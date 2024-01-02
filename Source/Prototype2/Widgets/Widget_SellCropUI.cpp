// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_SellCropUI.h"

#include "Components/TextBlock.h"

void UWidget_SellCropUI::SetCropValue(int _value)
{
	FString firstString = "$";
	FString secondString = FString::FromInt(_value);

	FString combinedString = FString::Printf(TEXT("%s %s"), *firstString, *secondString);
	
	SellText->SetText(FText::FromString(combinedString));
}
