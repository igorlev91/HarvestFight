// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SellBin.h"
#include "Prototype2Character.generated.h"

class UAudioComponent;
class USoundCue;
class UItemComponent;

UCLASS(config=Game)
class APrototype2Character : public ACharacter
{
	GENERATED_BODY()
public:
	APrototype2Character();	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsSprinting();

	void CheckForFloorSurface();
	
	UPROPERTY(VisibleAnywhere, Replicated)
	int PlayerID{-1};
	
	/* Public Networking functions */
	// Item
	UFUNCTION(Server, Reliable)
	void Server_PickupItem(UItemComponent* itemComponent, APickUpItem* _item);
	void Server_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item);

	UFUNCTION(Server, Reliable)
	void Server_DropItem();
	void Server_DropItem_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SocketItem(UStaticMeshComponent* _object, FName _socket);
	void Multi_SocketItem_Implementation(UStaticMeshComponent* _object, FName _socket);

	// Weapon
	UFUNCTION(Server, Reliable)
	void Server_SocketItem(UStaticMeshComponent* _object, FName _socket);
	void Server_SocketItem_Implementation(UStaticMeshComponent* _object, FName _socket);

	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();
	void Server_DropWeapon_Implementation();
	
	// Attack
	UFUNCTION(Server, Reliable)
	void Server_StartAttack();
	void Server_StartAttack_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_StartAttack();
	void Multi_StartAttack_Implementation();
	
	UFUNCTION(Server, Reliable)
	void Server_ReleaseAttack();
	void Server_ReleaseAttack_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ReleaseAttack();
	void Multi_ReleaseAttack_Implementation();

	// VFX events
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerAttackVFX(FVector Position, float Radius, float Charge);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StretchWeapon();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SquashWeapon();

	UFUNCTION(BlueprintCallable)
	void SocketWeapon(FName Socket);
	
	// HUD
	UFUNCTION(Server, Reliable)
	void Server_AddHUD();
	void Server_AddHUD_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Client_AddHUD();
	void Multi_Client_AddHUD_Implementation();
	
	UFUNCTION(Client, Reliable)
	void Client_AddHUD();
	void Client_AddHUD_Implementation();

	// Audio
	void PlaySoundAtLocation(FVector Location, USoundCue* SoundToPlay, USoundAttenuation* _attenation = nullptr);
	
	UFUNCTION(Server, Reliable)
	void Server_PlaySoundAtLocation(FVector _location, USoundCue* _soundQueue, USoundAttenuation* _attenation);
	void Server_PlaySoundAtLocation_Implementation(FVector _location, USoundCue* _soundQueue, USoundAttenuation* _attenation);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlaySoundAtLocation(FVector _location, USoundCue* _soundQueue, USoundAttenuation* _attenation);
	void Multi_PlaySoundAtLocation_Implementation(FVector _location, USoundCue* _soundQueue, USoundAttenuation* _attenation);
	
	// Ragdoll
	void Ragdoll(bool _ragdoll);
	UFUNCTION(Server, Reliable)
	void Server_Ragdoll(bool _ragdoll);
	void Server_Ragdoll_Implementation(bool _ragdoll);
	UFUNCTION(NetMulticast, Reliable)
    void Multi_Ragdoll(bool _ragdoll);
    void Multi_Ragdoll_Implementation(bool _ragdoll);
	
	class UWidget_PlayerHUD* GetPlayerHUD();

	/* Stencil - on/off */
	void EnableStencil(bool _on);

	/* Decal Arrow */
	void UpdateDecalDirection(bool _on);
	void UpdateDecalDirection(bool _on, bool _targetShippingBin);

protected: /* Protected Networking functions */
	void PlayNetworkMontage(UAnimMontage* _montage);
	
	UFUNCTION(Server, Reliable)
	void Server_PlayNetworkMontage(UAnimMontage* _montage);
	void Server_PlayNetworkMontage_Implementation(UAnimMontage* _montage);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayNetworkMontage(UAnimMontage* _montage);
	void Multi_PlayNetworkMontage_Implementation(UAnimMontage* _montage);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerColour();
	void Server_SetPlayerColour_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_Sprint();
	void Server_Sprint_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlayerColour();
	void Multi_SetPlayerColour_Implementation();

	void TryInteract();
	
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();
	void Server_TryInteract_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropItem();
	void Multi_DropItem_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PickupItem(UItemComponent* itemComponent, APickUpItem* _item);
	void Multi_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropWeapon();
	void Multi_DropWeapon_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& InMaterialsArray);
	void Server_ReceiveMaterialsArray_Implementation(const TArray<UMaterialInstance*>& InMaterialsArray);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& InMaterialsArray);
	void Multi_ReceiveMaterialsArray_Implementation(const TArray<UMaterialInstance*>& InMaterialsArray);

	ENetRole IdealNetRole{ROLE_AutonomousProxy};

	UFUNCTION(Server, Reliable)
	void Server_FireDizzySystem();
	void Server_FireDizzySystem_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireParticleSystem();
	void Multi_FireParticleSystem_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_ToggleChargeSound(bool _soundEnabled);
	void Server_ToggleChargeSound_Implementation(bool _soundEnabled);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleChargeSound(bool _soundEnabled);
	void Multi_ToggleChargeSound_Implementation(bool _soundEnabled);

	UFUNCTION(Server, Reliable)
	void Server_CountdownTimers(float DeltaSeconds);
	void Server_CountdownTimers_Implementation(float DeltaSeconds);

protected: /* Protected non-network Functions */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/* Called for Attack input */
	void ChargeAttack();

	/* Release Attack */
	void ReleaseAttack();

	/* Execute Attack */
	void ExecuteAttack(float AttackSphereRadius);

	/* Pickup/Plant/Sell */
	void Interact();

	/* Activate Sprint */
	void Sprint();

	/* Handle character speed */
	void UpdateCharacterSpeed(float _WalkSpeed, float _SprintSpeed, float _BaseAnimationRateScale);
	
	/* Create a sphere collider which calculates nearest item */
	void CheckForInteractables();
	
	/* Called when hit by another player */
	UFUNCTION(BlueprintCallable)
	void GetHit(float AttackCharge, FVector AttackerLocation);
	
	/* UI */
	void OpenIngameMenu();
	
	/* Update decal direction */
	void UpdateDecalAngle();

public: /* Public variables */
	UPROPERTY(VisibleAnywhere)
	UWidget_PlayerHUD* PlayerHUDRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<USkeletalMesh*> PlayerMeshes;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class APrototype2PlayerState* PlayerStateRef;

	/* Audio */
	UPROPERTY(EditAnywhere, Replicated)
	USoundAttenuation* SoundAttenuationSettings;

	UPROPERTY(EditAnywhere, Replicated)
	UAudioComponent* ChargeAttackAudioComponent;
	
	UPROPERTY(EditAnywhere)
	USoundCue* ChargeCue;
	
	UPROPERTY(EditAnywhere)
	USoundCue* ExecuteCue;
	
	UPROPERTY(EditAnywhere)
	USoundCue* PickUpCue;
	
	UPROPERTY(EditAnywhere)
	USoundCue* DropCue;
	
	UPROPERTY(EditAnywhere)
	USoundCue* SellCue;
	
	UPROPERTY(EditAnywhere)
	USoundCue* PlantCue;
	
	UPROPERTY(EditAnywhere)
	USoundCue* GetHitCue;
	
	UPROPERTY(EditAnywhere)
	USoundCue* MandrakeScreamCue;

	/* Material */
	UPROPERTY(VisibleAnywhere, Replicated)
	UMaterialInstance* PlayerMat;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWeapon* Weapon;
	
	/* Currently held item */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APickUpItem* HeldItem;

	/* Is player holding down attack */
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsChargingAttack;

	/* Maximum amount of Attack Charge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAttackCharge = 3.0f;

	UPROPERTY(Replicated, BlueprintReadWrite)
	float AttackChargeAmount;

	/* Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CanSprintTime = 5.0f;

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere)
	float CanSprintTimer;

	/* Weapon degrading */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int WeaponCurrentDurability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int WeaponMaxDurability = 5;

	
	class IInteractInterface* ClosestInteractableItem;

	UPROPERTY(VisibleAnywhere)
	AActor* ClosestInteractableActor;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* ParticleSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* InteractSystem;
	
	UPROPERTY(Replicated, EditAnywhere)
	class UNiagaraComponent* DizzyComponent;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DizzySystem;

	UPROPERTY(Replicated, EditAnywhere)
	bool bIsHoldingGold;

	/* Decal component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DecalArmSceneComponent;
	
	UPROPERTY(EditAnywhere)
	UDecalComponent* DecalComponent;

	UPROPERTY(EditAnywhere)
	FVector StartPosition{};

	UPROPERTY(EditAnywhere)
	bool bDecalOn{false};

	UPROPERTY(EditAnywhere)
	bool bDecalTargetShippingBin{false};

	UPROPERTY(EditAnywhere)
	ASellBin* SellBin;
	
protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private: /* Private variables */
	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/* UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MenuAction;

	/* Attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChargeAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReleaseAttackAction;

	/* PickUp/Plant/Sell */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/* Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/* Animation */
	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* PickupMontage;
	
	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* ExecuteAttackMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* ExecuteAttackMontage_LongerWindUp;
	
	/* Interact radius for checking closest item */
	UPROPERTY(EditAnywhere)
	float InteractRadius = 225.0f;
	
	/* Amount of knockback applied which is multiplied by charge */
	UPROPERTY(EditAnywhere, Category = KnockBack)
	float KnockBackAmount = 1000.0f;
	UPROPERTY(EditAnywhere, Category = KnockBack)
	float MaxKnockBackVelocity = 10000.0f;
	
	/* Interact timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InteractTimerTime = 1.0f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InteractTimer{};
	
	/* Attack timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float AttackTimerTime = 1.0f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float AttackTimer{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InstantAttackDelay = 0.2f;
	UPROPERTY(EditAnywhere)
	float ItemLaunchStrength = 500000.0f;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWidget_PlayerHUD> PlayerHudPrefab;

	
	/* Reference to change speed: Sprint/Walk/Slow Walk */
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimSequence* RunAnimation;
	
	UPROPERTY(EditAnywhere, Category = Animation)
	TArray<UAnimSequence*> RunAnimations;
	
	UPROPERTY(EditAnywhere)
	float IceFriction{0.1f};
	
	float WalkSpeed = 500.f;

	float GoldPlantSpeed = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintSpeed = 750.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTime = 2.0f;

	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTimer;

	UPROPERTY(EditAnywhere, Category = RateScale)
	float WalkRateScale = 1.5f;
	UPROPERTY(EditAnywhere, Category = RateScale)
	float GoldSlowRateScale = 0.7f;
	UPROPERTY(EditAnywhere, Category = RateScale)
	float SprintRateScaleScalar = 1.5f;

	/* Attack */
	UPROPERTY(EditAnywhere)
	float InstantAttackThreshold = 1.0f;
	
	bool bCanAttack = true;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bIsStunned;
	
	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float StunTimer;
		
	UPROPERTY(EditAnywhere, Category=Attack)
	FVector KnockUp = {1.0f, 1.0f, 100000000.0f};
	
	/* Other */
	bool DoOnce{};

	UPROPERTY(Replicated, VisibleAnywhere)
	FTransform LocationWhenStunned{};
	
	UPROPERTY(Replicated, VisibleAnywhere)
	FTransform MeshLocationWhenStunned{};
};