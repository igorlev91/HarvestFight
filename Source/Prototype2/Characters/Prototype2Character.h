

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Prototype2/Gameplay/SellBin.h"
#include "Containers/Map.h"
#include "Prototype2/DataAssets/WeaponData.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2Character.generated.h"

class APrototype2PlayerController;
class UAnimationData;
class UAudioComponent;
class USoundCue;
class UItemComponent;
class UDebuffComponent;

/* Enum for controlling particle systems */
UENUM()
enum class EParticleSystems : uint8
{
	Default = 0,

	WalkPoof,
	SprintPoof,
	Sweat,
	AttackTrail,
	Attack,
	Test,

	END
};

UCLASS(config=Game)
class APrototype2Character : public ACharacter
{
	GENERATED_BODY()

protected:
	///////////////////////////////////////////////////////////////
	
	/* Constructor */
	APrototype2Character();	

	/* For networking */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/* Begin play function */
	virtual void BeginPlay();
	void DelayedBeginPlay();

	/* Input Component Setup */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Tick Function*/
	virtual void Tick(float DeltaSeconds) override;
	
	/** Called for movement input */
	void Move(const FInputActionValue& _Value);

	/** Called for looking input */
	void Look(const FInputActionValue& _Value);

	/* Activate Sprint */
	void Sprint();
	
	/* Called for Attack input */
	void ChargeAttack();

	/* Canceled by being hit by other player */
	void CancelChargeAttack();
	
	/* Release Attack */
	void ReleaseAttack();
	
	/* Pickup/Plant/Sell */
	void Interact();
	UFUNCTION(Server, Reliable)
	void Server_Interact();
	
	/* UI */
	void OpenIngameMenu();

	///////////////////////////////////////////////////////////////
	
	UFUNCTION()
	void InitPlayerNameWidgetComponent();
	UFUNCTION()
	void InitAudioComponents();
	UFUNCTION()
	void InitWeapon();
	UFUNCTION()
	void InitWeaponMesh();
	UFUNCTION()
	void InitPlayerHUD();
	UFUNCTION()
	void InitShippingBin();
	UFUNCTION()
	void InitDecals();

	UFUNCTION()
	void ClearInteractionText();

	
	UFUNCTION()
	void HandleAttackChargeBehavior(float _DeltaSeconds);
	UFUNCTION()
	void UpdatePlayerHUD();
	UFUNCTION()
	void UpdateParticleSystems();
	UFUNCTION()
	void ToggleParticleSystems();

	UFUNCTION()
	void DeltaDecrement(float& _Variable, float& _DeltaSeconds);
	UFUNCTION()
	void TickTimers(float _DeltaSeconds);
	UFUNCTION(Server, Unreliable)
	void Server_CountdownTimers(float _DeltaSeconds);

	UFUNCTION(Server, Unreliable)
	void Server_UpdateAttackChargeAmount(float _DeltaSeconds);
	
	UPROPERTY()
	APrototype2PlayerController* PlayerController;
	UPROPERTY()
	APrototype2Gamestate* GameState;
	UPROPERTY(Replicated, VisibleAnywhere)
	class ARadialPlot* ClaimedPlot{};
	
	/////////////////////////////////////////////////////////////////////
public:
	bool HasClaimedPlot();
	void SetClaimedPlot(class ARadialPlot* _Plot);
	UFUNCTION()
	bool HasIdealRole();

	/////////////////////////////////////////////////////////////////////

public:
	/* Delegate for telling the animation blueprint when charge is cancelled*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeCancelled);
	UPROPERTY(BlueprintAssignable)
	FOnChargeCancelled OnChargeCancelledDelegate;

	/* Delegate for telling the Player Hud widget when to emphasize UI */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecuteAttack);
	UPROPERTY(BlueprintAssignable)
	FOnExecuteAttack OnExecuteAttackDelegate;

	UFUNCTION(BlueprintPure)
	bool GetIsPunching () { return DefaultWeaponSeedData == CurrentWeaponSeedData; }
	
	/* Update's character speed according to bIsHoldingGold */
	UFUNCTION()
	void UpdateCharacterSpeed(bool _HoldingGold);
	
	/* Getter for when character is sprinting */
	UFUNCTION(BlueprintCallable)
	bool IsSprinting();

	/* Checks if player is on the ground */
	void CheckForFloorSurface();

	/* Blueprint event for VFX when player attacks */
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerAttackVFX(FVector _Position, float _Radius, float _Charge);

	/* Stretches the weapon mesh on attack */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StretchWeapon();

	/* Squashes the weapon mesh on attack */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SquashWeapon();

	/* Swaps the weapon to a different socket for when attacking or on back */
	UFUNCTION(BlueprintCallable)
	void SocketWeapon(FName _Socket);

	/* Plays audio */
	void PlaySoundAtLocation(FVector _Location, USoundCue* _SoundToPlay, USoundAttenuation* _Attenuation = nullptr);

	/* Ragdolls character - not implemented yet*/
	void Ragdoll(bool _bShouldRagdoll);

	/* Stencil - on/off */
	void EnableStencil(bool _bIsOn);

	/* Decal Arrow */
	void UpdateDecalDirection(bool _bIsOn);
	void UpdateDecalDirection(bool _bIsOn, bool _bIsTargetingShippingBin);

	/* Player teleporter for start and end of game */
	void TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation = FRotator::ZeroRotator);

	/* Activating and Deactivating particle systems */
	void ActivateParticleSystemFromEnum(EParticleSystems _NewSystem);
	void DeActivateParticleSystemFromEnum(EParticleSystems _NewSystem);
	
	/* Getters */
	UFUNCTION(BlueprintCallable)
	class USeedData* GetWeaponData() const { return CurrentWeaponSeedData; }

	/* PlayerHUD getter */
	class UWidget_PlayerHUD* GetPlayerHUD();

	/* Pickup function for doing stuff that doens't need rpc/multi, but calls the rpc which calls multi */
	void PickupItem(APickUpItem* _Item, EPickupActor _PickupType);
	
	/* Called when hit by another player */
	UFUNCTION(BlueprintCallable)
	void GetHit(float _AttackCharge, FVector _AttackerLocation, UWeaponData* _OtherWeaponData);
	
	/* Playing montages */
	void PlayNetworkMontage(UAnimMontage* _Montage);

	class UCameraComponent* ReturnFollowCamera();

	/* Allows for client only functionality when dropping weapon */
	void DropWeapon();

	/* Sets the character speed */
	void SetCharacterSpeed(float _WalkSpeed, float _SprintSpeed, float _BaseAnimationRateScale);
	UFUNCTION(Server, Unreliable)
	void Server_SetCharacterSpeed(float _WalkSpeed, float _SprintSpeed, float _BaseAnimationRateScale);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SetCharacterSpeed(float _WalkSpeed, float _SprintSpeed, float _BaseAnimationRateScale);

	/* Getters for walkspeed and ratescale for debuff component */
	float GetWalkSpeed(){ return WalkSpeed; }
	float GetWalkRateScale() { return WalkRateScale; }

	/* Client side Drop Item function */
	void DropItem();
	
	/* Public Variables */
	
	/* Camera used for end of the game */
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* EndGameCamera;

	/* PlayerHUD reference */
	UPROPERTY(VisibleAnywhere)
	UWidget_PlayerHUD* PlayerHUDRef;

	/* Player State Reference */
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
	
	/* Weapon Mesh */
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

	/* The Amount of charge the player currently has */
	UPROPERTY(Replicated, BlueprintReadWrite)
	float AttackChargeAmount;

	/* Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CanSprintTime = 5.0f;
	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere)
	float CanSprintTimer{};
	
	/* Weapon degrading */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int WeaponCurrentDurability;

	/* Default Weapon Data Asset is no weapon (punching with fists) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	USeedData* DefaultWeaponSeedData;
	
	/* The closest interactable item for HUD showing popup text */
	class IInteractInterface* ClosestInteractableItem;

	/* Closest Interactable Actor for player interacting with things */
	UPROPERTY(VisibleAnywhere)
	AActor* ClosestInteractableActor;

	/* Bool for checking if player is holding gold to slow them down */
	UPROPERTY(Replicated, EditAnywhere)
	bool bIsHoldingGold;

	/* Varaibles needed for Decal component */
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

	/* Variables needed for VFX */
	UPROPERTY(EditAnywhere, Category = VFX) 
	UStaticMeshComponent* AttackAreaIndicatorMesh;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* WalkPoof_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* SprintPoof_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Sweat_NiagaraComponent;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* AttackTrail_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Attack_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Dizzy_NiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TArray<EParticleSystems> ParticleSystemsToActivate{};
	UPROPERTY(VisibleAnywhere)
	TArray<EParticleSystems> ParticleSystemsToDeActivate{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationData* AnimationData;
	
	/* The current weapon data to use data from */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	class USeedData* CurrentWeaponSeedData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintReadOnly)
	EWeaponAnimation CurrentWeaponAnimation{};

	/* Used to stop player input moving the player */
	UPROPERTY(Replicated, VisibleAnywhere)
	bool bAllowMovementFromInput = true;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	UDebuffComponent* DebuffComponent;
protected:
	/* Protected Functions */

	void InitCharacterMovementComponent();
	void InitCameraStuff();
	void InitMeshAndCapsule();
	void InitNiagraComponents();
	void InitMiscComponents();

	/* Execute Attack */
	void ExecuteAttack(float _AttackSphereRadius);
	
	/* Create a sphere collider which calculates nearest item */
	void CheckForInteractables();
	
	/* Update decal direction */
	void UpdateDecalAngle();

	/* Updates the area of attack indicator */
	void UpdateAOEIndicator();

	/* Billboarding player names + removing current players name */
	void UpdatePlayerNames();

	void SyncCharacterColourWithPlayerState();

	/* Protected variables */
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	/* Display name widget (above head) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* PlayerNameWidgetComponent;

	UPROPERTY()
	class UWidget_PlayerName* PlayerNameWidget{nullptr};
	
private:
	/* Private variables */
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
	UPROPERTY(EditAnywhere, Category = Animation, meta = (AllowPrivateAccess))
	class UAnimMontage* PickupMontage;
	UPROPERTY(EditAnywhere, Category = Animation, meta = (AllowPrivateAccess))
	class UAnimMontage* ExecuteAttackMontage;
	UPROPERTY(EditAnywhere, Category = Animation, meta = (AllowPrivateAccess))
	class UAnimMontage* ExecuteAttackMontage_LongerWindUp;
	
	/* Interact radius for checking closest item */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float InteractRadius = 150.0f;
	
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

	/* When an item is dropped, how hard its launched */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float ItemLaunchStrength = 500000.0f;

	/* Player HUD Prefab */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<class UWidget_PlayerHUD> PlayerHudPrefab;

	/* Friction for when player is on the ice */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float IceFriction{0.1f};

	/* Player regular walk speed */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	float WalkSpeed = 500.f;

	/* Speed for when player is holding gold plant */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	float GoldPlantSpeed = 300.0f;

	/* Speed for when player is sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintSpeed = 750.0f;
	/* How long the player sprints for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTime = 2.0f;
	/* Timer for handling sprint */
	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTimer{};
	/* Stops updating character speed on tick */
	UPROPERTY(Replicated)
	bool bHasSprintingStopped = false;

	/* Rate scales for adjusting animation speed */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = RateScale)
	float WalkRateScale = 1.5f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = RateScale)
	float GoldSlowRateScale = 0.7f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = RateScale)
	float SprintRateScaleScalar = 1.5f;

	/* Attack */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float InstantAttackThreshold = 1.0f;
	UPROPERTY(meta = (AllowPrivateAccess))
	bool bCanAttack = true;

	/* Stun variables */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bIsStunned;	
	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float StunTimer;
	UPROPERTY(Replicated, VisibleAnywhere, meta = (AllowPrivateAccess))
	FTransform LocationWhenStunned{};	
	UPROPERTY(Replicated, VisibleAnywhere, meta = (AllowPrivateAccess))
	FTransform MeshLocationWhenStunned{};

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{nullptr},{nullptr},{nullptr},{nullptr}};

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstanceDynamic*> PlayerMaterialsDynamic{};

public: /* Pubic Networking */
	
	/* RPC for picking up items */
	UFUNCTION(Server, Reliable)
	void Server_PickupItem(APickUpItem* _Item, EPickupActor _PickupType);

	/* RPC for dropping items */
	UFUNCTION(Server, Reliable)
	void Server_DropItem();

	/* RPC for socketing item, used for weapon */
	UFUNCTION(Server, Unreliable)
	void Server_SocketItem(UStaticMeshComponent* _Object, FName _Socket);

	/* Multicast for socketing items, used for weapon*/
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SocketItem(UStaticMeshComponent* _Object, FName _Socket);

	/* RPC for dropping a weapon */
	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();

	/**
	 * @brief Chargeattack sets bIsChargingAttack to true,
	 * Drops item if holding one, and sockets weapon to attacking socket
	 */
	UFUNCTION(Server, Unreliable)
	void Server_ChargeAttack();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SetPlayerAimingMovement(bool _bIsAiming);

	UFUNCTION(Server, Unreliable)
	void Server_CancelChargeAttack();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_CancelChargeAttack();
	
	/* RPC for when attack key is released */
	UFUNCTION(Server, Unreliable)
	void Server_ReleaseAttack();

	/* Multicast for when attack key is released */
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ReleaseAttack();

	/* Adding a HUD for to player */
	UFUNCTION(Server, Unreliable)
	void Server_AddHUD();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_Client_AddHUD();
	UFUNCTION(Client, Unreliable)
	void Client_AddHUD();

	/* Playing Audio */
	UFUNCTION(Server, Unreliable)
	void Server_PlaySoundAtLocation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_PlaySoundAtLocation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);

	/* Ragdoll */
	UFUNCTION(Server, Unreliable)
	void Server_Ragdoll(bool _Ragdoll);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_Ragdoll(bool _Ragdoll);

	/* Teleporting at start and end of game */
	UFUNCTION(Server, Unreliable)
	void Server_TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation);

	/* Toggling particle effects */
	UFUNCTION(Server, Unreliable)
	void Server_ToggleParticleSystems(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ToggleParticleSystems(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off);
	
	/* Mutlicast for dropping a weapon */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropWeapon();

	/* Called from ExecuteAttack() in UWeapon derived class to reset attack variables */
	void ResetAttack();
protected: /* Protected Networking */

	/* Playing animation montages */
	UFUNCTION(Server, Unreliable)
	void Server_PlayNetworkMontage(UAnimMontage* _Montage);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_PlayNetworkMontage(UAnimMontage* _Montage);

	/* Setting player colour*/
	UFUNCTION(Server, Unreliable)
	void Server_SetPlayerColour();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SetPlayerColour();

	/* Sprint RPC */
	UFUNCTION(Server, Reliable)
	void Server_Sprint();

	/* Multicast dropping an item */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropItem();

	/* Multicast for picking up item */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PickupItem(APickUpItem* _Item, EPickupActor _PickupType);

	/* Receiving materials for the farmer costume */
	UFUNCTION(Server, Unreliable)
	void Server_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& _InMaterialsArray);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& _InMaterialsArray);

	/* Potentially unused particle functions */
	UFUNCTION(Server, Unreliable)
	void Server_FireParticleSystem(UNiagaraSystem* _NiagaraSystem, FVector _Position);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_FireParticleSystem(UNiagaraSystem* _NiagaraSystem, FVector _Position);

	/* New way of activating/deactivating particle systems */
	UFUNCTION(Server, Unreliable)
	void Server_SetParticleActive(UNiagaraComponent* _NiagaraComponent, bool _bIsActive);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SetParticleActive(UNiagaraComponent* _NiagaraComponent, bool _bIsActive);

	/* Charging weapon sound control */
	UFUNCTION(Server, Unreliable)
	void Server_ToggleChargeSound(bool _bIsSoundEnabled);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ToggleChargeSound(bool _bIsSoundEnabled);

	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SyncCharacterColour();

public:
	void TeleportToEndGame(FTransform _EndGameTransform);

protected:
	UFUNCTION(Server, Unreliable)
	void Server_TeleportToEndGame(FTransform _EndGameTransform);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_TeleportToEndGame(FTransform _EndGameTransform);



	UPROPERTY(meta = (AllowPrivateAccess))
	bool DoOnce{true};
};