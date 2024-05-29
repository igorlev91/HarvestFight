

#include "Widget_PlayerNamesTeam2.h"
#include "Components/Overlay.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UWidget_PlayerNamesTeam2::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/* Get gamestate and set to reference variable */
	if (auto* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}

	/* Team two - Init arrays */
	OverlaysTeam2.Add(OverlayT2_1);
	OverlaysTeam2.Add(OverlayT2_2);
	OverlaysTeam2.Add(OverlayT2_3);
	OverlaysTeam2.Add(OverlayT2_4);

	PlayerIconsTeam2.Add(PlayerIconT2_1);
	PlayerIconsTeam2.Add(PlayerIconT2_2);
	PlayerIconsTeam2.Add(PlayerIconT2_3);
	PlayerIconsTeam2.Add(PlayerIconT2_4);

	PlayerRingsTeam2.Add(PlayerRingT2_1);
	PlayerRingsTeam2.Add(PlayerRingT2_2);
	PlayerRingsTeam2.Add(PlayerRingT2_3);
	PlayerRingsTeam2.Add(PlayerRingT2_4);
	
	PlayerNamesTeam2.Add(PlayerNameT2_1);
	PlayerNamesTeam2.Add(PlayerNameT2_2);
	PlayerNamesTeam2.Add(PlayerNameT2_3);
	PlayerNamesTeam2.Add(PlayerNameT2_4);

	PlayerBackgrounds.Add(BackgroundT2_1);
	PlayerBackgrounds.Add(BackgroundT2_2);
	PlayerBackgrounds.Add(BackgroundT2_3);
	PlayerBackgrounds.Add(BackgroundT2_4);

	UpdatePlayerNames();
}

void UWidget_PlayerNamesTeam2::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	for (int i = 0; i < GameStateReference->Server_TeamTwo.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_TeamTwo[i])
		{
			/* Set ring colours to player colour */
			if (PlayerRingsTeam2.Num() > i)
			{
				PlayerRingsTeam2[i]->SetColorAndOpacity((FLinearColor)(Player->Details.PureToneColour));
			}

			/* Set player icons to correct image based on character and colour */
			if (PlayerIconsTeam2.Num() > i)
			{
				switch (Player->Details.Character)
				{
				case ECharacters::COW:
					{
						PlayerIconsTeam2[i]->SetBrushFromTexture(Player->CowTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::PIG:
					{
						PlayerIconsTeam2[i]->SetBrushFromTexture(Player->PigTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::CHICKEN:
					{
						PlayerIconsTeam2[i]->SetBrushFromTexture(Player->ChickenTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::DUCK:
					{
						PlayerIconsTeam2[i]->SetBrushFromTexture(Player->DuckTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::BEE:
					{
						PlayerIconsTeam2[i]->SetBrushFromTexture(Player->BeeTextures[(int32)Player->Details.Colour]);
						break;
					}
				default:
					{
						UE_LOG(LogTemp, Warning, TEXT("Error: Widget_PlayerNames: Unable to determine character type"));
						break;
					}
				}
			}
			
			/* Set player names to actual names */
			if (PlayerNamesTeam2.Num() > i)
			{
				PlayerNamesTeam2[i]->SetText(FText::FromString(FString(Player->PlayerName)));
			}

			/* Set colour of backgrounds */
			if (PlayerBackgrounds.Num() > i)
			{
				if (PlayerBackgrounds.Num() > i)
				{
					PlayerBackgrounds[i]->SetColorAndOpacity(FLinearColor(
						Player->Details.PureToneColour.X,
						Player->Details.PureToneColour.Y,
						Player->Details.PureToneColour.Z,
						0.7f));
				}
			}
		}
	}
}

void UWidget_PlayerNamesTeam2::UpdatePlayerNames()
{
	if (!GameStateReference)
		return;

	if (GameStateReference->Server_TeamTwo.Num() == 4)
	{
		OverlayT2_1->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_2->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_3->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_4->SetVisibility(ESlateVisibility::Visible);
	}
	else if (GameStateReference->Server_TeamTwo.Num() == 3)
	{
		OverlayT2_1->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_2->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_3->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_4->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (GameStateReference->Server_TeamTwo.Num() == 2)
	{
		OverlayT2_1->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_2->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_3->SetVisibility(ESlateVisibility::Hidden);
		OverlayT2_4->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (GameStateReference->Server_TeamTwo.Num() == 1)
	{
		OverlayT2_1->SetVisibility(ESlateVisibility::Visible);
		OverlayT2_2->SetVisibility(ESlateVisibility::Hidden);
		OverlayT2_3->SetVisibility(ESlateVisibility::Hidden);
		OverlayT2_4->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		OverlayT2_1->SetVisibility(ESlateVisibility::Hidden);
		OverlayT2_2->SetVisibility(ESlateVisibility::Hidden);
		OverlayT2_3->SetVisibility(ESlateVisibility::Hidden);
		OverlayT2_4->SetVisibility(ESlateVisibility::Hidden);
	}
}
