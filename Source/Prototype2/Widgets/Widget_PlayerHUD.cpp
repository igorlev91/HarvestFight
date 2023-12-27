// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_PlayerHUD.h"
#include "Widget_IngameMenu.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Prototype2GameMode.h"

void UWidget_PlayerHUD::EnableDisableMenu()
{
	IngameMenu->EnableDisableMenu();
}
