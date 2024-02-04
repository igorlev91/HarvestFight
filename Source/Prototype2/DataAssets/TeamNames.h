

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "TeamNames.generated.h"
USTRUCT()
struct FColouredNames
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FString> Names{};
};
UCLASS()
class PROTOTYPE2_API UTeamNames : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<EColours, FColouredNames> TeamNames
	{
		{EColours::BLUE, 
			FColouredNames{
				TArray<FString>{
				"Blue Blazers",
				"Blue Bombers",
				"Blue Buccaneers",
				"Blue Bandits",
				"Blue Bulls",
				"Blue Buccaneers",
				"Blue Bisons",
				"Blue Bulldogs",
				"Blue Bobcats",
				"Blue Barracudas"
				}
			}
		},
		{EColours::GREEN, 
			FColouredNames{
				TArray<FString>{
				"Green Giants",
				"Green Geckos",
				"Green Gazelles",
				"Green Gators",
				"Green Griffins",
				"Green Goblins",
				"Green Gorillas",
				"Green Gliders",
				"Green Ghosts",
				"Green Guardians"
				}
			}
		},
		{EColours::YELLOW, 
			FColouredNames{
				TArray<FString>{
				"Yellow Yaks",
				"Yellow Yetis",
				"Yellow Yankees",
				"Yellow Yorkies",
				"Yellow Yachts",
				"Yellow Yogis",
				"Yellow Yo-yos",
				"Yellow Yodas",
				"Yellow Yeomen",
				"Yellow Yodelers"
				}
			}
		},
		{EColours::PURPLE, 
			FColouredNames{
				TArray<FString>{
				"Purple Panthers",
				"Purple Pythons",
				"Purple Penguins",
				"Purple Pumas",
				"Purple Parrots",
				"Purple Pelicans",
				"Purple Patriots",
				"Purple Peacocks",
				"Purple Phantoms",
				"Purple Paladins"
				}
			}
		},
		{EColours::ORANGE, 
			FColouredNames{
				TArray<FString>{
				"Orange Owls",
				"Orange Otters",
				"Orange Ocelots",
				"Orange Orcas",
				"Orange Octopuses",
				"Orange Outlaws",
				"Orange Oxen",
				"Orange Orioles",
				"Orange Olympians",
				"Orange Orators"
				}
			}
		},
		{EColours::BLACK, 
			FColouredNames{
				TArray<FString>{
				"Black Bears",
				"Black Bats",
				"Black Bulldogs",
				"Black Badgers",
				"Black Buffaloes",
				"Black Buccaneers",
				"Black Bullfrogs",
				"Black Bandicoots",
				"Black Beagles",
				"Black Beetles"
				}
			}
		},
		{EColours::WHITE, 
			FColouredNames{
				TArray<FString>{
				"White Wolves",
				"White Weasels",
				"White Walruses",
				"White Wildcats",
				"White Wombats",
				"White Whippets",
				"White Wrens",
				"White Warthogs",
				"White Wizards",
				"White Woodpeckers"
				}
			}
		}
	};
};
