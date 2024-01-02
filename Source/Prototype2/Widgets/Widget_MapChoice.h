// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MapChoice.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UWidget_MapChoice : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void EnableMapChoice();

	UFUNCTION(BlueprintCallable)
	void SelectLevel(FString _level);
	
};
