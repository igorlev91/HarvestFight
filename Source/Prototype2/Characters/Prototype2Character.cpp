

#include "Prototype2Character.h"

#include "DebuffComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Pickups/Weapon.h"
#include "Components/SphereComponent.h"
#include "DynamicMesh/ColliderMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prototype2/Widgets/Widget_InteractionPanel.h"
#include "Prototype2/Widgets/Widget_IngameMenu.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Widgets/Widget_PlayerHUD.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/Image.h"
#include "Components/WidgetComponent.h"
#include "Engine/Internal/Kismet/BlueprintTypeConversions.h"
#include "Prototype2/Pickups/GrowableWeapon.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Gameplay/LaunchPad.h"
#include "Prototype2/Pickups/Weapons/WeaponLeekSword.h"
#include "Prototype2/Pickups/Weapons/WeaponPunching.h"
#include "Prototype2/Spawning/FertiliserSpawner.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/VFX/VFXComponent.h"
#include "Prototype2/Gameplay/Smite.h"
#include "Prototype2/Gameplay/SmiteManager.h"
#include "Prototype2/GameUserSettings/HarvestHavocGameUserSettings.h"
#include "Prototype2/Widgets/Widget_GameOptions.h"
#include "Prototype2/Widgets/Widget_PlayerEmote.h"
#include "Prototype2/Widgets/Widget_PlayerName.h"

APrototype2Character::APrototype2Character()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	InitCharacterMovementComponent();
	InitCameraStuff();
	VFXComponent = CreateDefaultSubobject<UVFXComponent>(TEXT("VFX Component"));
	InitMiscComponents();
	InitNiagraComponents();
}

void APrototype2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2Character, Weapon);
	DOREPLIFETIME(APrototype2Character, HeldItem);
	DOREPLIFETIME(APrototype2Character, bChargeAnimationState);
	//DOREPLIFETIME(APrototype2Character, WeaponCurrentDurability);
	//DOREPLIFETIME(APrototype2Character, bIsHoldingGold);
	//DOREPLIFETIME(APrototype2Character, InteractTimer);
	DOREPLIFETIME(APrototype2Character, ClaimedPlot);
	DOREPLIFETIME(APrototype2Character, DebuffComponent);
	//DOREPLIFETIME(APrototype2Character, bAllowMovementFromInput);
	//DOREPLIFETIME(APrototype2Character, FallTimer);
	DOREPLIFETIME(APrototype2Character, CurrentWeaponSeedData);
	DOREPLIFETIME(APrototype2Character, CurrentWeaponAnimation);
	DOREPLIFETIME(APrototype2Character, bIsHoldingGoldWeapon);
//	DOREPLIFETIME(APrototype2Character, bIsHoldingInteract);
	//DOREPLIFETIME(APrototype2Character, WeaponFlashTimer);
	//DOREPLIFETIME(APrototype2Character, bShouldWeaponFlashRed);
	DOREPLIFETIME(APrototype2Character, WeaponMesh);
	DOREPLIFETIME(APrototype2Character, bSprintAnimationState);
	
	// Sprint
	// DOREPLIFETIME(APrototype2Character, bHasNotifiedCanSprint);
	// DOREPLIFETIME(APrototype2Character, SprintTimer);
	// DOREPLIFETIME(APrototype2Character, bSprinting);
	// DOREPLIFETIME(APrototype2Character, bCanSprint);
	// DOREPLIFETIME(APrototype2Character, CurrentMaxWalkSpeed);
	// DOREPLIFETIME(APrototype2Character, DelayedSprintRegenTimer);
	
	//// Niagara Components
	//DOREPLIFETIME(APrototype2Character, AttackAreaIndicatorMesh);
	//DOREPLIFETIME(APrototype2Character, Dizzy_NiagaraComponent);
	////DOREPLIFETIME(APrototype2Character, WalkPoof_NiagaraComponent);
	//DOREPLIFETIME(APrototype2Character, SprintPoof_NiagaraComponent);
	//DOREPLIFETIME(APrototype2Character, Sweat_NiagaraComponent);
	//DOREPLIFETIME(APrototype2Character, AttackTrail_NiagaraComponent);
	//DOREPLIFETIME(APrototype2Character, Attack_NiagaraComponent);
	//DOREPLIFETIME(APrototype2Character, Smite_NiagaraComponent);
	//DOREPLIFETIME(APrototype2Character, SmiteShockWave_NiagaraComponent);
	//DOREPLIFETIME(APrototype2Character, SmiteElectrifyWarning_NiagaraComponent);
}

void APrototype2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	AssignHelperReferences();
	InitInputMappingContext();
	//InitPlayerHUD();
	InitDecals();
	InitMeshAndCapsule();
	InitWeapon();
	InitShippingBin();
	SetFOV();
	// Setup FOV after HUD has been created
	if (IsValid(PlayerHUDRef) && IsValid(PlayerHUDRef->OptionsMenu))
	{
		PlayerHUDRef->OptionsMenu->OnFOVChangedDelegate.AddDynamic(this, &APrototype2Character::SetFOV);
	}

}

void APrototype2Character::DelayedBeginPlay()
{
	if (!IsValid(PlayerStateRef))
		PlayerStateRef = GetPlayerState<APrototype2PlayerState>();

	if (PlayerMaterialsDynamic.Num() <= 0)
	{
		for(UMaterialInstance* Material : PlayerMaterials)
		{
			PlayerMaterialsDynamic.Add(UMaterialInstanceDynamic::Create(Material,this));
		}
	}
	InitPlayerNameWidgetComponent();
	InitEmoteWidgetComponent();
	SyncCharacterColourWithPlayerState();
}

void APrototype2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrototype2Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrototype2Character::Look);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APrototype2Character::Sprint);
		EnhancedInputComponent->BindAction(EndSprintAction, ETriggerEvent::Triggered, this, &APrototype2Character::EndSprint);

		// Attack
		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ChargeAttack);
		EnhancedInputComponent->BindAction(ReleaseAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ReleaseAttack);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APrototype2Character::Interact, false);
		//EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APrototype2Character::HoldInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APrototype2Character::ReleaseInteract);
		
		// Hold Interact
		//EnhancedInputComponent->BindAction(HoldInteractAction, ETriggerEvent::Ongoing, this, &APrototype2Character::HoldInteract);
		//EnhancedInputComponent->BindAction(ReleaseHoldInteractAction, ETriggerEvent::Triggered, this, &APrototype2Character::ReleaseHoldInteract);
		
		// UI
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this, &APrototype2Character::OpenIngameMenu);
		EnhancedInputComponent->BindAction(ShowEmoteAction, ETriggerEvent::Triggered, this, &APrototype2Character::ShowRadialEmoteMenu);
		EnhancedInputComponent->BindAction(DisableEmoteAction, ETriggerEvent::Triggered, this, &APrototype2Character::DisableRadialEmoteMenu);

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APrototype2Character::Jump);
	}
}

void APrototype2Character::SetFOV()
{
	// Setup FOV after HUD has been created
	if (IsValid(PlayerHUDRef) && IsValid(PlayerHUDRef->OptionsMenu))
	{
		FollowCamera->SetFieldOfView(PlayerHUDRef->OptionsMenu->TempFOV);
	}
}

void APrototype2Character::AssignHelperReferences()
{
	PlayerController = Cast<APrototype2PlayerController>(Controller);
	GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
}

void APrototype2Character::InitInputMappingContext()
{
	if (IsValid(PlayerController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -100.0f; // lowest point looking up
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 100.0f; // Highest point looking down
		}
	}

	//GetCharacterMovement()->SetIsReplicated(true);
}

void APrototype2Character::Multi_SetSmite(USmite* _smite)
{
	smite = _smite;
	smite->SetPlayer(this);
}

void APrototype2Character::Multi_RemoveSmite()
{
	smite->bPauseTimer = true;
}

void APrototype2Character::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);
	DelayedBeginPlay();
	
	TickTimers(_DeltaSeconds);
	UpdateParticleSystems();
	
	ClearInteractionText();
	CheckForInteractables();
	
	CheckForFloorSurface();
	CheckForFalling(_DeltaSeconds);
	
	UpdateWeaponMeshSkin();
	HandleAttackChargeBehavior(_DeltaSeconds);

	TogglePlayerStencil();
	UpdatePlayerNames();
	UpdateEmote();
	UpdatePlayerHUD();

	FollowThroughOnAttack();
	
	//ToggleParticleSystems();
	
	if (IsValid(smite))
	{
		smite->Tick(_DeltaSeconds);
	}
}

void APrototype2Character::TogglePlayerStencil()
{
	APrototype2PlayerState* MyPlayerState = GetPlayerState<APrototype2PlayerState>();
	if (!IsValid(MyPlayerState))
		return;
	
	auto HHGameGameUserSettings = UHarvestHavocGameUserSettings::GetHarvestHavocGameUserSettings();

	if (IsLocallyControlled() == true)
	{
		GetMesh()->SetRenderCustomDepth(HHGameGameUserSettings->bPlayerStencil);
		GetMesh()->SetCustomDepthStencilValue((uint8_t)MyPlayerState->Details.Colour + 2);

		// SET ALL ENEMIES RED //
		for (APrototype2PlayerState* SomePlayerState : GameState->Server_Players)
		{
			if (MyPlayerState && SomePlayerState && SomePlayerState != MyPlayerState)
			{
				if (auto SomePawn = SomePlayerState->GetPawn())
				{
					if (auto SomeCharacter = Cast<APrototype2Character>(SomePawn))
					{
						if (SomePlayerState->Details.Colour != MyPlayerState->Details.Colour)
						{
							if (HHGameGameUserSettings->bEnemyAlwaysRed == true)
							{
								SomeCharacter->GetMesh()->SetRenderCustomDepth(HHGameGameUserSettings->bEnemyAlwaysRed);
								SomeCharacter->GetMesh()->SetCustomDepthStencilValue(2);
							}
							else
							{
								SomeCharacter->GetMesh()->SetRenderCustomDepth(HHGameGameUserSettings->bPlayerStencil);
								SomeCharacter->GetMesh()->SetCustomDepthStencilValue((uint8_t)SomePlayerState->Details.Colour + 2);
							}
						}
						else
						{
							SomeCharacter->GetMesh()->SetRenderCustomDepth(HHGameGameUserSettings->bPlayerStencil);
							SomeCharacter->GetMesh()->SetCustomDepthStencilValue((uint8_t)SomePlayerState->Details.Colour + 2);
						}
					}
				}
			}
		}
	}
}


void APrototype2Character::Move(const FInputActionValue& _Value)
{
	const FVector2D MovementVector = _Value.Get<FVector2D>();
	//if (!Controller || bIsHoldingInteract)
	//	return;

	if (!bAllowMovementFromInput && GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)// && Grounded
		return;
	
	if (DebuffComponent->CurrentDebuff == EDebuff::Daze || DebuffComponent->CurrentDebuff == EDebuff::Stun)
		return;
	
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APrototype2Character::Look(const FInputActionValue& _Value)
{
	const FVector2D LookAxisVector = _Value.Get<FVector2D>();
	if (!IsValid(Controller))
		return;
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APrototype2Character::Sprint()
{
	if (!HasIdealRole())
		return;
	
	// If the player used all their stamina to the bottom it has to fully reset
	if (!bCanSprint)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Reason for no sprint: bCanSprint is false");
		OnFailedToSprintDelegate.Broadcast();
		return;
	}

	// Fixes crash when trying to sprint while slowed with debuff
	if (!IsValid(DebuffComponent))
		return;
	
	// If no debuff or sprint is at least 1/5th full
	if (DebuffComponent->CurrentDebuff == EDebuff::None && SprintTimer > (SprintTime / 5.0f))
	{
		bSprinting = true;		
		bHasNotifiedCanSprint = false;
		RefreshCurrentMaxSpeed();
		Server_SetSprintState(true);
	}
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Reason for no sprint: this check was false->>>> if(DebuffComponent->CurrentDebuff == EDebuff::None && SprintTimer > (SprintTime / 5.0f)");
		OnFailedToSprintDelegate.Broadcast();
	}
}

void APrototype2Character::EndSprint()
{
	if (!HasIdealRole())
		return;

	// end sprint
	bSprinting = false;
	bHasNotifiedCanSprint = false;
	Server_SetSprintState(false);
	
	if (bCanSprint)
	{
		DelayedSprintRegenTimer = 0.0f;
	}

	// Only apply the speed if no debuff, incase the player "stops sprinting" with a slow debuff that will reset their speed
	if (DebuffComponent->CurrentDebuff == EDebuff::None)
	{
		RefreshCurrentMaxSpeed();
	}
}

void APrototype2Character::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		PlaySoundAtLocation(GetActorLocation(), JumpCue);
	}	
	Super::Jump();
}

void APrototype2Character::ChargeAttack()
{
	if (!IsValid(Weapon))
		return;
	
	//if (bIsHoldingInteract)
	//	return;

	if (bIsChargingAttack)
	{
		return;
	}

	if (!bCanAttack)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Charging Attack");
		return;
	}

	if (DebuffComponent->CurrentDebuff == EDebuff::Daze || DebuffComponent->CurrentDebuff == EDebuff::Stun)
		return;

	// for throwing item
	if (IsValid(HeldItem))
		return;
	
	//DropItem();

	if (AttackTimer <= 0.0f)
	{
		AttackTimer = AttackTimerTime;
		
		Weapon->Client_ChargeAttack(this);
		
		bIsChargingAttack = true;
		AttackAreaIndicatorMesh->SetHiddenInGame(false);

		if (HasAuthority())
		{
			Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
			
			// Charge animation
			bChargeAnimationState = true;
			OnRep_ChargeStateChanged();
			
			Weapon->ChargeAttack(this);
		}
		else
		{
			Server_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
			Server_ChargeAttack();
		}
		PlayWeaponSound(ChargeCue);
	}
}

void APrototype2Character::CancelChargeAttack()
{
	Server_CancelChargeAttack();
}

void APrototype2Character::ReleaseAttack()
{
	// Throw item functionality
	if (IsValid(HeldItem))
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle,
			FTimerDelegate::CreateLambda(
				[this]
				{
					ThrowItem();
				}), InstantAttackDelay, false);

		if (AnimationData->NormalPunchingAttack)
		{
			PlayNetworkMontage(AnimationData->NormalPunchingAttack);
			UKismetSystemLibrary::PrintString(GetWorld(), "Threw item");
		}
		return;
	}

	if (!IsValid(Weapon))
		return;

	if (!bIsChargingAttack || !bCanAttack)
	{
		return;
	}
	
	bCanAttack = false;
	
	if (CurrentWeaponSeedData->WeaponData != DefaultWeaponSeedData->WeaponData)
	{
		// VFX
		ActivateParticleSystemFromEnum(EParticleSystems::AttackTrail);
	}

	if (AttackChargeAmount >= MaxAttackCharge) // InstantAttackThreshold
	{
		SetPlayerAimingMovement(false);
	}

	if (AttackChargeAmount > 0.5f || GetVelocity().Length() > 10.0f)
	{
		bIsFollowingThroughAttack = true;
	}

	// Make sure weapon is socketed to hand
	if (HasAuthority())
	{
		Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
	}
	else
	{
		Server_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
	}

	// Set the radius of the sphere for attack
	int32 AttackSphereRadius = CurrentWeaponSeedData->WeaponData->BaseAttackRadius + AttackChargeAmount * CurrentWeaponSeedData->WeaponData->AOEMultiplier;
	
	// Always delay attack because of the new launch
	bool bFullChargeAttack = true;
	if (AttackChargeAmount < MaxAttackCharge) // InstantAttackThreshold
		{
		bFullChargeAttack = false;
		}

	// Play the correct attack montage according to charge and weapon type equipped
	Weapon->ReleaseAttack(bFullChargeAttack, this);

	// Make sure attack charge isn't over max
	if (AttackChargeAmount > MaxAttackCharge)
	{
		AttackChargeAmount = MaxAttackCharge;
	}
	float InAttackCharge = AttackChargeAmount;
	bool InSprinting = bSprinting;
	
	// Delayed attack
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda(
			[this, AttackSphereRadius, InAttackCharge, InSprinting]
			{
				
				SetPlayerAimingMovement(false);
				ExecuteAttack(AttackSphereRadius, InAttackCharge, InSprinting);
				ResetAttack();
			}), InstantAttackDelay, false);
}

void APrototype2Character::Interact(bool _Direct)
{
	if (!IsValid(PlayerStateRef))
		return;

	if (ClosestInteractableItem == nullptr)
		return;
	
	if (DebuffComponent->DebuffInfo.Debuff == EDebuff::Daze ||
		DebuffComponent->DebuffInfo.Debuff == EDebuff::Stun)
		return;
	
	if (bIsChargingAttack)
		return;

	if (InteractTimer > 0)
		return;

	if (_Direct)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "Attempted To Interact (DIRECT)");
		ClosestInteractableItem->ClientInteract(this);
		InteractTimer = InteractTimerTime;
		Server_Interact(ClosestInteractableActor);
		EnableStencil(false);
		ClosestInteractableActor = nullptr;
		ClosestInteractableItem = nullptr;
		RefreshCurrentMaxSpeed();
	}
	else
	{
		switch (ClosestInteractableItem->IsInteractable(PlayerStateRef))
		{
		case HOLD:
			{
				UKismetSystemLibrary::PrintString(GetWorld(), "Attempted To Hold Interact");
				InteractTimeline->SetPlayRate(1.0f / ClosestInteractableItem->HoldDuration);
				bAllowMovementFromInput = false;
				InteractTimeline->PlayFromStart();
				break;
			}
		case INSTANT:
			{
				UKismetSystemLibrary::PrintString(GetWorld(), "Attempted To Interact");
				ClosestInteractableItem->ClientInteract(this);
				InteractTimer = InteractTimerTime;
				Server_Interact(ClosestInteractableActor);
				EnableStencil(false);
				ClosestInteractableActor = nullptr;
				ClosestInteractableItem = nullptr;
				RefreshCurrentMaxSpeed();
				break;
			}
		default:
			{
				break;
			}
		}
	}
}

void APrototype2Character::ReleaseInteract()
{
	InteractTimeline->SetPlaybackPosition(0.0f, false, false);
	InteractTimeline->Stop();
	OnStoppedClaimingPlotDelegate.Broadcast();
	
	if (DebuffComponent->DebuffInfo.Debuff == EDebuff::Daze ||
		DebuffComponent->DebuffInfo.Debuff == EDebuff::Stun)
			return;

	bAllowMovementFromInput = true;
}

void APrototype2Character::Server_Interact_Implementation(AActor* _Item)
{

	
	Cast<IInteractInterface>(_Item)->Interact(this);
		
	if (IsValid(HeldItem))
	{
		if (AnimationData->Pickup)
		{
			PlayNetworkMontage(AnimationData->Pickup);
		}
		if (IsValid(Weapon))
		{
			Multi_SocketItem(WeaponMesh, FName("WeaponHolsterSocket"));
		}
	}
}

void APrototype2Character::OpenIngameMenu()
{
	if (!IsValid(PlayerHUDRef))
		return;

	PlayerHUDRef->EnableDisableMenu();
}

void APrototype2Character::ShowRadialEmoteMenu()
{
	if (!IsValid(PlayerHUDRef))
		return;

	PlayerHUDRef->ShowEmoteRadialMenu();
}

void APrototype2Character::DisableRadialEmoteMenu()
{
	if (!IsValid(PlayerHUDRef))
		return;

	PlayerHUDRef->DisableEmoteRadialMenu();
}

void APrototype2Character::ExecuteAttack(float _AttackSphereRadius, float _AttackChargeAmount, bool _bSprinting)
{
	Weapon->ExecuteAttack(_AttackSphereRadius, this, _AttackChargeAmount, _bSprinting);
	 // Can't pickup for short duration
	InteractTimer = InteractTimerTime;
	//Server_ExecuteAttack(_AttackSphereRadius, _AttackChargeAmount, _bSprinting);
}

void APrototype2Character::Server_ExecuteAttack_Implementation(float _AttackSphereRadius, float _AttackChargeAmount, bool _bSprinting)
{

	
	//Weapon->ExecuteAttack(_AttackSphereRadius, this, _AttackChargeAmount, _bSprinting);
}

void APrototype2Character::DropItem(float WhoopsyStrength)
{
	if (IsValid(HeldItem))
	{
		// Local Stuff
		if (IsValid(PlayerHUDRef))
		{
			PlayerHUDRef->ClearPickupUI();
			PlayerHUDRef->SetHUDInteractText("");
		}
		HeldItem->Client_Drop();
		
		if (HasAuthority())
		{
			// Server_DropItem
			HeldItem->Server_Drop();
			
			if (DropCue)
				PlaySoundAtLocation(GetActorLocation(), DropCue);
			if (WeaponMesh)
				Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
			
			Multi_DropItem();
			HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);

			FVector RandomWhoopsyDirection{(float)(rand() % 3) - 1.0f, (float)(rand() % 3) - 1.0f, 1.0f};
			HeldItem->ItemComponent->Mesh->AddImpulse(RandomWhoopsyDirection.GetSafeNormal() * WhoopsyStrength, NAME_None, true);
			
			HeldItem = nullptr;
			//
		}
		else
		{
			Server_DropItem(WhoopsyStrength);
		}
		RefreshCurrentMaxSpeed();
	}
	else
	{
		HeldItem = nullptr;
	}
}

void APrototype2Character::CheckForInteractables()
{
	if (!IsLocallyControlled())
		return;

	/* IF HOLDING */
	if (InteractTimeline->IsPlaying())
		return;
	
	//if (bIsHoldingInteract)
	//	return;
	
	if (InteractTimer > InteractTimerTime / 2.0f || bIsChargingAttack)
	{
		EnableStencil(false);
		if (ClosestInteractableItem)
			ClosestInteractableItem->OnClientWalkAway(this);
		ClosestInteractableActor = nullptr;
		ClosestInteractableItem = nullptr;
		return;
	}
	
	TArray<FHitResult> OutHits;
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(InteractRadius * 1.5f);
	const bool HasFoundCloseActor = GetWorld()->SweepMultiByChannel(OutHits, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_Visibility, CollisionSphere);
	if (!HasFoundCloseActor)
	{
		EnableStencil(false);
		if (ClosestInteractableItem)
			ClosestInteractableItem->OnClientWalkAway(this);
		ClosestInteractableActor = nullptr;
		ClosestInteractableItem = nullptr;
		return;
	}

	TArray<FHitResult> InteractableHitResults;
	TArray<AActor*> InteractableActors;
	for (FHitResult& HitResult : OutHits)
	{
		if (IInteractInterface* InteractInterface = Cast<IInteractInterface>(HitResult.GetActor()))
		{
			if (InteractInterface->IsInteractable(PlayerStateRef))
			{
				InteractableHitResults.Add(HitResult);
				InteractableActors.Add(HitResult.GetActor());
			}
		}
	}

	TArray<FHitResult> ImportantHitResults;
	TArray<AActor*> ImportantActors;
	TArray<FHitResult> ExtraImportantHitResults;
	TArray<AActor*> ExtraImportantActors;
	for(int i = 0; i < InteractableActors.Num(); i++)
	{
		if (AGrowSpot* SomeGrowSpot = Cast<AGrowSpot>(InteractableActors[i]))
		{
			if (ABeehive* SomeBeeHive = Cast<ABeehive>(SomeGrowSpot->ItemRef))
			{
				ExtraImportantActors.Add(InteractableActors[i]);
				ExtraImportantHitResults.Add(InteractableHitResults[i]);
			}
			else
			{
				ImportantActors.Add(InteractableActors[i]);
				ImportantHitResults.Add(InteractableHitResults[i]);
			}
		}
		else if (Cast<ASellBin>(InteractableActors[i])
			|| Cast<AFertiliserSpawner>(InteractableActors[i]))
		{
			ImportantActors.Add(InteractableActors[i]);
			ImportantHitResults.Add(InteractableHitResults[i]);
		}
	}
	
	if (ExtraImportantActors.Num() > 0)
	{
		InteractableActors = ExtraImportantActors;
		InteractableHitResults = ExtraImportantHitResults;
	}
	else if (ImportantActors.Num() > 0)
	{
		InteractableActors = ImportantActors;
		InteractableHitResults = ImportantHitResults;
	}
	
	float DistanceToClosest;
	AActor* NearestActor = UGameplayStatics::FindNearestActor(GetActorLocation(), InteractableActors, DistanceToClosest);
	int32 NearestActorIndex = InteractableActors.Find(NearestActor);
	if (!NearestActor)
	{
		EnableStencil(false);
		if (ClosestInteractableItem)
			ClosestInteractableItem->OnClientWalkAway(this);
		ClosestInteractableItem = nullptr;
		ClosestInteractableActor = nullptr;
		return;
	}
	else if (NearestActor && FVector::Distance(InteractableHitResults[NearestActorIndex].TraceEnd, GetActorLocation()) <= InteractRadius)
	{
		if (ClosestInteractableActor && ClosestInteractableActor != NearestActor)
		{
			EnableStencil(false);
			if (ClosestInteractableItem)
				ClosestInteractableItem->OnClientWalkAway(this);
		}
		ClosestInteractableActor = NearestActor;
		ClosestInteractableItem = Cast<IInteractInterface>(NearestActor);
		return;
	}

	EnableStencil(false);
	if (ClosestInteractableItem)
		ClosestInteractableItem->OnClientWalkAway(this);
	ClosestInteractableItem = nullptr;
	ClosestInteractableActor = nullptr;
}

void APrototype2Character::EnableStencil(bool _bIsOn)
{
	if (IsValid(HeldItem))
	{
		HeldItem->ItemComponent->Mesh->SetRenderCustomDepth(_bIsOn);
	}
	
	if (!ClosestInteractableActor)
	{
		return;
	}
	if (IsValid(HeldItem))
	{
		HeldItem->ItemComponent->Mesh->SetRenderCustomDepth(false);
	}
	// Swet all meshes
	TArray<UItemComponent*> AllItemComponents{};
	ClosestInteractableActor->GetComponents<UItemComponent>(AllItemComponents, true);
	for (int i = 0; i < AllItemComponents.Num(); i++)
	{
		AllItemComponents[i]->Mesh->SetRenderCustomDepth(_bIsOn);
	}

	if (auto Growspot = Cast<AGrowSpot>(ClosestInteractableActor))
	{
		Growspot->Mesh->SetRenderCustomDepth(_bIsOn);
	}
}

void APrototype2Character::GetHit(float _AttackCharge, FVector _AttackerLocation, UWeaponData* _OtherWeaponData)
{
	if (HasAuthority())
	{
		if (IsLocallyControlled())
			ReleaseInteract();
		
		CalculateAndApplyHit(_AttackCharge, _AttackerLocation, _OtherWeaponData);
	}
	else
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "NON AUTHORITY GetHit");		
	}	
}

void APrototype2Character::CalculateAndApplyHit(float _AttackCharge, FVector _AttackerLocation,
                                                UWeaponData* _OtherWeaponData)
{
	if (!HasAuthority())
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "NON AUTHORITY CalculateAndApplyHit");
	}

	// cancel any holding interact functionionality
	//bIsHoldingInteract = false;
	
	// Invincible after getting hit
	if (InvincibilityTimer > 0.0f)
	{
		return;
	}
	InvincibilityTimer = InvincibilityDuration;
	
	if (IsValid(DebuffComponent))
	{
		if (DebuffComponent->CurrentDebuff == EDebuff::None)
		{
			// Create debuff info and assing new values
			FDebuffInfo NewDebuffInfo;
			NewDebuffInfo.Debuff = _OtherWeaponData->Debuff;
			if (_AttackCharge > MaxAttackCharge)
			{
				NewDebuffInfo.Duration = MaxAttackCharge;
			}
			else
			{
				NewDebuffInfo.Duration = _AttackCharge;
			}

			// Replicate and apply new debuff info
			DebuffComponent->DebuffInfo = NewDebuffInfo;
			DebuffComponent->OnRep_ApplyDebuff();			
		}
	}
	
	// Knockback
	FVector KnockAway = (GetActorUpVector() * _OtherWeaponData->KnockUpMultiplier) + (GetActorLocation() - _AttackerLocation).GetSafeNormal();
	
	// Set minimum attack charge for scaling knockback
	if (_AttackCharge < 1.0f)
	{
		_AttackCharge = 1.0f;
	}
	
	KnockAway *= _AttackCharge * _OtherWeaponData->KnockbackMultiplier;
	
	// Limit the knockback to MaxKnockBackVelocity
	if (KnockAway.Size() > _OtherWeaponData->MaxKnockback) 
	{
		KnockAway = KnockAway.GetSafeNormal() * _OtherWeaponData->MaxKnockback; 
	}

	// Knock this player away
	if (GetCharacterMovement()->IsFalling())
	{
		GetCharacterMovement()->Launch(KnockAway * 0.75f);
	}
	else
	{
		GetCharacterMovement()->Launch(KnockAway);
	}
	
	// Drop item
	DropItem(1000.0f);
	
	PlayWeaponSound(GetHitCue);
	
	// VFX
	FVector AttackVFXLocation = _AttackerLocation - GetActorLocation();
	AttackVFXLocation = AttackVFXLocation.GetSafeNormal();
	AttackVFXLocation *= 50.0f;
	AttackVFXLocation += GetActorLocation();
	//Attack_NiagaraComponent->SetWorldLocation(AttackVFXLocation);

	ActivateParticleSystemFromEnum(EParticleSystems::Attack);

	if (HasAuthority())
	{
		Multi_CancelChargeAttack();
	}
	else
	{
		CancelChargeAttack();
	}
}

void APrototype2Character::Server_HitWinterSellBin_Implementation(ASellBin_Winter* _HitWinterSellBin, float _AttackCharge,
                                                                  float _MaxAttackCharge, FVector _AttackerLocation)
{
	_HitWinterSellBin->GetHit(_AttackCharge, MaxAttackCharge, _AttackerLocation);
}

void APrototype2Character::Server_CheckIfCrownHit_Implementation(APrototype2Character* _HitPlayer)
{
	if (_HitPlayer->GetHasCrown())
	{
		int32 PointsForHit = static_cast<int32>(AttackChargeAmount);
		if (PointsForHit < 1)
		{
			PointsForHit = 1;
		}
		PlayerStateRef->AddCoins(PointsForHit);
		_HitPlayer->PlaySoundAtLocation(_HitPlayer->GetActorLocation(), _HitPlayer->SellCue);
	}
}

void APrototype2Character::Server_HitPlayer_Implementation(APrototype2Character* _HitPlayer, float _AttackCharge,
                                                           FVector _AttackerLocation, UWeaponData* _OtherWeaponData)
{
	_HitPlayer->GetHit(_AttackCharge, _AttackerLocation, _OtherWeaponData);
}

void APrototype2Character::Server_HitConcrete_Implementation(AGrowSpot* _HitGrowSpot)
{
	if (_HitGrowSpot->DegradeConcrete())
	{
		PlaySoundAtLocation(GetActorLocation(), HitConcreteCue);
	}
}

void APrototype2Character::InitiateSmite(float _AttackCharge, UWeaponData* _OtherWeaponData)
{
	// Init values
	SmiteWeaponData = _OtherWeaponData;
	SmiteKnockBack = _AttackCharge;

	// VFX
	SmiteCloud->SetVisibility(true);
	ActivateParticleSystemFromEnum(EParticleSystems::SmiteElectrifyWarning);
	PlaySoundAtLocation(GetActorLocation(), SmiteCue, AltarAttenuationSettings);
	
	// Start timer that fires this stuff when done
	GetWorld()->GetTimerManager().SetTimer(SmiterTimer, this, &APrototype2Character::GetSmited, SmiteWarningTime);
}

void APrototype2Character::GetSmited()
{
	// VFX
	SmiteCloud->SetVisibility(false);
	DeActivateParticleSystemFromEnum(EParticleSystems::SmiteElectrifyWarning);
	ActivateParticleSystemFromEnum(EParticleSystems::SmiteStaticLightning);
	
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Walking)
	{
		ActivateParticleSystemFromEnum(EParticleSystems::Smite);
	}
	else
	{
		ActivateParticleSystemFromEnum(EParticleSystems::SmiteShockWave);
	}

	// Smite
	FVector HitLocation = GetActorLocation();
	float CircleRadius = 1000.0f;
	float RandomDegrees =  FMath::RandRange(0.0f, 360.0f);
	HitLocation.X = CircleRadius * FMath::Cos(RandomDegrees) + GetActorLocation().X;
	HitLocation.Y = CircleRadius * FMath::Sin(RandomDegrees) + GetActorLocation().Y;
	
	if (IsValid(SmiteWeaponData))
	{
		GetHit(SmiteKnockBack, HitLocation, SmiteWeaponData);
	}
	
	UKismetSystemLibrary::PrintString(GetWorld(), "GetSmited noobs");
}

void APrototype2Character::Multi_SocketItem_Implementation(UStaticMeshComponent* _Object, FName _Socket)
{
	_Object->SetSimulatePhysics(false);
	_Object->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_Object->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	_Object->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(_Socket));
}

void APrototype2Character::UpdateCanSprintUI()
{
	if (!bHasNotifiedCanSprint)
	{
		DeActivateParticleSystemFromEnum(EParticleSystems::Sweat);
		OnSprintReadyDelegate.Broadcast();
	}
}

void APrototype2Character::InitCharacterMovementComponent()
{
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	//GetCharacterMovement()->SetIsReplicated(true);
}

void APrototype2Character::InitCameraStuff()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 700.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeLocation({0.0f, 0.0f, 80.0f});
}

void APrototype2Character::InitMeshAndCapsule()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// Set collisions
	//GetMesh()->SetIsReplicated(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void APrototype2Character::InitNiagraComponents()
{
	// Niagara Components
	Dizzy_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Dizzy Component"));
	Dizzy_NiagaraComponent->SetupAttachment(GetCapsuleComponent());
	WalkPoof_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WalkPoof Component"));
	WalkPoof_NiagaraComponent->SetupAttachment(RootComponent);
	SprintPoof_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SprintPoof Component"));
	SprintPoof_NiagaraComponent->SetupAttachment(RootComponent);
	Sweat_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Sweat Component"));
	Sweat_NiagaraComponent->SetupAttachment(RootComponent);
	AttackTrail_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AttackTrail Component"));
	AttackTrail_NiagaraComponent->SetupAttachment(WeaponMesh);
	Attack_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Attack Component"));
	Attack_NiagaraComponent->SetupAttachment(RootComponent);
	Smite_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Smite Component"));
	Smite_NiagaraComponent->SetupAttachment(RootComponent);
	SmiteShockWave_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmiteShockWave Component"));
	SmiteShockWave_NiagaraComponent->SetupAttachment(RootComponent);
	SmiteShockWaveOnly_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmiteShockWaveOnly Component"));
	SmiteShockWaveOnly_NiagaraComponent->SetupAttachment(RootComponent);
	SmiteElectrifyWarning_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmiteElectrifyWarning Component"));
	SmiteElectrifyWarning_NiagaraComponent->SetupAttachment(RootComponent);
	SmiteStaticLightning_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmiteStaticLightning Component"));
	SmiteStaticLightning_NiagaraComponent->SetupAttachment(RootComponent);
	AssertDominance_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AssertDominance Component"));
	AssertDominance_NiagaraComponent->SetupAttachment(RootComponent);
	Slow_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Slow Component"));
	Slow_NiagaraComponent->SetupAttachment(RootComponent);
}

void APrototype2Character::InitMiscComponents()
{
	// Weapon
	Weapon = CreateDefaultSubobject<UWeaponPunching>(TEXT("Weapon"));
	
	// Weapon Mesh
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	WeaponMesh->SetIsReplicated(true);
	

	// Area of attack indicator mesh set up

	AttackAreaIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackAreaIndicatorMesh"));
	AttackAreaIndicatorMesh->SetupAttachment(RootComponent);
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	AttackAreaIndicatorMesh->SetHiddenInGame(false);
	
	// SFX
	WeaponAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponAudioComponent"));
	WeaponAudioComponent->SetupAttachment(RootComponent);

	// Decal component
	DecalArmSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DecalArrowArm"));
	DecalArmSceneComponent->SetupAttachment(RootComponent);
	DecalArmSceneComponent->SetIsReplicated(false);
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalArrow"));
	DecalComponent->AttachToComponent(DecalArmSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DecalComponent->SetIsReplicated(false);

	/* Display name widget (above head) */
	PlayerNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameWidgetComponent"));
	PlayerNameWidgetComponent->SetupAttachment(RootComponent);
	PlayerNameWidgetComponent->SetIsReplicated(false);
	PlayerNameWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Emote widget (above head) */
	EmoteWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EmoteWidgetComponent"));
	EmoteWidgetComponent->SetupAttachment(RootComponent);
	EmoteWidgetComponent->SetIsReplicated(false);
	EmoteWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Debuff component
	DebuffComponent = CreateDefaultSubobject<UDebuffComponent>("DebuffComponent");
	//DebuffComponent->SetIsReplicated(true);

	SmiteCloud = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmiteCloud"));
	SmiteCloud->SetupAttachment(RootComponent);
	SmiteCloud->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SmiteCloud->SetVisibility(false);

	InteractTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Interaction Timeline"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> LinearCurve(TEXT("/Game/Curves/LinearCurve"));
	if (LinearCurve.Object != nullptr)
	{
		FOnTimelineFloat TimelineCallback{};
		TimelineCallback.BindDynamic(this, &APrototype2Character::OnUpdate_InteractTimeline);
		InteractTimeline->AddInterpFloat(LinearCurve.Object, TimelineCallback);
		FOnTimelineEvent FinishedCallback{};
		FinishedCallback.BindDynamic(this, &APrototype2Character::OnFinish_InteractTimeline);
		InteractTimeline->SetTimelineFinishedFunc(FinishedCallback);
	}
}

void APrototype2Character::UpdateDecalAngle()
{
	if (!IsValid(SellBin))
		return;

	const FVector CurrentPlayerPosition = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
	const FVector SellPosition = FVector(SellBin->GetActorLocation().X, SellBin->GetActorLocation().Y, 0);

	DecalComponent->SetVisibility(false);
	if (!IsValid(HeldItem))
		return;

	if (Cast<APlant>(HeldItem))
		DecalComponent->SetVisibility(true);
}

void APrototype2Character::OnRep_UpdateAOE()
{
	if (AttackChargeAmount > MaxAttackCharge)
	{
		AttackChargeAmount = MaxAttackCharge;
	}
	if (IsValid(Weapon))
		Weapon->UpdateAOEIndicator(this, AttackChargeAmount);
}

void APrototype2Character::PlayEmote(EEmote _Emote)
{
	if (HasAuthority())
		Multi_PlayEmote(_Emote);
	else
		Server_PlayEmote(_Emote);
}

void APrototype2Character::Server_PlayEmote_Implementation(EEmote _Emote)
{
	Multi_PlayEmote(_Emote);
}

void APrototype2Character::Multi_PlayEmote_Implementation(EEmote _Emote)
{
	EmoteWidget->PlayEmote(_Emote);
}


void APrototype2Character::UpdatePlayerNames()
{
	const FVector CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector LookAtDirection = CameraLocation - PlayerNameWidgetComponent->GetComponentLocation();
	LookAtDirection.Normalize();

	FRotator LookAtRotation = LookAtDirection.Rotation();
	PlayerNameWidgetComponent->SetWorldRotation(LookAtRotation);
}

void APrototype2Character::UpdateEmote()
{
	const FVector CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector LookAtDirection = CameraLocation - EmoteWidgetComponent->GetComponentLocation();
	LookAtDirection.Normalize();

	FRotator LookAtRotation = LookAtDirection.Rotation();
	EmoteWidgetComponent->SetWorldRotation(LookAtRotation);
}

void APrototype2Character::SyncCharacterColourWithPlayerState()
{
	if (!IsValid(PlayerStateRef))
		return;
	
	if (GetMesh()->GetMaterial(0) != PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character])
	{
		auto animationData = AllAnimationDatas[(int32)PlayerStateRef->Details.Character];
		if (AllAnimationDatas.Num() > (int32)PlayerStateRef->Details.Character)
		{
			AnimationData = animationData;
		}
		if (animationData)
		{
			// Set the animation data
			
			
			// Set the skeletal mesh from animation data
			GetMesh()->SetSkeletalMeshAsset(animationData->SkeletalMesh);

			// When the skeletal mesh is set, the animation blueprint gets reset, so update it 
			if (animationData->TemplatedAnimationBlueprint)
			{
				GetMesh()->SetAnimInstanceClass(animationData->TemplatedAnimationBlueprint);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not set AnimationData in APrototype2Character::SyncCharacterColourWithPlayerState()"));
		}
		
		PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Cow Colour"), PlayerStateRef->Details.CharacterColour);
		PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Spot Colour"), PlayerStateRef->Details.CharacterSubColour);
	
		GetMesh()->SetMaterial(0, PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]);
	}	
}

void APrototype2Character::CheckForFalling(float _DeltaTime)
{
	if (GetMovementComponent()->IsFalling())
	{
		FallTimer += _DeltaTime;
	}
	else if(FallTimer > FallTimerThreshold)
	{
		FallTimer = 0.0f;
		LandAfterfalling();
	}
	else
	{
		FallTimer = 0.0f;
	}
}

void APrototype2Character::LandAfterfalling()
{
	// Line trace to check for shroom and dont play animation or jump if landing on shroom
	FVector StartLocation = GetActorLocation() + FVector{0,0,100}; // The start location of the line trace
	FVector EndLocation = GetActorLocation() + FVector{0,0,-200}; // The end location of the line trace
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation, ECC_Visibility, QueryParams);
	bool bIsLandingOnShroom = false;
	for (auto Hit : HitResults)
	{
		if (Cast<ALaunchPad>(Hit.GetActor()))
		{
			bIsLandingOnShroom=true;
			break;
		}
	}
	
	if (!bIsLandingOnShroom)
	{
		Jump();
		PlaySoundAtLocation(GetActorLocation(), FallOnButtCue);
		if (IsValid(AnimationData->FallOnButtAndGetBackUp))
		{
			if (!bIsChargingAttack)
			{
				PlayNetworkMontage(AnimationData->FallOnButtAndGetBackUp);
			}
			OnFallOnButtDelegate.Broadcast();
		}
	}
}

void APrototype2Character::ActivateParticleSystemFromEnum(EParticleSystems _NewSystem)
{
	if (!IsValid(VFXComponent))
		return;
	
	switch (_NewSystem)
	{
	case EParticleSystems::WalkPoof:
		{
			if (!WalkPoof_NiagaraComponent->IsActive())
			{
				VFXComponent->ActivateParticleSystem(WalkPoof_NiagaraComponent);
			}
			break;
		}
	case EParticleSystems::SprintPoof:
		{
			if (!SprintPoof_NiagaraComponent->IsActive())
			{
				VFXComponent->ActivateParticleSystem(SprintPoof_NiagaraComponent);}
			break;
		}
	case EParticleSystems::Sweat:
		{
			VFXComponent->ActivateParticleSystem(Sweat_NiagaraComponent);
			break;
		}
	case EParticleSystems::AttackTrail:
		{
			VFXComponent->ActivateParticleSystem(AttackTrail_NiagaraComponent);
			break;
		}
	case EParticleSystems::Attack:
		{
			VFXComponent->ActivateParticleSystem(Attack_NiagaraComponent);
			break;
		}
	case EParticleSystems::Dizzy:
		{
			VFXComponent->ActivateParticleSystem(Dizzy_NiagaraComponent);
			break;
		}
	case EParticleSystems::Smite:
		{
			VFXComponent->ActivateParticleSystem(Smite_NiagaraComponent);
			break;
		}
	case EParticleSystems::SmiteShockWave:
		{
			//SmiteShockWave_NiagaraComponent->SetVectorParameter(TEXT("BeamStartPoint"), SmiteCloud->GetComponentLocation());
			//SmiteShockWave_NiagaraComponent->SetVectorParameter(TEXT("BeamEndPoint"), GetActorLocation());
			SmiteShockWaveOnly_NiagaraComponent->SetWorldLocation(SmiteCloud->GetComponentLocation());
			VFXComponent->ActivateParticleSystem(SmiteShockWaveOnly_NiagaraComponent);
			VFXComponent->ActivateParticleSystem(SmiteShockWave_NiagaraComponent);
			break;
		}
	case EParticleSystems::SmiteElectrifyWarning:
		{
			VFXComponent->ActivateParticleSystem(SmiteElectrifyWarning_NiagaraComponent);
			break;
		}
	case EParticleSystems::SmiteStaticLightning:
		{
			VFXComponent->ActivateParticleSystem(SmiteStaticLightning_NiagaraComponent);
			break;
		}
	case EParticleSystems::AssertDominance:
		{
			VFXComponent->ActivateParticleSystem(AssertDominance_NiagaraComponent);
			break;
		}
	case EParticleSystems::Slow:
		{
			VFXComponent->ActivateParticleSystem(Slow_NiagaraComponent);
			break;
		}
	case EParticleSystems::Test:
		{
			//Test_NiagraComponent->Activate();
			break;
		}
	default:
		break;
	}
}

void APrototype2Character::DeActivateParticleSystemFromEnum(EParticleSystems _NewSystem)
{
	if (!IsValid(VFXComponent))
		return;
	
	switch (_NewSystem)
	{
	case EParticleSystems::WalkPoof:
		{
			if (WalkPoof_NiagaraComponent->IsActive())
			{
				WalkPoof_NiagaraComponent->Deactivate();
				VFXComponent->DeActivateParticleSystem(WalkPoof_NiagaraComponent);
				UKismetSystemLibrary::PrintString(GetWorld(), "Walkpoof DEactivated" + FString::SanitizeFloat(UGameplayStatics::GetTimeSeconds(GetWorld())));
			}
			break;
		}
	case EParticleSystems::SprintPoof:
		{
			if (SprintPoof_NiagaraComponent->IsActive())
			{
				VFXComponent->DeActivateParticleSystem(SprintPoof_NiagaraComponent);
			}
			break;
		}
	case EParticleSystems::Sweat:
		{
			VFXComponent->DeActivateParticleSystem(Sweat_NiagaraComponent);
			break;
		}
	case EParticleSystems::AttackTrail:
		{
			VFXComponent->DeActivateParticleSystem(AttackTrail_NiagaraComponent);
			break;
		}
	case EParticleSystems::Attack:
		{
			VFXComponent->DeActivateParticleSystem(Attack_NiagaraComponent);
			break;
		}
	case EParticleSystems::Dizzy:
		{
			VFXComponent->DeActivateParticleSystem(Dizzy_NiagaraComponent);
			break;
		}
	case EParticleSystems::Smite:
		{
			VFXComponent->DeActivateParticleSystem(Smite_NiagaraComponent);
			break;
		}
	case EParticleSystems::SmiteShockWave:
		{
			VFXComponent->DeActivateParticleSystem(SmiteShockWaveOnly_NiagaraComponent);
			VFXComponent->DeActivateParticleSystem(SmiteShockWave_NiagaraComponent);
			break;
		}
	case EParticleSystems::SmiteElectrifyWarning:
		{
			VFXComponent->DeActivateParticleSystem(SmiteElectrifyWarning_NiagaraComponent);
			break;
		}
	case EParticleSystems::SmiteStaticLightning:
		{
			VFXComponent->DeActivateParticleSystem(SmiteStaticLightning_NiagaraComponent);
			break;
		}
	case EParticleSystems::AssertDominance:
		{
			VFXComponent->DeActivateParticleSystem(AssertDominance_NiagaraComponent);
			break;
		}
	case EParticleSystems::Slow:
		{
			VFXComponent->DeActivateParticleSystem(Slow_NiagaraComponent);
			break;
		}
	case EParticleSystems::Test:
		{
			//Test_NiagraComponent->Deactivate();
			break;
		}
	default:
		break;
	}
}

void APrototype2Character::PickupItem(APickUpItem* _Item, EPickupActor _PickupType)
{
	if (!IsValid(_Item))
		return;
	
	Client_PickupItem(_Item, _PickupType);
	
	Server_PickupItem(_Item, _PickupType);

	// Refresh speed
	if (HasAuthority())
	{
		Client_RefreshCurrentMaxSpeed();
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		RefreshCurrentMaxSpeed();
	}
}


void APrototype2Character::UpdateDecalDirection(bool _bIsOn)
{
	if (IsLocallyControlled())
	{
		DecalComponent->SetVisibility(_bIsOn);
	}
}

void APrototype2Character::UpdateDecalDirection(bool _bIsOn, bool _bIsTargetingShippingBin)
{
	if (IsLocallyControlled())
	{
		DecalComponent->SetVisibility(_bIsOn);

		bDecalTargetShippingBin = _bIsTargetingShippingBin;
	}
}

void APrototype2Character::TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation)
{
	if (GetLocalRole() == ROLE_Authority || GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_TeleportToLocation(_DestinationLocation, _DestinationRotation);
	}
}

void APrototype2Character::DebugSomething()
{
	ActivateParticleSystemFromEnum(EParticleSystems::SmiteShockWave);
}

void APrototype2Character::Server_TeleportToLocation_Implementation(FVector _DestinationLocation, FRotator _DestinationRotation)
{
	SetActorLocation(_DestinationLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(_DestinationRotation, ETeleportType::TeleportPhysics);
}

bool APrototype2Character::HasClaimedPlot()
{
	if (IsValid(ClaimedPlot))
		return true;

	return false;
}

void APrototype2Character::SetClaimedPlot(ARadialPlot* _Plot)
{
	if (!IsValid(_Plot))
		return;
	
	ClaimedPlot = _Plot;
	OnPlotClaimedDelegate.Broadcast(_Plot);
	Client_BroadcastPlotClaimed(_Plot);
}

void APrototype2Character::OnRep_HeldItem()
{
	RefreshCurrentMaxSpeed();
	if (!IsValid(HeldItem))
	{
		if (PlayerHUDRef)
		{
			PlayerHUDRef->ClearPickupUI();
		}
	}
}

bool APrototype2Character::IsHolding(UClass* _ObjectType)
{
	bool IsHoldingObjectType = false;

	if (IsValid(HeldItem))
	{
		IsHoldingObjectType = HeldItem->IsA(_ObjectType);
	}
	
	return IsHoldingObjectType;
}

USeedData* APrototype2Character::GetHeldItemData()
{
	USeedData* OutSeedData{nullptr};
	if (IsValid(HeldItem))
	{
		if (IsValid(HeldItem->ServerData.SeedData))
		{
			OutSeedData = HeldItem->ServerData.SeedData;
		}
	}
	return OutSeedData;
}

void APrototype2Character::ClearPickupUI()
{
	if (IsValid(PlayerHUDRef))
		PlayerHUDRef->ClearPickupUI();
}

void APrototype2Character::UpdatePickupUI(UTexture2D* _Icon)
{
	if (IsValid(PlayerHUDRef))
		PlayerHUDRef->UpdatePickupUI(_Icon);
}

void APrototype2Character::AddCoins(int32 _Amount)
{
	if (IsValid(PlayerStateRef))
	{
		PlayerStateRef->AddCoins(_Amount);
	}
}

void APrototype2Character::OnUpdate_InteractTimeline(float _Progress)
{
	if (!IsValid(ClosestInteractableActor) || ClosestInteractableItem == nullptr)
		return;
	
	OnClaimingPlotDelegate.Broadcast(_Progress, InteractTimeline->GetTimelineLength());

}

void APrototype2Character::OnFinish_InteractTimeline()
{
	Interact();
	OnStoppedClaimingPlotDelegate.Broadcast();
	ReleaseInteract();
	//bAllowMovementFromInput = true;
}

void APrototype2Character::Client_BroadcastPlotClaimed_Implementation(ARadialPlot* _Plot)
{
	OnPlotClaimedDelegate.Broadcast(_Plot);
}

bool APrototype2Character::IsSprinting()
{
	return bSprinting;
}

void APrototype2Character::CheckForFloorSurface()
{
	//UE_LOG(LogTemp, Warning, TEXT("Ground Check"));

	if (!HasIdealRole())
		return;
	
	FVector StartLocation = GetActorLocation() + FVector{0,0,100}; // The start location of the line trace
	FVector EndLocation = GetActorLocation() + FVector{0,0,-100}; // The end location of the line trace

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true; // Enable complex tracing for accurate physics material retrieval
	QueryParams.bReturnPhysicalMaterial = true;

	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	GetCharacterMovement()->BrakingFriction = 2.0f;
	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->GroundFriction = 8.0f;
	
	// Perform the line trace
	if (GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation, ECC_EngineTraceChannel1, QueryParams))
	{
		for(auto& Hit : HitResults)
		{
			if (Hit.GetActor()->ActorHasTag(FName(TEXT("goop"))))
			{
				if (DebuffComponent->CurrentDebuff != EDebuff::None)
					return;
				
				if (HasAuthority())
				{
					DebuffComponent->DebuffInfo.Debuff = EDebuff::Slow;
					DebuffComponent->DebuffInfo.Duration = 0.3f;
					DebuffComponent->OnRep_ApplyDebuff();
				}
				else if (GetLocalRole() == ROLE_AutonomousProxy)
				{
					Server_ApplyHoneyGoopSlow();
				}
			}
			
			if (const UPhysicalMaterial* PhysicalMaterial = Hit.PhysMaterial.Get())
			{
				const float Friction = PhysicalMaterial->Friction;
			
				if (Friction <= 0.5f)
				{
					GetCharacterMovement()->BrakingFriction = 0.0f;
					GetCharacterMovement()->MaxAcceleration = 2048.0f * 0.5f;
					GetCharacterMovement()->GroundFriction = 0.0f;
					break;
				}
			}
		}
	}
}

void APrototype2Character::PlaySoundAtLocation(FVector _Location, USoundCue* _SoundToPlay, USoundAttenuation* _Attenuation)
{
	if (HasAuthority())
	{
		Multi_PlaySoundAtLocation(_Location, _SoundToPlay, _Attenuation );
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_PlaySoundAtLocation(_Location, _SoundToPlay, _Attenuation );
	}
}

void APrototype2Character::PlayWeaponSound(USoundCue* _SoundToPlay)
{
	if (HasAuthority())
	{
		Multi_PlayWeaponSound(_SoundToPlay);
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_PlayWeaponSound(_SoundToPlay);
	}
}

void APrototype2Character::Server_PlayWeaponSound_Implementation(USoundCue* _SoundToPlay)
{
	Multi_PlayWeaponSound(_SoundToPlay);
}

void APrototype2Character::Multi_PlayWeaponSound_Implementation(USoundCue* _SoundToPlay)
{
	WeaponAudioComponent->SetSound(_SoundToPlay);
	WeaponAudioComponent->Play();
}

UWidget_PlayerHUD* APrototype2Character::GetPlayerHUD()
{
	// Update UI
	return PlayerHUDRef;
}

void APrototype2Character::Multi_TeleportToLocation_Implementation(FVector _DestinationLocation, FRotator _DestinationRotation)
{
	SetActorLocation(_DestinationLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(_DestinationRotation, ETeleportType::TeleportPhysics);
}

void APrototype2Character::PlayNetworkMontage(UAnimMontage* _Montage)
{
	if (HasAuthority())
		Multi_PlayNetworkMontage(_Montage);
	else
		Server_PlayNetworkMontage(_Montage);
}

UCameraComponent* APrototype2Character::ReturnFollowCamera()
{
	return FollowCamera;
}


void APrototype2Character::DropWeapon()
{
	//Client_DropWeapon(); // no longer called from RPC
	// Set UI
	if (IsValid(PlayerHUDRef) && IsValid(DefaultWeaponSeedData))
	{
		PlayerHUDRef->WeaponImage->SetBrushFromTexture(DefaultWeaponSeedData->WeaponData->WeaponIcon);
		FVector2D ImageSize ={(float)DefaultWeaponSeedData->WeaponData->WeaponIcon->GetSizeX(), (float)DefaultWeaponSeedData->WeaponData->WeaponIcon->GetSizeY()};
		PlayerHUDRef->WeaponImage->SetDesiredSizeOverride(ImageSize);
	}
	if (HasAuthority())
	{
		Multi_DropWeapon();
	}
	else
	{
		Server_DropWeapon();
	}
}

void APrototype2Character::SetPlayerAimingMovement(bool _bIsAiming)
{

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
	bAllowMovementFromInput = !_bIsAiming;

	//Server_SetPlayerAimingMovement(_bIsAiming);
}

//void APrototype2Character::Multi_SetPlayerAimingMovement_Implementation(bool _bIsAiming)
//{
//	GetCharacterMovement()->bUseControllerDesiredRotation = true;
//	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
//	bAllowMovementFromInput = !_bIsAiming;
//}
//
//void APrototype2Character::Client_SetPlayerAimingMovement_Implementation(bool _bIsAiming)
//{
//	GetCharacterMovement()->bUseControllerDesiredRotation = true;
//	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
//	bAllowMovementFromInput = !_bIsAiming;
//}

void APrototype2Character::Server_CancelChargeAttack_Implementation()
{
	Multi_CancelChargeAttack();
}

void APrototype2Character::Multi_CancelChargeAttack_Implementation()
{
	// Tell the animation blueprint the charge attack is cancelled
	OnChargeCancelledDelegate.Broadcast();
	ResetAttack();
}

void APrototype2Character::Client_PickupItem_Implementation(APickUpItem* _Item, EPickupActor _PickupType)
{
	if (!IsValid(_Item))
		return;
	
	if (IsValid(PlayerHUDRef))
		PlayerHUDRef->SetHUDInteractText("");

	if (IsValid(HeldItem))
		HeldItem->Client_Pickup(this);
	
	// Set the HUD UI pickup icon depending on seed/plant/weapon
	switch (_PickupType)
	{
	case EPickupActor::PlantActor:
		{
			if (!IsValid(PlayerHUDRef) || !_Item->ServerData.SeedData->PlantData)
			{
				break;
			}
			if (_Item->ItemComponent->bGold)
			{
				PlayerHUDRef->UpdatePickupUI(_Item->ServerData.SeedData->PlantData->GoldPlantIcon);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Plant was normal when picked up for me"));
				PlayerHUDRef->UpdatePickupUI(_Item->ServerData.SeedData->PlantData->PlantIcon);
			}
			OnPickUpItemDelegate.Broadcast();
			break;
		}
	case EPickupActor::WeaponActor:
		{
			// Set UI
			if (PlayerHUDRef && _Item->ServerData.SeedData->WeaponData)
			{
				PlayerHUDRef->ClearPickupUI();
				if (_Item->ItemComponent->bGold)
				{
					PlayerHUDRef->WeaponImage->SetBrushFromTexture(_Item->ServerData.SeedData->WeaponData->GoldWeaponIcon);
				}
				else
				{
					PlayerHUDRef->WeaponImage->SetBrushFromTexture(_Item->ServerData.SeedData->WeaponData->WeaponIcon);
				}
				FVector2D ImageSize ={(float)_Item->ServerData.SeedData->WeaponData->WeaponIcon->GetSizeX(), (float)_Item->ServerData.SeedData->WeaponData->WeaponIcon->GetSizeY()};
				PlayerHUDRef->WeaponImage->SetDesiredSizeOverride(ImageSize);
				OnPickUpWeaponDelegate.Broadcast();
				
				WeaponCurrentDurability = _Item->ServerData.SeedData->WeaponData->Durability;
				if (_Item->ItemComponent->bGold)
				{
					WeaponCurrentDurability *= _Item->ServerData.SeedData->WeaponData->GoldDurabilityMultiplier;
				}
			}			
			break;
		}
	case EPickupActor::SeedActor:
		{
			if (IsValid(PlayerHUDRef) && _Item)
			{
				PlayerHUDRef->UpdatePickupUI(_Item->ServerData.SeedData->PacketIcon);
			}
			OnPickUpItemDelegate.Broadcast();
			break;
		}
	case EPickupActor::FertilizerActor:
		{
			if (!IsValid(PlayerHUDRef))
				break;
			
			if (_Item->ServerData.SeedData->PacketIcon)
			{
				PlayerHUDRef->UpdatePickupUI(_Item->ServerData.SeedData->PacketIcon);
			}
			OnPickUpItemDelegate.Broadcast();
		}
	default:
		break;
	}
}

void APrototype2Character::Client_DropItem_Implementation()
{
	if (IsValid(HeldItem))
	{
		HeldItem->Client_Drop();
	}
	// clear pickup UI and text
	if (IsValid(PlayerHUDRef))
	{
		PlayerHUDRef->ClearPickupUI();
		PlayerHUDRef->SetHUDInteractText("");
	}
}

void APrototype2Character::Client_DropWeapon_Implementation()
{
	// Set UI
	if (IsValid(PlayerHUDRef) && IsValid(DefaultWeaponSeedData))
	{
		PlayerHUDRef->WeaponImage->SetBrushFromTexture(DefaultWeaponSeedData->WeaponData->WeaponIcon);
		FVector2D ImageSize ={(float)DefaultWeaponSeedData->WeaponData->WeaponIcon->GetSizeX(), (float)DefaultWeaponSeedData->WeaponData->WeaponIcon->GetSizeY()};
		PlayerHUDRef->WeaponImage->SetDesiredSizeOverride(ImageSize);
	}
}

//void APrototype2Character::Server_SetPlayerAimingMovement_Implementation(bool _bIsAiming)
//{
//	GetCharacterMovement()->bUseControllerDesiredRotation = _bIsAiming;
//	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
//	bAllowMovementFromInput = !_bIsAiming;
//
//	Multi_SetPlayerAimingMovement(_bIsAiming);
//}

void APrototype2Character::SetMaxWalkSpeed(float _Speed)
{
	if (!HasIdealRole())
		return;

	GetCharacterMovement()->MaxWalkSpeed = _Speed;

	if (!HasAuthority())
		Server_SetMaxWalkSpeed(_Speed);
}

void APrototype2Character::Server_SetMaxWalkSpeed_Implementation(float _Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = _Speed;
}

void APrototype2Character::Server_SetSprintState_Implementation(bool _NewSprintAnimationState)
{
	bSprintAnimationState = _NewSprintAnimationState;
}

void APrototype2Character::RefreshCurrentMaxSpeed()
{
	if (DebuffComponent->CurrentDebuff != EDebuff::None)
	{
		return;
	}

	// Handle gold held item 
	if (IsValid(HeldItem) && HeldItem->ItemComponent->bGold)
	{
		if (bSprinting)
		{
			SetMaxWalkSpeed(WalkSpeed);
			DeActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
			ActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
			ActivateParticleSystemFromEnum(EParticleSystems::AssertDominance);
			return;
		}
		SetMaxWalkSpeed(GoldPlantSpeed);
		DeActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
		DeActivateParticleSystemFromEnum(EParticleSystems::AssertDominance);
		return;
	}

	// sprint
	if (bSprinting)
	{
		SetMaxWalkSpeed(SprintSpeed);
		DeActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
		ActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
		ActivateParticleSystemFromEnum(EParticleSystems::AssertDominance);
		return;
	}

	// walk
	SetMaxWalkSpeed(WalkSpeed);
	DeActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
	DeActivateParticleSystemFromEnum(EParticleSystems::AssertDominance);
}

void APrototype2Character::Client_RefreshCurrentMaxSpeed_Implementation()
{
	RefreshCurrentMaxSpeed();
}

void APrototype2Character::ResetAttack()
{
	bCanAttack = true;
	bIsChargingAttack = false;
	AttackChargeAmount = 0.0f;
	AutoAttackTimer = 0.0f;
	// Stop the player Interacting while "executing attack"
	InteractTimer = InteractTimerTime;
	SetPlayerAimingMovement(false);
	AttackAreaIndicatorMesh->SetHiddenInGame(false);
	
	// Reset Attack Timer
	if (HasAuthority())
	{
		// Reset Attack variables
		//bIsChargingAttack = false;

		// Charge animation
		bChargeAnimationState = false;
		OnRep_ChargeStateChanged();
	}
	else
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_ResetAttack();
		}
	}
}

void APrototype2Character::Server_ResetAttack_Implementation()
{
	// Reset Attack variables
	
	// Charge animation
	bChargeAnimationState = false;
	OnRep_ChargeStateChanged();
	
	bCanAttack = true;
}

void APrototype2Character::FollowThroughOnAttack()
{
	if (bIsFollowingThroughAttack)
	{
		AddMovementInput(GetActorForwardVector(), 1.0f);
	}
}

void APrototype2Character::OnRep_ChargeStateChanged()
{
	OnChargeStateChangedDelegate.Broadcast(bChargeAnimationState);
}

void APrototype2Character::Server_SetAOEIndicatorVisibility_Implementation(bool _bIsVisible)
{
	AttackAreaIndicatorMesh->SetHiddenInGame(!_bIsVisible);
}

void APrototype2Character::ThrowItem()
{
	if (!IsValid(HeldItem))
		return;
	
	// Local Stuff
	if (IsValid(PlayerHUDRef))
	{
		PlayerHUDRef->ClearPickupUI();
		PlayerHUDRef->SetHUDInteractText("");
	}
	if (IsValid(HeldItem))
		HeldItem->Client_Drop();
	
	if (HasAuthority())
	{
		// Server_DropItem
		if (IsValid(HeldItem))
		HeldItem->Server_Drop();
			
		if (IsValid(DropCue))
			PlaySoundAtLocation(GetActorLocation(), DropCue);
		if (IsValid(WeaponMesh))
			Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));

		CalculateThrowVectorAndLaunch(GetActorForwardVector(), GetCharacterMovement()->GetLastUpdateVelocity().Length());
		HeldItem = nullptr;
	}
	else
	{
		Server_ThrowItem(GetActorForwardVector(), GetCharacterMovement()->GetLastUpdateVelocity().Length());
	}
	RefreshCurrentMaxSpeed();
}

void APrototype2Character::CalculateThrowVectorAndLaunch(FVector _ForwardVector, float _PlayerSpeed)
{
	// Calculate the throw vector
	FVector ThrowVector = _ForwardVector;
	float PlayerSpeed = FMath::Clamp(_PlayerSpeed, 0, 450);
	ThrowVector *= ThrowItemStrength + PlayerSpeed;
	ThrowVector.Z = 500.0f;

	// Detach from player and setup collisions
	Multi_ThrowItem();

	// Throw
	HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
	HeldItem->ItemComponent->Mesh->AddImpulse(ThrowVector, NAME_None, true);
	HeldItem->ItemComponent->PlayerWhoThrewItem = this;
}

void APrototype2Character::Server_ApplyHoneyGoopSlow_Implementation()
{
	DebuffComponent->DebuffInfo.Debuff = EDebuff::Slow;
	DebuffComponent->DebuffInfo.Duration = 0.3f;
}

void APrototype2Character::Multi_ThrowItem_Implementation()
{
	// Drop into world
	if (!IsValid(HeldItem))
		return;
	
	HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
	HeldItem->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// So that CheckForInteractables() can see it again
	HeldItem->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
}

void APrototype2Character::Server_ThrowItem_Implementation(FVector _ForwardVector, float _PlayerSpeed)
{
	if (!IsValid(HeldItem))
		return;
	
	HeldItem->Server_Drop();
	
	// Server_DropItem
	if (IsValid(DropCue))
		PlaySoundAtLocation(GetActorLocation(), DropCue);
	if (IsValid(WeaponMesh))
		Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));

	CalculateThrowVectorAndLaunch(_ForwardVector, _PlayerSpeed);
	HeldItem = nullptr;
}

void APrototype2Character::Client_BroadcastOvercharge_Implementation()
{
	OnOverChargeDelegate.Broadcast(AutoAttackTimer, AutoAttackDuration);
}

void APrototype2Character::SetCanSprint(bool _bCanSprint)
{
	if (_bCanSprint)
	{
		UpdateCanSprintUI();
	}
	else
	{
		ActivateParticleSystemFromEnum(EParticleSystems::Sweat);
		OnFailedToSprintDelegate.Broadcast();
	}
}

void APrototype2Character::OnLaunchedByLaunchPad()
{
	OnLaunchedByLaunchPadDelegate.Broadcast();
}

void APrototype2Character::TeleportToEndGame(FTransform _EndGameTransform)
{
	if (HasAuthority())
	{
		Multi_TeleportToEndGame(_EndGameTransform);
		TArray<AActor*> Smite{};
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASmiteManager::StaticClass(), Smite);
		for(AActor* SmiteActor : Smite)
		{
			if (auto* SmiteManager = Cast<ASmiteManager>(SmiteActor))
			{
				SmiteManager->ClearPlayerSmites();
			}
		}
	}
	else
	{
		Server_TeleportToEndGame(_EndGameTransform);
	}
}

void APrototype2Character::Server_TeleportToEndGame_Implementation(FTransform _EndGameTransform)
{
	Multi_TeleportToEndGame(_EndGameTransform);
}

void APrototype2Character::Multi_TeleportToEndGame_Implementation(FTransform _EndGameTransform)
{
	//bIsHoldingInteract = false;
	PlayerNameWidgetComponent->SetVisibility(true);
	EmoteWidgetComponent->SetVisibility(true);

	OnPlayerAtEndGamePodiumDelegate.Broadcast();
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	
	SetActorLocation(_EndGameTransform.GetLocation(), false, nullptr, ETeleportType::ResetPhysics);
	SetActorRotation(_EndGameTransform.Rotator(), ETeleportType::ResetPhysics);
}

bool APrototype2Character::HasIdealRole()
{
	return HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy;
}

bool APrototype2Character::GetHasCrown()
{
	// check if this player is crown holder and give points to the attacker if so
	if (!IsValid(PlayerStateRef) || !IsValid(GameState))
	{
		return false;
	}

	UPrototypeGameInstance* GameInstance = Cast<UPrototypeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance))
	{
		if (GameInstance->bTeams)
			return false;
		if (GameState->bGameHasStarted == false)
			return false;
		
		for(auto SomePlayerState : GameState->Server_Players)
		{
			if (SomePlayerState->Coins > PlayerStateRef->Coins && SomePlayerState != PlayerStateRef)
				return false;
		}
	}
	
	return true;
}

void APrototype2Character::InitPlayerNameWidgetComponent()
{
	if (IsValid(PlayerNameWidget))
		return;
		
	if (!IsValid(PlayerNameWidgetComponent))
		return;

	UUserWidget* Widget = PlayerNameWidgetComponent->GetWidget();
	if (!Widget)
		return;

	UWidget_PlayerName* NameWidget = Cast<UWidget_PlayerName>(Widget);
	if (!IsValid(NameWidget))
		return;

	if (!IsValid(PlayerStateRef))
		return;

	if (IsLocallyControlled())
	{
		PlayerNameWidgetComponent->SetVisibility(false);
	}
	
	PlayerNameWidget = NameWidget;
	PlayerNameWidget->SetPlayerRef(PlayerStateRef);
}

void APrototype2Character::InitEmoteWidgetComponent()
{
	if (IsValid(EmoteWidget))
		return;
		
	if (!IsValid(EmoteWidgetComponent))
		return;

	UUserWidget* Widget = EmoteWidgetComponent->GetWidget();
	if (!Widget)
		return;

	UWidget_PlayerEmote* PlayerEmoteWidget = Cast<UWidget_PlayerEmote>(Widget);
	if (!PlayerEmoteWidget)
		return;

	if (!IsValid(PlayerStateRef))
		return;
	
	EmoteWidget = PlayerEmoteWidget;
	EmoteWidget->SetPlayerRef(PlayerStateRef);
}

void APrototype2Character::InitWeapon()
{
	AutoAttackTimer = 0.0f;
	bCanAttack = true;
}
void APrototype2Character::InitPlayerHUD()
{
	if (IsValid(PlayerHUDRef))
		return;

	if (!IsValid(PlayerController))
		return;
	
	if (!IsValid(PlayerHudPrefab))
		return;
	
	if (!HasNetOwner())
		return;
	
	PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(PlayerController, PlayerHudPrefab);
	PlayerHUDRef->AddToViewport();

	PlayerHUDRef->WeaponImage->SetBrushFromTexture(CurrentWeaponSeedData->WeaponData->WeaponIcon);
}

void APrototype2Character::InitShippingBin()
{
	if (IsValid(SellBin))
		return;
	
	if (!HasIdealRole())
		return;
	
	AActor* FoundSellBin = UGameplayStatics::GetActorOfClass(GetWorld(), ASellBin::StaticClass());
	if (!IsValid(FoundSellBin))
		return;

	SellBin = Cast<ASellBin>(FoundSellBin);
}

void APrototype2Character::InitDecals()
{
	if (!IsValid(DecalComponent))
		return;
	
	DecalComponent->SetIsReplicated(false);
	DecalComponent->SetVisibility(false);

	if (!IsValid(DecalArmSceneComponent))
		return;
	
	DecalArmSceneComponent->SetIsReplicated(false);
}

void APrototype2Character::ClearInteractionText()
{
	if (!IsValid(PlayerHUDRef))
		return;
	
	PlayerHUDRef->SetHUDInteractText("");
}

bool APrototype2Character::GetIsPunching()
{
	if (!IsValid(DefaultWeaponSeedData) || !IsValid(CurrentWeaponSeedData))
	{
		return false;
	}
	return DefaultWeaponSeedData == CurrentWeaponSeedData; 
}

bool APrototype2Character::GetIsWeaponGold()
{
	return bIsHoldingGoldWeapon;
}

void APrototype2Character::HandleAttackChargeBehavior(float _DeltaSeconds)
{
	if (bIsChargingAttack)
	{
		if(AttackChargeAmount < MaxAttackCharge)// + AutoAttackDuration)
		{
			AttackChargeAmount += _DeltaSeconds * 1.2f; // increase charge speed by 10%
		}
		
		if (AttackChargeAmount >= MaxAttackCharge)
		{
			AttackChargeAmount = MaxAttackCharge;
			AutoAttackTimer += _DeltaSeconds;
		}

		if (AttackChargeAmount >= MaxAttackCharge)
		{
			OnOverChargeDelegate.Broadcast(AutoAttackTimer, AutoAttackDuration);
			
			if (AutoAttackTimer >= AutoAttackDuration)
				ReleaseAttack();
		}
	}
	if (bChargeAnimationState)
	{
		if (AttackChargeAmount > MaxAttackCharge)
		{
			AttackChargeAmount = MaxAttackCharge;
		}
		if (IsValid(Weapon))
			Weapon->UpdateAOEIndicator(this, AttackChargeAmount);
	}
	else
	{
		AttackAreaIndicatorMesh->SetHiddenInGame(true);
	}
}

void APrototype2Character::UpdatePlayerHUD()
{
	if (!IsValid(PlayerHUDRef))
		return;

	PlayerHUDRef->SetPlayerSprintTimer(SprintTimer);

	if(!IsValid(HeldItem) && !IsValid(Weapon))
	{
		PlayerHUDRef->ClearPickupUI();
	}
}

void APrototype2Character::UpdateWeaponMeshSkin()
{
	WeaponMesh->SetStaticMesh(CurrentWeaponSeedData->BabyMesh);
	
	if (bShouldWeaponFlashRed)
	{
		for (int i = 0; i < CurrentWeaponSeedData->BabyMaterials.Num(); i++)
		{
			WeaponMesh->SetMaterial(i, FlashRedMaterial);
		}
	}
	else if (bIsHoldingGoldWeapon)
	{
		for (int i = 0; i < CurrentWeaponSeedData->BabyGoldMaterials.Num(); i++)
		{
			if (CurrentWeaponSeedData->BabyGoldMaterials[i])
			{
				WeaponMesh->SetMaterial(i, CurrentWeaponSeedData->BabyGoldMaterials[i]);
			}
			// if picking up golden honey after holding a golden weapon, this makes the honey go back
			// to honey and stop it from being a gold blob on the end
			else if (CurrentWeaponSeedData->BabyMaterials[i])
			{
				WeaponMesh->SetMaterial(i, CurrentWeaponSeedData->BabyMaterials[i]);
			}
		}
	}
	else
	{
		for (int i = 0; i < CurrentWeaponSeedData->BabyMaterials.Num(); i++)
		{
			if (CurrentWeaponSeedData->BabyMaterials[i])
			{
				WeaponMesh->SetMaterial(i, CurrentWeaponSeedData->BabyMaterials[i]);
			}
		}
	}
}

void APrototype2Character::UpdateParticleSystems()
{
	if (GetCharacterMovement()->Velocity.Size() < 100.0f || !GetCharacterMovement()->IsMovingOnGround())
	{
		if (WalkPoof_NiagaraComponent->IsActive())
		WalkPoof_NiagaraComponent->Deactivate();
		//DeActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
	}
	else
	{
		if (!WalkPoof_NiagaraComponent->IsActive())
		WalkPoof_NiagaraComponent->Activate();
		//ActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
	}
}

void APrototype2Character::ToggleParticleSystems()
{
	
	//if (ParticleSystemsToActivate.Num() > 0 || ParticleSystemsToDeActivate.Num() > 0)
	//{
	//	Server_ToggleParticleSystems(ParticleSystemsToActivate, ParticleSystemsToDeActivate);
	//	ParticleSystemsToActivate.Empty();
	//	ParticleSystemsToDeActivate.Empty();
	//}
}

void APrototype2Character::DeltaDecrement(float& _Variable, float& _DeltaSeconds)
{
	if (_Variable > 0)
		_Variable -= _DeltaSeconds;
}

void APrototype2Character::TickTimers(float _DeltaSeconds)
{
	// No longer replicated
	DeltaDecrement(AttackTimer, _DeltaSeconds);
	DecrementSprintTimers(_DeltaSeconds);
	DeltaDecrement(InvincibilityTimer, _DeltaSeconds);
	DeltaDecrement(WeaponFlashTimer, _DeltaSeconds);
	if (WeaponFlashTimer < 0.0f)
	{
		bShouldWeaponFlashRed = false;
		WeaponFlashTimer = WeaponFlashDuration;
	}
	DeltaDecrement(InteractTimer, _DeltaSeconds);
}

void APrototype2Character::DecrementSprintTimers(float _DeltaSeconds)
{
	bool chargingWithWeapon = bIsChargingAttack && CurrentWeaponSeedData != DefaultWeaponSeedData;
	
	if (bSprinting)
	{
		if (SprintTimer > 0)
		{
			SprintTimer -= chargingWithWeapon ? _DeltaSeconds * 2 : _DeltaSeconds; // magic number increaeses sprint drain when sprinting with weapon

			if (SprintTimer <= 0)
			{
				// end sprint
				bSprinting = false;
				bHasNotifiedCanSprint = false;
				EndSprint();
				SetCanSprint(false);
				bCanSprint = false;
			}
		}
	}
	else
	{
		if (SprintTimer < SprintTime)
		{
			if (DelayedSprintRegenTimer < DelayedSprintRegenTotalDuration)
			{
				DelayedSprintRegenTimer += _DeltaSeconds;
			}
			else
			{
				SprintTimer += _DeltaSeconds * 0.75f; // magic number slows down sprint regen
			}
		}
		else
		{
			SetCanSprint(true);

			bHasNotifiedCanSprint = true;
			bCanSprint = true;
		}
	}
}

void APrototype2Character::Multi_SyncCharacterColour_Implementation()
{
	auto SkinColour = PlayerStateRef->Details.CharacterColour;
	auto FeaturesColour = PlayerStateRef->Details.CharacterColour / 1.5f;
	PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Cow Colour"), SkinColour);
	PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Spot Colour"), FeaturesColour);
	
	GetMesh()->SetMaterial(0, PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]);
}

void APrototype2Character::Server_PlayNetworkMontage_Implementation(UAnimMontage* _Montage)
{
	Multi_PlayNetworkMontage(_Montage);
}

void APrototype2Character::Multi_PlayNetworkMontage_Implementation(UAnimMontage* _Montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_Montage);
}

void APrototype2Character::Server_SetPlayerColour_Implementation()
{
	Multi_SetPlayerColour();
}

void APrototype2Character::OnRep_InteractTimer()
{
	if (InteractTimer == InteractTimerTime)
		ReleaseInteract();
}

void APrototype2Character::SocketWeapon(FName _Socket)
{
	if (!IsValid(WeaponMesh))
		return;
	
	Server_SocketItem(WeaponMesh, _Socket);
}

void APrototype2Character::Server_AddHUD_Implementation()
{
	Client_AddHUD();
}

void APrototype2Character::Multi_Client_AddHUD_Implementation()
{
	if (IsValid(PlayerHudPrefab) && !IsValid(PlayerHUDRef) && IsValid(PlayerStateRef))
    {
    	PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), PlayerStateRef->Player_ID), PlayerHudPrefab);
   
    	if (IsValid(PlayerHUDRef))
    		PlayerHUDRef->AddToViewport();
    }
}

void APrototype2Character::Server_ChargeAttack_Implementation()
{
	// Charge animation
	bChargeAnimationState = true;
	OnRep_ChargeStateChanged();
	
	Weapon->ChargeAttack(this);
}

void APrototype2Character::Server_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation)
{
	if (!IsValid(_SoundQueue))
		return;
	
	Multi_PlaySoundAtLocation(_Location, _SoundQueue, _Attenuation);
}

void APrototype2Character::Multi_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation)
{
	if (!IsValid(_SoundQueue))
		return;
	
	if (_Attenuation)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _SoundQueue, _Location, 1, 1, 0, _Attenuation);
	else
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _SoundQueue, _Location, 1, 1, 0, SoundAttenuationSettings);

}

void APrototype2Character::Client_AddHUD_Implementation()
{
	if (IsValid(PlayerHudPrefab) && !IsValid(PlayerHUDRef))
	{
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(Cast<APrototype2PlayerController>(Controller), PlayerHudPrefab);

		PlayerHUDRef->AddToViewport();
	}
}

void APrototype2Character::Multi_SetPlayerColour_Implementation()
{
	// No longer used?
	/*if (auto* GamemodeCast = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (auto* GamestateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			for(auto PlayerStateCast : GamestateCast->PlayerArray)
			{
				if (auto* CharacterCast = Cast<APrototype2Character>(PlayerStateCast->GetPlayerController()->GetCharacter()))
				{
					//character->GetMesh()->SetMaterial(0, gamemode->PlayerMaterials[Cast<APrototype2PlayerState>(player)->Player_ID]);
				}
			}
		}
	}*/
}

void APrototype2Character::Server_DropItem_Implementation(float WhoopsyStrength)
{
	if (!IsValid(HeldItem))
		return;
	
	HeldItem->Server_Drop();
	
	// Server_DropItem
	if (IsValid(DropCue))
		PlaySoundAtLocation(GetActorLocation(), DropCue);
	if (IsValid(WeaponMesh))
		Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
			
	Multi_DropItem();
	HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
	HeldItem->ItemComponent->Mesh->AddImpulse(FVector::UpVector * WhoopsyStrength, NAME_None, true);

	HeldItem = nullptr;
}

void APrototype2Character::Multi_DropItem_Implementation()
{
	// Drop into world
	if (!IsValid(HeldItem))
		return;

	HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HeldItem->SetActorLocation(GetActorLocation() + (FVector::UpVector * 100.0f));
		
	HeldItem->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// So that CheckForInteractables() can see it again
	HeldItem->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
}

void APrototype2Character::Server_PickupItem_Implementation(APickUpItem* _Item, EPickupActor _PickupType)
{
	PlaySoundAtLocation(GetActorLocation(), PickUpCue);
	
	if (IsValid(HeldItem))
	{
		if (HasAuthority())
			HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
	
		Multi_DropItem();
	}

	if (!IsValid(_Item))
		return;
	
	_Item->ItemComponent->Mesh->SetSimulatePhysics(false);
	switch (_PickupType)
	{
	case EPickupActor::PlantActor:
		{
			HeldItem = _Item;

			_Item->ItemComponent->PlayerWhoThrewItem = nullptr;
			
			Multi_SocketItem_Implementation(WeaponMesh, FName("WeaponHolsterSocket"));
			break;
		}
	case EPickupActor::WeaponActor:
		{
			// Replace weapon with new one
			if (Weapon)
				Weapon->DestroyComponent();
			if (UWeapon* NewWeapon = NewObject<UWeapon>(this, _Item->ServerData.SeedData->WeaponData->WeaponComponent))
			{
				NewWeapon->RegisterComponent();
				NewWeapon->SetIsReplicated(true);
				Weapon = NewWeapon;
			}
			
			CurrentWeaponSeedData = _Item->ServerData.SeedData;
			
			//WeaponCurrentDurability = CurrentWeaponSeedData->WeaponData->Durability; // moved to client, no longer replicate
			if (_Item->ItemComponent->bGold)
			{
				//WeaponCurrentDurability *= CurrentWeaponSeedData->WeaponData->GoldDurabilityMultiplier;// moved to client, no longer replicate
				bIsHoldingGoldWeapon = true;
			}
			else
			{
				bIsHoldingGoldWeapon = false;
			}

			CurrentWeaponAnimation = _Item->ServerData.SeedData->WeaponData->WeaponAnimationType;
			_Item->Destroy();

			Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
			break;
		}
	default:
		{
			HeldItem = _Item;
			
			Multi_SocketItem(WeaponMesh, FName("WeaponHolsterSocket"));
			break;
		}
	}

	Multi_PickupItem(_Item, _PickupType);
}

void APrototype2Character::Server_SocketItem_Implementation(UStaticMeshComponent* _Object, FName _Socket)
{
	Multi_SocketItem(_Object, _Socket);
	//_Object->SetSimulatePhysics(false);
	//_Object->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//_Object->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	//
	//_Object->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(_Socket));
}

void APrototype2Character::Server_DropWeapon_Implementation()
{
	Multi_DropWeapon();
}

void APrototype2Character::Multi_PickupItem_Implementation(APickUpItem* _Item, EPickupActor _PickupType)
{
	if (!IsValid(_Item))
	{
		return;
	}	
	
	_Item->ItemComponent->Mesh->SetSimulatePhysics(false);
	// Setup picked up items mesh physics and collision response
	_Item->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	// So that CheckForInteractables() cant see it while player is holding it
	_Item->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// Attach plant/seed ot the HeldItemSocket, and change weapon mesh to the one passed in
	switch (_PickupType)
	{
	case EPickupActor::WeaponActor:
		{
			AttackAreaIndicatorMesh->SetStaticMesh(_Item->ServerData.SeedData->WeaponData->AOEIndicatorMesh);
			break;
		}
	default:
		{
			_Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeldItemSocket"));
			break;
		}
	}
}

void APrototype2Character::Multi_DropWeapon_Implementation()
{
	CurrentWeaponSeedData = DefaultWeaponSeedData;
	
	if (!IsValid(Weapon))
		return;
	
	Weapon->DestroyComponent();
	if (UWeapon* NewWeapon = NewObject<UWeapon>(this, CurrentWeaponSeedData->WeaponData->WeaponComponent))
	{
		NewWeapon->RegisterComponent();
		NewWeapon->CreationMethod = EComponentCreationMethod::Instance;
					
		Weapon = NewWeapon;
	}
	WeaponMesh->SetStaticMesh(DefaultWeaponSeedData->BabyMesh);
	AttackAreaIndicatorMesh->SetStaticMesh(CurrentWeaponSeedData->WeaponData->AOEIndicatorMesh);
	CurrentWeaponAnimation = CurrentWeaponSeedData->WeaponData->WeaponAnimationType;
	SetPlayerAimingMovement(false);
}

void APrototype2Character::Server_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& _InMaterialsArray)
{
	Multi_ReceiveMaterialsArray(_InMaterialsArray);
}

void APrototype2Character::Multi_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& _InMaterialsArray)
{
	USkeletalMeshComponent* SkellyMeshComponent = GetMesh();
	if (SkellyMeshComponent && _InMaterialsArray.Num() > 0)
	{
		for (int32 i = 0; i < _InMaterialsArray.Num(); i++)
		{
			SkellyMeshComponent->SetMaterial(i, _InMaterialsArray[i]);
		}
	}
}