

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "GameFramework/Character.h"
#include "LobbyCharacter.generated.h"

class UWidgetComponent;
UCLASS()
class PROTOTYPE2_API ALobbyCharacter : public ACharacter
{
	GENERATED_BODY()

	/* Public Functions */
public:
	void SetPlayerStateRef(class ALobbyPlayerState* _NewLobbyPlayerState);
	void SetNameWidget(FString _name);
	void SetNameWidgetPlayerRef();

	UFUNCTION(Client, Reliable)
	void Client_InitPlayerHUD();
	UFUNCTION(Server, Reliable)
	void Server_InitPlayerHUD();
	void InitPlayerHUD();

	UFUNCTION()
	void SetIdealDetails(FCharacterDetails _IdealDetails);
	UFUNCTION(Client, Reliable)
	void Client_SetIdealDetails(FCharacterDetails _IdealDetails);
	
	/* Protected Functions */
protected:
	ALobbyCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* _PlayerInputComponent) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetNameWidget(const FString& _name);
	
	UFUNCTION(Server, Reliable)
	void Server_SetCharacterMesh();
	void Server_SetCharacterMesh_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetCharacterMesh();
	void Multi_SetCharacterMesh_Implementation();

	void SyncCharacterSkin();
	
	UFUNCTION()
	bool HasIdealRole();

	/* Public Variables */
public:

	/* Need reference to the AnimBP to be set dynamically after updating skeletal mesh */
	UPROPERTY(EditAnywhere)
	UClass* TemplatedAnimationBlueprint;
	
	/* Protected Variables */
protected:
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	class ALobbyPlayerState* PlayerStateRef;

	UPROPERTY(VisibleAnywhere)
	class UWidget_LobbyPlayerHUDV2* PlayerHUDRef;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UWidget_LobbyPlayerHUDV2> PlayerHudPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TArray<USkeletalMesh*> PlayerMeshes;

	///* Display name widget (above head) */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	//UWidgetComponent* PlayerNameWidgetComponent;
	
	UPROPERTY(meta = (AllowPrivateAccess))
	class UWidget_PlayerName* PlayerNameWidget{nullptr};
	
	///* Ready button widget (above head) */
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	//UWidgetComponent* ReadyImageWidgetComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstanceDynamic*> PlayerMaterialsDynamic{};
};
