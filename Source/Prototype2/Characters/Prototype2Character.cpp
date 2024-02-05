

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
#include "Prototype2/Pickups/GrowableWeapon.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/Pickups/Weapons/WeaponLeekSword.h"
#include "Prototype2/Pickups/Weapons/WeaponPunching.h"
#include "Prototype2/Spawning/FertiliserSpawner.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/VFX/VFXComponent.h"
#include "Prototype2/Gameplay/Smite.h"
#include "Prototype2/GameUserSettings/HarvestHavocGameUserSettings.h"
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
	DOREPLIFETIME(APrototype2Character, WeaponCurrentDurability);
	//DOREPLIFETIME(APrototype2Character, bIsHoldingGold);
	DOREPLIFETIME(APrototype2Character, InteractTimer);
	DOREPLIFETIME(APrototype2Character, ClaimedPlot);
	DOREPLIFETIME(APrototype2Character, DebuffComponent);
	DOREPLIFETIME(APrototype2Character, bAllowMovementFromInput);
	DOREPLIFETIME(APrototype2Character, FallTimer);
	DOREPLIFETIME(APrototype2Character, CurrentWeaponSeedData);
	DOREPLIFETIME(APrototype2Character, CurrentWeaponAnimation);
	DOREPLIFETIME(APrototype2Character, bIsHoldingGoldWeapon);
	DOREPLIFETIME(APrototype2Character, bIsHoldingInteract);
	DOREPLIFETIME(APrototype2Character, WeaponFlashTimer);
	DOREPLIFETIME(APrototype2Character, bShouldWeaponFlashRed);
	DOREPLIFETIME(APrototype2Character, WeaponMesh);
	DOREPLIFETIME(APrototype2Character, bSprintAnimationState);
	
	// Sprint
	// DOREPLIFETIME(APrototype2Character, bHasNotifiedCanSprint);
	// DOREPLIFETIME(APrototype2Character, SprintTimer);
	// DOREPLIFETIME(APrototype2Character, bSprinting);
	// DOREPLIFETIME(APrototype2Character, bCanSprint);
	// DOREPLIFETIME(APrototype2Character, CurrentMaxWalkSpeed);
	// DOREPLIFETIME(APrototype2Character, DelayedSprintRegenTimer);
	
	// Niagara Components
	DOREPLIFETIME(APrototype2Character, AttackAreaIndicatorMesh);
	DOREPLIFETIME(APrototype2Character, Dizzy_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, WalkPoof_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, SprintPoof_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, Sweat_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, AttackTrail_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, Attack_NiagaraComponent);
}

void APrototype2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	AssignHelperReferences();
	InitInputMappingContext();
	InitPlayerHUD();
	InitDecals();
	InitMeshAndCapsule();
	InitWeapon();
	InitShippingBin();
}

void APrototype2Character::DelayedBeginPlay()
{
	if (!PlayerStateRef)
		PlayerStateRef = GetPlayerState<APrototype2PlayerState>();

	if (PlayerMaterialsDynamic.Num() <= 0)
	{
		for(UMaterialInstance* Material : PlayerMaterials)
		{
			PlayerMaterialsDynamic.Add(UMaterialInstanceDynamic::Create(Material,this));
		}
	}
	InitPlayerNameWidgetComponent();
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
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APrototype2Character::Interact);
		// Hold Interact
		EnhancedInputComponent->BindAction(HoldInteractAction, ETriggerEvent::Ongoing, this, &APrototype2Character::HoldInteract);
		EnhancedInputComponent->BindAction(ReleaseHoldInteractAction, ETriggerEvent::Triggered, this, &APrototype2Character::ReleaseHoldInteract);
		
		// UI
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this, &APrototype2Character::OpenIngameMenu);
		EnhancedInputComponent->BindAction(ShowEmoteAction, ETriggerEvent::Triggered, this, &APrototype2Character::ShowRadialEmoteMenu);
		EnhancedInputComponent->BindAction(DisableEmoteAction, ETriggerEvent::Triggered, this, &APrototype2Character::DisableRadialEmoteMenu);

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APrototype2Character::Jump);
	}
}

void APrototype2Character::AssignHelperReferences()
{
	PlayerController = Cast<APrototype2PlayerController>(Controller);
	GameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
}

void APrototype2Character::InitInputMappingContext()
{
	if (PlayerController)
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

void APrototype2Character::Multi_SetSmite(Smite* _smite)
{
	smite = _smite;
	smite->SetPlayer(this);
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
	
	if (smite)
	{
		smite->Tick(_DeltaSeconds);
	}
}

void APrototype2Character::TogglePlayerStencil()
{
	auto MyPlayerState = GetPlayerState<APrototype2PlayerState>();
	if (!MyPlayerState)
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
	if (!Controller || !bAllowMovementFromInput || bIsHoldingInteract)
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
	if (!Controller)
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
	if (!DebuffComponent)
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
	Super::Jump();
}

void APrototype2Character::ChargeAttack()
{
	if (!Weapon)
		return;
	
	if (bIsHoldingInteract)
		return;

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
	if (HeldItem)
		return;
	
	//DropItem();

	if (AttackTimer <= 0.0f)
	{
		AttackTimer = AttackTimerTime;
		
		Weapon->Client_ChargeAttack(this);
		
		bIsChargingAttack = true;

		if (HasAuthority())
		{
			WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("Base-HumanWeapon"));

			// Charge animation
			bChargeAnimationState = true;
			OnRep_ChargeStateChanged();
			
			Weapon->ChargeAttack(this);
			AttackAreaIndicatorMesh->SetHiddenInGame(false);
		}
		else
		{
			Server_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
			Server_ChargeAttack();
			Server_SetAOEIndicatorVisibility(true);
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
	if (HeldItem)
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

	if (!Weapon)
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

	bIsFollowingThroughAttack = true;
	
	Server_ReleaseAttack(GetActorLocation(), GetActorForwardVector());


	// Set the radius of the sphere for attack
	int32 AttackSphereRadius = CurrentWeaponSeedData->WeaponData->BaseAttackRadius + AttackChargeAmount * CurrentWeaponSeedData->WeaponData->AOEMultiplier;
	

	// Always delay attack because of the new launch
	bool bFullChargeAttack = true;
	if (AttackChargeAmount < MaxAttackCharge) // InstantAttackThreshold
		{
		bFullChargeAttack = false;
		}
	
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

void APrototype2Character::Interact()
{
	if (bIsChargingAttack)
		return;

	if (InteractTimer > 0)
		return;

	if (ClosestInteractableItem)
		ClosestInteractableItem->ClientInteract(this);
	
	Server_Interact();
	RefreshCurrentMaxSpeed();
}

void APrototype2Character::Server_Interact_Implementation()
{
	if (ClosestInteractableItem)
	{
		ClosestInteractableItem->Interact(this);

		if (!bIsHoldingInteract)
		InteractTimer = InteractTimerTime;
		
		if (HeldItem)
		{
			if (AnimationData->Pickup)
			{
				PlayNetworkMontage(AnimationData->Pickup);
			}
			if (Weapon)
			{
				Multi_SocketItem(WeaponMesh, FName("WeaponHolsterSocket"));
			}
		}
	}

	ClosestInteractableActor = nullptr;
	if (!bIsHoldingInteract)
	ClosestInteractableItem = nullptr;
}

void APrototype2Character::HoldInteract()
{
	// This will get set in the items interact function
	if (bIsHoldingInteract)
		Server_HoldInteract();
}

void APrototype2Character::ReleaseHoldInteract()
{
	Server_ReleaseHoldInteract();
}

void APrototype2Character::Server_HoldInteract_Implementation()
{
	if (ClosestInteractableItem)
		ClosestInteractableItem->HoldInteract(this);	
}

void APrototype2Character::OpenIngameMenu()
{
	if (!PlayerHUDRef)
		return;

	PlayerHUDRef->EnableDisableMenu();
}

void APrototype2Character::ShowRadialEmoteMenu()
{
	if (!PlayerHUDRef)
		return;

	PlayerHUDRef->ShowEmoteRadialMenu();
}

void APrototype2Character::DisableRadialEmoteMenu()
{
	if (!PlayerHUDRef)
		return;

	PlayerHUDRef->DisableEmoteRadialMenu();
}

void APrototype2Character::ExecuteAttack(float _AttackSphereRadius, float _AttackChargeAmount, bool _bSprinting)
{
	Server_ExecuteAttack(_AttackSphereRadius, _AttackChargeAmount, _bSprinting);
}

void APrototype2Character::Server_ExecuteAttack_Implementation(float _AttackSphereRadius, float _AttackChargeAmount, bool _bSprinting)
{
	Weapon->ExecuteAttack(_AttackSphereRadius, this, _AttackChargeAmount, _bSprinting);
}

void APrototype2Character::DropItem(float WhoopsyStrength)
{
	if (HeldItem)
	{
		// Local Stuff
		if (PlayerHUDRef)
		{
			PlayerHUDRef->ClearPickupUI();
			PlayerHUDRef->SetHUDInteractText("");
		}
		HeldItem->Client_Drop();

		bIsHoldingGold = false;
		
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
			HeldItem->ItemComponent->Mesh->AddImpulse(FVector::UpVector * WhoopsyStrength, NAME_None, true);
			
			HeldItem = nullptr;
			//
		}
		else
		{
			Server_DropItem(WhoopsyStrength);
		}
		RefreshCurrentMaxSpeed();
	}
}

void APrototype2Character::CheckForInteractables()
{
	if (!HasIdealRole())
		return;

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
			if (ABeehive* SomeBeeHive = Cast<ABeehive>(SomeGrowSpot->GrowingItemRef))
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
	if (HeldItem)
	{
		HeldItem->ItemComponent->Mesh->SetRenderCustomDepth(_bIsOn);
	}
	
	if (!ClosestInteractableActor)
	{
		return;
	}

	if (HeldItem)
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
	
}

void APrototype2Character::GetHit(float _AttackCharge, FVector _AttackerLocation, UWeaponData* _OtherWeaponData)
{
	// Can't pickup for short duration
	InteractTimer = InteractTimerTime;

	// cancel any holding interact functionionality
	bIsHoldingInteract = false;
	
	// Invincible after getting hit
	if (InvincibilityTimer > 0.0f)
	{
		return;
	}
	InvincibilityTimer = InvincibilityDuration;
	
	if (DebuffComponent)
	{
		if (DebuffComponent->CurrentDebuff == EDebuff::None)
		{
			DebuffComponent->DebuffInfo.Debuff = _OtherWeaponData->Debuff;
			if (_AttackCharge > MaxAttackCharge)
			{
				DebuffComponent->DebuffInfo.Duration = MaxAttackCharge;
			}
			else
			{
				DebuffComponent->DebuffInfo.Duration = _AttackCharge;
			}
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
	GetCharacterMovement()->Launch(KnockAway);
	
	// Drop item
	DropItem(1000.0f);
	
	PlayWeaponSound(GetHitCue);
	
	// VFX
	FVector AttackVFXLocation = _AttackerLocation - GetActorLocation();
	AttackVFXLocation = AttackVFXLocation.GetSafeNormal();
	AttackVFXLocation *= 50.0f;
	AttackVFXLocation += GetActorLocation();
	//Attack_NiagaraComponent->SetWorldLocation(AttackVFXLocation);
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
	{
		ActivateParticleSystemFromEnum(EParticleSystems::Attack);
	}

	if (HasAuthority())
	{
		Multi_CancelChargeAttack();
	}
	else
	{
		CancelChargeAttack();
	}
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
	AttackAreaIndicatorMesh->SetIsReplicated(true);
	
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
}

void APrototype2Character::UpdateDecalAngle()
{
	if (!SellBin)
		return;

	const FVector CurrentPlayerPosition = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
	const FVector SellPosition = FVector(SellBin->GetActorLocation().X, SellBin->GetActorLocation().Y, 0);

	DecalComponent->SetVisibility(false);
	if (!HeldItem)
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
	if (Weapon)
		Weapon->UpdateAOEIndicator(this, AttackChargeAmount);
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
	if (!PlayerStateRef)
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
	if (AnimationData->FallOnButtAndGetBackUp)
	{
		if (!bIsChargingAttack)
		{
			PlayNetworkMontage(AnimationData->FallOnButtAndGetBackUp);
		}
		OnFallOnButtDelegate.Broadcast();
	}
}

void APrototype2Character::ActivateParticleSystemFromEnum(EParticleSystems _NewSystem)
{
	switch (_NewSystem)
	{
	case EParticleSystems::WalkPoof:
		{
			VFXComponent->ActivateParticleSystemFromEnum(WalkPoof_NiagaraComponent);
			break;
		}
	case EParticleSystems::SprintPoof:
		{
			VFXComponent->ActivateParticleSystemFromEnum(SprintPoof_NiagaraComponent);
			break;
		}
	case EParticleSystems::Sweat:
		{
			VFXComponent->ActivateParticleSystemFromEnum(Sweat_NiagaraComponent);
			break;
		}
	case EParticleSystems::AttackTrail:
		{
				VFXComponent->ActivateParticleSystemFromEnum(AttackTrail_NiagaraComponent);
			break;
		}
	case EParticleSystems::Attack:
		{
			VFXComponent->ActivateParticleSystemFromEnum(Attack_NiagaraComponent);
			break;
		}
	case EParticleSystems::Dizzy:
		{
			VFXComponent->ActivateParticleSystemFromEnum(Dizzy_NiagaraComponent);
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
	switch (_NewSystem)
	{
	case EParticleSystems::WalkPoof:
		{
			VFXComponent->DeActivateParticleSystemFromEnum(WalkPoof_NiagaraComponent);
			break;
		}
	case EParticleSystems::SprintPoof:
		{
			VFXComponent->DeActivateParticleSystemFromEnum(SprintPoof_NiagaraComponent);
			break;
		}
	case EParticleSystems::Sweat:
		{
			VFXComponent->DeActivateParticleSystemFromEnum(Sweat_NiagaraComponent);
			break;
		}
	case EParticleSystems::AttackTrail:
		{
			VFXComponent->DeActivateParticleSystemFromEnum(AttackTrail_NiagaraComponent);
			break;
		}
	case EParticleSystems::Attack:
		{
			VFXComponent->DeActivateParticleSystemFromEnum(Attack_NiagaraComponent);
			break;
		}
	case EParticleSystems::Dizzy:
		{
			VFXComponent->DeActivateParticleSystemFromEnum(Dizzy_NiagaraComponent);
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

void APrototype2Character::Server_ToggleParticleSystems_Implementation(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off)
{
	Multi_ToggleParticleSystems(_On, _Off);
}

void APrototype2Character::Multi_ToggleParticleSystems_Implementation(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off)
{
	
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

void APrototype2Character::Server_TeleportToLocation_Implementation(FVector _DestinationLocation, FRotator _DestinationRotation)
{
	SetActorLocation(_DestinationLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(_DestinationRotation, ETeleportType::TeleportPhysics);
}

bool APrototype2Character::HasClaimedPlot()
{
	if (ClaimedPlot)
		return true;

	return false;
}

void APrototype2Character::SetClaimedPlot(ARadialPlot* _Plot)
{
	ClaimedPlot = _Plot;
	OnPlotClaimedDelegate.Broadcast(_Plot);
	Client_BroadcastPlotClaimed(_Plot);
}

void APrototype2Character::Client_BroadcastPlotClaimed_Implementation(ARadialPlot* _Plot)
{
	OnPlotClaimedDelegate.Broadcast(_Plot);
}

void APrototype2Character::Client_OnStoppedClaimingPlot_Implementation()
{
	OnStoppedClaimingPlotDelegate.Broadcast();
}

void APrototype2Character::Client_OnClaimingPlot_Implementation(float _CurrentClaimTime, float _TotalClaimDuration)
{
	OnClaimingPlotDelegate.Broadcast(_CurrentClaimTime, _TotalClaimDuration);
}

void APrototype2Character::Server_ReleaseHoldInteract_Implementation()
{
	bIsHoldingInteract = false;
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
	Client_DropWeapon();
	Server_DropWeapon();
}

void APrototype2Character::SetPlayerAimingMovement(bool _bIsAiming)
{

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
	bAllowMovementFromInput = !_bIsAiming;

	Server_SetPlayerAimingMovement(_bIsAiming);
}

void APrototype2Character::Multi_SetPlayerAimingMovement_Implementation(bool _bIsAiming)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
	bAllowMovementFromInput = !_bIsAiming;
}

void APrototype2Character::Client_SetPlayerAimingMovement_Implementation(bool _bIsAiming)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
	bAllowMovementFromInput = !_bIsAiming;
}

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
	if (PlayerHUDRef)
		PlayerHUDRef->SetHUDInteractText("");

	if (HeldItem)
		HeldItem->Client_Pickup();
	
	// Set the HUD UI pickup icon depending on seed/plant/weapon
	switch (_PickupType)
	{
	case EPickupActor::PlantActor:
		{
			if (!PlayerHUDRef || !_Item->SeedData->PlantData)
			{
				break;
			}
			if (_Item->ItemComponent->bGold)
			{
				PlayerHUDRef->UpdatePickupUI(_Item->SeedData->PlantData->GoldPlantIcon);
				bIsHoldingGold = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Plant was normal when picked up for me"));
				PlayerHUDRef->UpdatePickupUI(_Item->SeedData->PlantData->PlantIcon);
				bIsHoldingGold = false;
			}
			OnPickUpItemDelegate.Broadcast();
			break;
		}
	case EPickupActor::WeaponActor:
		{
			// Set UI
			if (PlayerHUDRef && _Item->SeedData->WeaponData)
			{
				PlayerHUDRef->ClearPickupUI();
				if (_Item->ItemComponent->bGold)
				{
					PlayerHUDRef->WeaponImage->SetBrushFromTexture(_Item->SeedData->WeaponData->GoldWeaponIcon);
				}
				else
				{
					PlayerHUDRef->WeaponImage->SetBrushFromTexture(_Item->SeedData->WeaponData->WeaponIcon);
				}
				FVector2D ImageSize ={(float)_Item->SeedData->WeaponData->WeaponIcon->GetSizeX(), (float)_Item->SeedData->WeaponData->WeaponIcon->GetSizeY()};
				PlayerHUDRef->WeaponImage->SetDesiredSizeOverride(ImageSize);
				OnPickUpWeaponDelegate.Broadcast();
			}			
			break;
		}
	case EPickupActor::SeedActor:
		{
			if (PlayerHUDRef)
			{
				PlayerHUDRef->UpdatePickupUI(_Item->SeedData->PacketIcon);
			}
			OnPickUpItemDelegate.Broadcast();
			break;
		}
	case EPickupActor::FertilizerActor:
		{
			if (!PlayerHUDRef)
				break;
			
			if (_Item->SeedData->PacketIcon)
			{
				PlayerHUDRef->UpdatePickupUI(_Item->SeedData->PacketIcon);
			}
			OnPickUpItemDelegate.Broadcast();
		}
	default:
		break;
	}
}

void APrototype2Character::Client_DropItem_Implementation()
{
	if (HeldItem)
	{
		HeldItem->Client_Drop();
	}
	// clear pickup UI and text
	if (PlayerHUDRef)
	{
		PlayerHUDRef->ClearPickupUI();
		PlayerHUDRef->SetHUDInteractText("");
	}
}

void APrototype2Character::Client_DropWeapon_Implementation()
{
	// Set UI
	if (PlayerHUDRef && DefaultWeaponSeedData)
	{
		PlayerHUDRef->WeaponImage->SetBrushFromTexture(DefaultWeaponSeedData->WeaponData->WeaponIcon);
		FVector2D ImageSize ={(float)DefaultWeaponSeedData->WeaponData->WeaponIcon->GetSizeX(), (float)DefaultWeaponSeedData->WeaponData->WeaponIcon->GetSizeY()};
		PlayerHUDRef->WeaponImage->SetDesiredSizeOverride(ImageSize);
	}
}

void APrototype2Character::Server_SetPlayerAimingMovement_Implementation(bool _bIsAiming)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = _bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
	bAllowMovementFromInput = !_bIsAiming;

	Multi_SetPlayerAimingMovement(_bIsAiming);
}

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
	if (bIsHoldingGold)
	{
		if (bSprinting)
		{
			SetMaxWalkSpeed(WalkSpeed);
			DeActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
			ActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
			return;
		}
		SetMaxWalkSpeed(GoldPlantSpeed);
		DeActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
		return;
	}

	// sprint
	if (bSprinting)
	{
		SetMaxWalkSpeed(SprintSpeed);
		DeActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
		ActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
		return;
	}

	// walk
	SetMaxWalkSpeed(WalkSpeed);
	DeActivateParticleSystemFromEnum(EParticleSystems::SprintPoof);
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
	
	// Reset Attack Timer
	if (HasAuthority())
	{
		// Reset Attack variables
		//bIsChargingAttack = false;

		// Charge animation
		bChargeAnimationState = false;
		OnRep_ChargeStateChanged();
		
		// Stop the player Interacting while "executing attack"
		InteractTimer = InteractTimerTime;
		
		SetPlayerAimingMovement(false);
		AttackAreaIndicatorMesh->SetHiddenInGame(false);
	}
	else
	{
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_ResetAttack();
			Server_SetPlayerAimingMovement(false);
			Server_SetAOEIndicatorVisibility(false);
		}
	}
}

void APrototype2Character::Server_ResetAttack_Implementation()
{
	// Reset Attack variables
	
	// Charge animation
	bChargeAnimationState = false;
	OnRep_ChargeStateChanged();

	//AutoAttackTimer = 0.0f;

	// Stop the player Interacting while "executing attack"
	InteractTimer = InteractTimerTime;

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
	// Local Stuff
	if (PlayerHUDRef)
	{
		PlayerHUDRef->ClearPickupUI();
		PlayerHUDRef->SetHUDInteractText("");
	}
	HeldItem->Client_Drop();
		
	bIsHoldingGold = false;

	if (HasAuthority())
	{
		// Server_DropItem
		HeldItem->Server_Drop();
			
		if (DropCue)
			PlaySoundAtLocation(GetActorLocation(), DropCue);
		if (WeaponMesh)
			Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));

		FVector ThrowVector = GetActorForwardVector();
		ThrowVector *= ThrowItemStrength + GetCharacterMovement()->GetLastUpdateVelocity().Length();
		ThrowVector.Z = 500.0f;
		Multi_ThrowItem();
		HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
		HeldItem->ItemComponent->Mesh->AddImpulse(ThrowVector, NAME_None, true);
		
		HeldItem = nullptr;
	}
	else
	{
		Server_ThrowItem();
	}
	RefreshCurrentMaxSpeed();
}

void APrototype2Character::Server_ApplyHoneyGoopSlow_Implementation()
{
	DebuffComponent->DebuffInfo.Debuff = EDebuff::Slow;
	DebuffComponent->DebuffInfo.Duration = 0.3f;
}

void APrototype2Character::Multi_ThrowItem_Implementation()
{
	// Drop into world
	if (!HeldItem)
		return;
	
	HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
	HeldItem->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// So that CheckForInteractables() can see it again
	HeldItem->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
}

void APrototype2Character::Server_ThrowItem_Implementation()
{
	HeldItem->Server_Drop();
	
	// Server_DropItem
	if (DropCue)
		PlaySoundAtLocation(GetActorLocation(), DropCue);
	if (WeaponMesh)
		Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));

	FVector ThrowVector = GetActorForwardVector();
	ThrowVector *= ThrowItemStrength + GetCharacterMovement()->GetLastUpdateVelocity().Length();
	ThrowVector.Z = 500.0f;
	
	Multi_ThrowItem();
	HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
	HeldItem->ItemComponent->Mesh->SetWorldLocation(GetActorLocation()+GetActorForwardVector()* 100.0f);
	HeldItem->ItemComponent->Mesh->AddImpulse(ThrowVector, NAME_None, true);

	bIsHoldingGold = false;	
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

void APrototype2Character::TeleportToEndGame(FTransform _EndGameTransform)
{
	if (HasAuthority())
	{
		Multi_TeleportToEndGame(_EndGameTransform);
	}
	else
	{
		Server_TeleportToEndGame(_EndGameTransform);
	}
}

void APrototype2Character::Server_TeleportToEndGame_Implementation(FTransform _EndGameTransform)
{
	Multi_TeleportToEndGame(_EndGameTransform);
	bIsHoldingInteract = false;
}

void APrototype2Character::Multi_TeleportToEndGame_Implementation(FTransform _EndGameTransform)
{
	PlayerNameWidgetComponent->SetVisibility(true);
	EmoteWidgetComponent->SetVisibility(true);
	
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
	if (!PlayerStateRef || !GameState)
	{
		return false;
	}
	
	if (PlayerStateRef->Player_ID == GameState->GetPlayerWinner())
	{
		return true;
	}

	return false;
	
}

void APrototype2Character::InitPlayerNameWidgetComponent()
{
	if (PlayerNameWidget)
		return;
		
	if (!PlayerNameWidgetComponent)
		return;

	UUserWidget* Widget = PlayerNameWidgetComponent->GetWidget();
	if (!Widget)
		return;

	UWidget_PlayerName* NameWidget = Cast<UWidget_PlayerName>(Widget);
	if (!NameWidget)
		return;

	if (!PlayerStateRef)
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
	if (EmoteWidget)
		return;
		
	if (!EmoteWidgetComponent)
		return;

	UUserWidget* Widget = EmoteWidgetComponent->GetWidget();
	if (!Widget)
		return;

	UWidget_PlayerEmote* EmoteRadialWidget = Cast<UWidget_PlayerEmote>(Widget);
	if (!EmoteRadialWidget)
		return;

	if (!PlayerStateRef)
		return;
	
	EmoteWidget = EmoteRadialWidget;
	EmoteWidget->SetPlayerRef(PlayerStateRef);
}

void APrototype2Character::InitWeapon()
{
	AutoAttackTimer = 0.0f;
	bCanAttack = true;
}
void APrototype2Character::InitPlayerHUD()
{
	if (PlayerHUDRef)
		return;

	if (!PlayerController)
		return;
	
	if (!PlayerHudPrefab)
		return;
	
	if (!HasNetOwner())
		return;
	
	PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(PlayerController, PlayerHudPrefab);
	PlayerHUDRef->AddToViewport();

	PlayerHUDRef->WeaponImage->SetBrushFromTexture(CurrentWeaponSeedData->WeaponData->WeaponIcon);
}

void APrototype2Character::InitShippingBin()
{
	if (SellBin)
		return;
	
	if (!HasIdealRole())
		return;
	
	AActor* FoundSellBin = UGameplayStatics::GetActorOfClass(GetWorld(), ASellBin::StaticClass());
	if (!FoundSellBin)
		return;

	SellBin = Cast<ASellBin>(FoundSellBin);
}

void APrototype2Character::InitDecals()
{
	if (!DecalComponent)
		return;
	
	DecalComponent->SetIsReplicated(false);
	DecalComponent->SetVisibility(false);

	if (!DecalArmSceneComponent)
		return;
	
	DecalArmSceneComponent->SetIsReplicated(false);
}

void APrototype2Character::ClearInteractionText()
{
	if (!PlayerHUDRef)
		return;
	
	PlayerHUDRef->SetHUDInteractText("");
}

bool APrototype2Character::GetIsPunching()
{
	if (!DefaultWeaponSeedData || !CurrentWeaponSeedData)
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
			AttackChargeAmount += _DeltaSeconds;
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
		if (Weapon)
			Weapon->UpdateAOEIndicator(this, AttackChargeAmount);
	}
	else
	{
		AttackAreaIndicatorMesh->SetHiddenInGame(true);
	}
}

void APrototype2Character::UpdatePlayerHUD()
{
	if (!PlayerHUDRef)
		return;

	PlayerHUDRef->SetPlayerSprintTimer(SprintTimer);

	if(!HeldItem && !Weapon)
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
	if (!HasIdealRole())
		return;
	
	if (GetCharacterMovement()->Velocity.Size() < 50.0f)
	{
		DeActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
	}
	else
	{
		ActivateParticleSystemFromEnum(EParticleSystems::WalkPoof);
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

	if (HasAuthority())
	{
		DeltaDecrement(InteractTimer, _DeltaSeconds);
		DeltaDecrement(InvincibilityTimer, _DeltaSeconds);
		DeltaDecrement(WeaponFlashTimer, _DeltaSeconds);
		if (WeaponFlashTimer < 0.0f)
		{
			bShouldWeaponFlashRed = false;
			WeaponFlashTimer = WeaponFlashDuration;
		}

	}
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

void APrototype2Character::SocketWeapon(FName _Socket)
{
	if (!WeaponMesh)
		return;
	
	Server_SocketItem(WeaponMesh, _Socket);
}

void APrototype2Character::Server_AddHUD_Implementation()
{
	Client_AddHUD();
}

void APrototype2Character::Multi_Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef && PlayerStateRef)
    {
    	PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), PlayerStateRef->Player_ID), PlayerHudPrefab);
   
    	if (PlayerHUDRef)
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

void APrototype2Character::Server_ReleaseAttack_Implementation(FVector _CachedActorLocation, FVector _CachedForwardVector)
{
	// Socket Weapon back to held
	Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
	

}

void APrototype2Character::Multi_ReleaseAttack_Implementation()
{
}

void APrototype2Character::Server_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation)
{
	Multi_PlaySoundAtLocation(_Location, _SoundQueue, _Attenuation);
}

void APrototype2Character::Multi_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation)
{
	if (_Attenuation)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _SoundQueue, _Location, 1, 1, 0, _Attenuation);
	else
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _SoundQueue, _Location, 1, 1, 0, SoundAttenuationSettings);

}

void APrototype2Character::Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef)
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
	HeldItem->Server_Drop();
	
	// Server_DropItem
	if (DropCue)
		PlaySoundAtLocation(GetActorLocation(), DropCue);
	if (WeaponMesh)
		Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
			
	Multi_DropItem();
	HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
	HeldItem->ItemComponent->Mesh->AddImpulse(FVector::UpVector * WhoopsyStrength, NAME_None, true);

	bIsHoldingGold = false;

	HeldItem = nullptr;
}

void APrototype2Character::Multi_DropItem_Implementation()
{
	// Drop into world
	if (!HeldItem)
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
	
	if (HeldItem)
	{
		if (HasAuthority())
			HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
	
		Multi_DropItem();
	}

	
	_Item->ItemComponent->Mesh->SetSimulatePhysics(false);
	switch (_PickupType)
	{
	case EPickupActor::PlantActor:
		{
			HeldItem = _Item;
			if (_Item->ItemComponent->bGold)
			{
				bIsHoldingGold = true;
			}
			else
			{
				bIsHoldingGold = false;
			}
			Multi_SocketItem_Implementation(WeaponMesh, FName("WeaponHolsterSocket"));
			break;
		}
	case EPickupActor::WeaponActor:
		{
			// Replace weapon with new one
			if (Weapon)
				Weapon->DestroyComponent();
			if (UWeapon* NewWeapon = NewObject<UWeapon>(this, _Item->SeedData->WeaponData->WeaponComponent))
			{
				NewWeapon->RegisterComponent();
				NewWeapon->SetIsReplicated(true);
				Weapon = NewWeapon;
			}
			
			CurrentWeaponSeedData = _Item->SeedData;
			
			WeaponCurrentDurability = CurrentWeaponSeedData->WeaponData->Durability;
			if (_Item->ItemComponent->bGold)
			{
				WeaponCurrentDurability *= CurrentWeaponSeedData->WeaponData->GoldDurabilityMultiplier;
				bIsHoldingGoldWeapon = true;
			}
			else
			{
				bIsHoldingGoldWeapon = false;
			}

			CurrentWeaponAnimation = _Item->SeedData->WeaponData->WeaponAnimationType;
			_Item->Destroy();

			Multi_SocketItem_Implementation(WeaponMesh, FName("Base-HumanWeapon"));
			break;
		}
	default:
		{
			HeldItem = _Item;
			
			Multi_SocketItem_Implementation(WeaponMesh, FName("WeaponHolsterSocket"));
			break;
		}
	}

	Multi_PickupItem(_Item, _PickupType);
}

void APrototype2Character::Server_SocketItem_Implementation(UStaticMeshComponent* _Object, FName _Socket)
{
	_Object->SetSimulatePhysics(false);
	_Object->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_Object->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	_Object->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(_Socket));
}

void APrototype2Character::Server_DropWeapon_Implementation()
{
	Multi_DropWeapon();
}

void APrototype2Character::Multi_PickupItem_Implementation(APickUpItem* _Item, EPickupActor _PickupType)
{
	if (!_Item)
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
			AttackAreaIndicatorMesh->SetStaticMesh(_Item->SeedData->WeaponData->AOEIndicatorMesh);
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
	
	if (!Weapon)
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
	Server_SetPlayerAimingMovement(false);
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