// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_LobbyCharacterSelection.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

void UWidget_LobbyCharacterSelection::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PlayerImage->SetBrushFromTexture(Texture_CowRed);
	PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
}

void UWidget_LobbyCharacterSelection::ChangeCharacterLeft()
{
	
}

void UWidget_LobbyCharacterSelection::ChangeCharacterRight()
{
	
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColourLeft()
{
	switch(CharacterColour)
	{
	case ECharacterColours::RED:
		CharacterColour = ECharacterColours::YELLOW;
		break;
	case ECharacterColours::BLUE:
		CharacterColour = ECharacterColours::RED;
		break;
	case ECharacterColours::GREEN:
		CharacterColour = ECharacterColours::BLUE;
		break;
	case ECharacterColours::YELLOW:
		CharacterColour = ECharacterColours::GREEN;
		break;
	default:
		CharacterColour = ECharacterColours::RED;
		break;
	}

	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::ChangeCharacterColourRight()
{
	switch(CharacterColour)
	{
	case ECharacterColours::RED:
		CharacterColour = ECharacterColours::BLUE;
		break;
	case ECharacterColours::BLUE:
		CharacterColour = ECharacterColours::GREEN;
		break;
	case ECharacterColours::GREEN:
		CharacterColour = ECharacterColours::YELLOW;
		break;
	case ECharacterColours::YELLOW:
		CharacterColour = ECharacterColours::RED;
		break;
	default:
		CharacterColour = ECharacterColours::RED;
		break;
	}

	UpdateCharacterImage();
}

void UWidget_LobbyCharacterSelection::UpdateCharacterImage()
{
	switch(CharacterColour)
	{
	case ECharacterColours::RED:
		PlayerImage->SetBrushFromTexture(Texture_CowRed);
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
		break;
	case ECharacterColours::BLUE:
		PlayerImage->SetBrushFromTexture(Texture_CowBlue);
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.034125f, 0.118703f, 0.192708f, 1.0f));
		break;
	case ECharacterColours::GREEN:
		PlayerImage->SetBrushFromTexture(Texture_CowGreen);
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.078957f, 0.192708f, 0.056207f, 1.0f));
		break;
	case ECharacterColours::YELLOW:
		PlayerImage->SetBrushFromTexture(Texture_CowYellow);
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.317708f, 0.180145, 0.033095, 1.0f));
		break;
	default:
		PlayerImage->SetBrushFromTexture(Texture_CowRed);
		PlayerColourImage->SetColorAndOpacity(FLinearColor(0.427083f, 0.100098f, 0.100098f, 1.0f));
		break;
	}
}
