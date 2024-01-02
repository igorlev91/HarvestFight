// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
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
	void SelectLevel(EFarm _level);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMapSelected{false};
	

	// Buttons

	
	// Normal farm
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* Farm_Counter;

	// Winter Farm
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UTextBlock* WinterFarm_Counter;
	
};
