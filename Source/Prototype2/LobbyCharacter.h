// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrototypeGameInstance.h"
#include "GameFramework/Character.h"
#include "LobbyCharacter.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALobbyCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable)
	void Server_SetCharacterMesh();
	void Server_SetCharacterMesh_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetCharacterMesh();
	void Multi_SetCharacterMesh_Implementation();
	
	UPROPERTY(VisibleAnywhere, Replicated)
	UMaterialInstance* PlayerMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ALobbyPlayerState* PlayerStateRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<USkeletalMesh*> PlayerMeshes;
};
