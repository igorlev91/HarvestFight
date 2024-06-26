#pragma once

#include "CoreMinimal.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Prototype2PlayerState.generated.h"

UENUM(BlueprintType)
enum class EEmote : uint8
{
	Emote1,
	Emote2,
	Emote3,
	Emote4,
	Emote5,
	Emote6,
	Emote7,
	Emote8,
	NONE
};

UCLASS()
class PROTOTYPE2_API APrototype2PlayerState : public APlayerState
{
	
	GENERATED_BODY()

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	APrototype2PlayerState();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;

public:
	UFUNCTION()
	void VoteMap(EFarm _Map);
	UFUNCTION(Server, Reliable)
	void Server_VoteMap(EFarm _Map);
	
	void AddCoins(int32 _amount);
	
	void GrabSkinFromGameInstance();

	void UpdateCharacterMaterial(FCharacterDetails _Details);
	
	UFUNCTION(Server, Reliable)
	void Server_GrabSkinFromGameInstance(FCharacterDetails _Details);
	void Server_GrabSkinFromGameInstance_Implementation(FCharacterDetails _Details);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GrabSkinFromGameInstance(FCharacterDetails _Details);
	void Multi_GrabSkinFromGameInstance_Implementation(FCharacterDetails _Details);

	UFUNCTION()
	void Client_OnAddCoins(int32 _Score);
	
	void Multi_OnAddCoins(int32 _Score);

	bool IsLoosing();
	bool IsWinning();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSold, int32, _PlayerID, int32, _Score);
	UPROPERTY(BlueprintAssignable)
	FOnItemSold OnItemSoldDelegate;
	
	bool bSkinSet{};
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
    int32 Player_ID{};

	UFUNCTION()
	void OnRep_Coins();
	
	UPROPERTY(ReplicatedUsing=OnRep_Coins, EditAnywhere)
	int32 Coins{0};

	UPROPERTY(VisibleAnywhere)
	int32 LastLocalCoins{};

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FString PlayerName{};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	EEmote Emote{EEmote::Emote1};

	UPROPERTY(VisibleAnywhere)
	class APrototype2Gamestate* Gamestate;

	// Showing coins that are being added to total
	//UPROPERTY(Replicated, EditAnywhere)
	//int32 ExtraCoins{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ExtraCoinsLocal{};

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
	
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> BeeTextures{{}, {}, {}, {}, {}, {}, {}, {}};
};
