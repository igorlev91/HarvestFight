// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Tutorial.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_Tutorial : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int tutorialPage{};
};
