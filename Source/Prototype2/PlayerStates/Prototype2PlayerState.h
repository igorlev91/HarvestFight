#pragma once

#include "CoreMinimal.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Prototype2PlayerState.generated.h"

UCLASS()
class PROTOTYPE2_API APrototype2PlayerState : public APlayerState
{
	
	GENERATED_BODY()

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	APrototype2PlayerState();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;

public:
	void AddCoins(int32 _amount);
	void AddCoins(class APlant* _SomePlant);
	
	void GrabSkinFromGameInstance();

	void UpdateCharacterMaterial(FCharacterDetails _Details);
	
	UFUNCTION(Server, Reliable)
	void Server_GrabSkinFromGameInstance(FCharacterDetails _Details);
	void Server_GrabSkinFromGameInstance_Implementation(FCharacterDetails _Details);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GrabSkinFromGameInstance(FCharacterDetails _Details);
	void Multi_GrabSkinFromGameInstance_Implementation(FCharacterDetails _Details);

	UFUNCTION(Client, Unreliable)
	void Client_OnAddCoins();
	
	void Multi_OnAddCoins();

	bool IsLoosing();
	bool IsWinning();
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSold, int32, _PlayerID);
	UPROPERTY(BlueprintAssignable)
	FOnItemSold OnItemSoldDelegate;
	
	bool bSkinSet{};
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
    int32 Player_ID{};
	
	UPROPERTY(Replicated, EditAnywhere)
	int32 Coins{0};

	UPROPERTY(Replicated, VisibleAnywhere)
	FString PlayerName{};

	UPROPERTY(VisibleAnywhere)
	class APrototype2Gamestate* Gamestate;

	// Showing coins that are being added to total
	UPROPERTY(Replicated, EditAnywhere)
	int32 ExtraCoins{0};
	UPROPERTY(Replicated, EditAnywhere)
	bool bIsShowingExtraCoins{false};
	UPROPERTY(Replicated, EditAnywhere)
	float MaxTimeShowExtraCoins{};
	UPROPERTY(Replicated, VisibleAnywhere)
	float TimerExtraCoins{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FCharacterDetails Details;

	/* Player UI Textures */
public:
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> CowTextures{{}, {}, {}, {}, {}, {}, {}, {}};

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> PigTextures{{}, {}, {}, {}, {}, {}, {}, {}};

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> ChickenTextures{{}, {}, {}, {}, {}, {}, {}, {}};

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> DuckTextures{{}, {}, {}, {}, {}, {}, {}, {}};
};
