#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Gamestates/LobbyGamestate.h"
#include "LobbyPlayerState.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

	ALobbyPlayerState();
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	UPROPERTY(Replicated, VisibleAnywhere)
	bool IsReady{};

	UPROPERTY(Replicated, VisibleAnywhere)
	int Player_ID{};
	
	void SetIsReady(bool _isReady);

	void UpdateCharacterMaterial(ECharacters _character, ECharacterColours _characterColour);

	// Character & Colour
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ECharacters Character{ECharacters::COW};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ECharacterColours CharacterColour{ECharacterColours::RED};
};
