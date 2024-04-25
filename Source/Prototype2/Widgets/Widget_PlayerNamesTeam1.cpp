

#include "Widget_PlayerNamesTeam1.h"

#include "Components/Overlay.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UWidget_PlayerNamesTeam1::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	/* Get gamestate and set to reference variable */
	if (auto* GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameState;
	}

	/* Team one - Init arrays */
	OverlaysTeam1.Add(OverlayT1_1);
	OverlaysTeam1.Add(OverlayT1_2);
	OverlaysTeam1.Add(OverlayT1_3);
	OverlaysTeam1.Add(OverlayT1_4);

	PlayerIconsTeam1.Add(PlayerIconT1_1);
	PlayerIconsTeam1.Add(PlayerIconT1_2);
	PlayerIconsTeam1.Add(PlayerIconT1_3);
	PlayerIconsTeam1.Add(PlayerIconT1_4);

	PlayerRingsTeam1.Add(PlayerRingT1_1);
	PlayerRingsTeam1.Add(PlayerRingT1_2);
	PlayerRingsTeam1.Add(PlayerRingT1_3);
	PlayerRingsTeam1.Add(PlayerRingT1_4);
	
	PlayerNamesTeam1.Add(PlayerNameT1_1);
	PlayerNamesTeam1.Add(PlayerNameT1_2);
	PlayerNamesTeam1.Add(PlayerNameT1_3);
	PlayerNamesTeam1.Add(PlayerNameT1_4);

	PlayerBackgrounds.Add(BackgroundT1_1);
	PlayerBackgrounds.Add(BackgroundT1_2);
	PlayerBackgrounds.Add(BackgroundT1_3);
	PlayerBackgrounds.Add(BackgroundT1_4);
}

void UWidget_PlayerNamesTeam1::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	/* Team 1 */
	if (GameStateReference->Server_TeamOne.Num() < 4)
	{
		OverlayT1_4->SetVisibility(ESlateVisibility::Hidden);

		if (GameStateReference->Server_TeamOne.Num() < 3)
		{
			OverlayT1_3->SetVisibility(ESlateVisibility::Hidden);

			if (GameStateReference->Server_TeamOne.Num() < 2)
			{
				OverlayT1_2->SetVisibility(ESlateVisibility::Hidden);

				if (GameStateReference->Server_TeamOne.Num() < 1)
				{
					OverlayT1_1->SetVisibility(ESlateVisibility::Hidden);
				}
				else
				{
					OverlayT1_1->SetVisibility(ESlateVisibility::Visible);
				}
			}
			else
			{
				OverlayT1_2->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			OverlayT1_3->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		OverlayT1_4->SetVisibility(ESlateVisibility::Visible);
	}

	for (int i = 0; i < GameStateReference->Server_TeamOne.Num(); i++)
	{
		if (auto Player = GameStateReference->Server_TeamOne[i])
		{
			/* Set ring colours to player colour */
			if (PlayerRingsTeam1.Num() > i)
			{
				PlayerRingsTeam1[i]->SetColorAndOpacity((FLinearColor)(Player->Details.PureToneColour));
			}

			/* Set player icons to correct image based on character and colour */
			if (PlayerIconsTeam1.Num() > i)
			{
				switch (Player->Details.Character)
				{
				case ECharacters::COW:
					{
						PlayerIconsTeam1[i]->SetBrushFromTexture(Player->CowTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::PIG:
					{
						PlayerIconsTeam1[i]->SetBrushFromTexture(Player->PigTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::CHICKEN:
					{
						PlayerIconsTeam1[i]->SetBrushFromTexture(Player->ChickenTextures[(int32)Player->Details.Colour]);
						break;
					}
				case ECharacters::DUCK:
					{
						PlayerIconsTeam1[i]->SetBrushFromTexture(Player->DuckTextures[(int32)Player->Details.Colour]);
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
			if (PlayerNamesTeam1.Num() > i)
			{
				PlayerNamesTeam1[i]->SetText(FText::FromString(FString(Player->PlayerName)));
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
