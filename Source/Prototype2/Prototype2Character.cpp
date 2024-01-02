// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GrowSpot.h"
#include "InteractInterface.h"
#include "PickUpItem.h"
#include "Prototype2GameMode.h"
#include "Prototype2PlayerController.h"
#include "Prototype2PlayerState.h"
#include "PrototypeGameInstance.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "DynamicMesh/ColliderMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Widget_InteractionPanel.h"
#include "Widgets/Widget_IngameMenu.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Gamestates/Prototype2Gamestate.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Widget_PlayerHUD.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


APrototype2Character::APrototype2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Set collisions	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Weapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));
	Weapon->Mesh->SetSimulatePhysics(false);
	Weapon->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	ChargeAttackAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ChargeAttackAudioComponent"));

	InteractSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	InteractSystem->SetupAttachment(RootComponent);

	DizzyComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Dizzy Component"));
	DizzyComponent->SetupAttachment(GetMesh(), FName("Base-HumanHead"));
}

void APrototype2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2Character, Weapon);
	DOREPLIFETIME(APrototype2Character, HeldItem);
	DOREPLIFETIME(APrototype2Character, PlayerMat);
	DOREPLIFETIME(APrototype2Character, PlayerID);
	DOREPLIFETIME(APrototype2Character, bIsChargingAttack);
	DOREPLIFETIME(APrototype2Character, AttackChargeAmount);
	DOREPLIFETIME(APrototype2Character, bIsStunned);
	DOREPLIFETIME(APrototype2Character, StunTimer);
	DOREPLIFETIME(APrototype2Character, LocationWhenStunned);
}

void APrototype2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	ChargeAttackAudioComponent->SetSound(ChargeCue);

	Weapon->Mesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("WeaponHolsterSocket"));

	if (PlayerHudPrefab && !PlayerHUDRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player HUD Created"));
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(Cast<APrototype2PlayerController>(Controller), PlayerHudPrefab);

		if (PlayerHUDRef)
			PlayerHUDRef->AddToViewport();
	}

	if (ParticleSystem)
	{
		InteractSystem->SetAsset(ParticleSystem);
	}
}

void APrototype2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GetMesh()->SetMaterial(0, PlayerMat);
	
	UpdateAllPlayerIDs();
	
	// Stun
	if (bIsStunned)
	{
		
		bIsChargingAttack = false;

		// Set animation to stun
		
		StunTimer -= DeltaSeconds;
		if (StunTimer < 0.0f)
		{
			bIsStunned = false;
			//Server_Ragdoll(false);
			// Enable input
			EnableInput(this->GetLocalViewingPlayerController());
		}
	}

	// Sprint
	if (SprintTimer < 0.0f && CanSprintTimer)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		if (RunAnimation)
		{
			RunAnimation->RateScale = 1.25f;
		}
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		if(RunAnimation)
		{
			RunAnimation->RateScale = 2.50f;
		}
	}
	
	// Attack
	if (bIsChargingAttack)
	{
		AttackChargeAmount += DeltaSeconds;
	}

	if (InteractTimer < 0.0f)
	{
		// Check if anything is around to be interacted with
		CheckForInteractables();
	}

	// Countdown timers
	InteractTimer -= DeltaSeconds;
	AttackTimer -= DeltaSeconds;
	SprintTimer -= DeltaSeconds;
	CanSprintTimer -= DeltaSeconds;

	if (GetVelocity().Length() > 1.0f)
	{
		InteractSystem->Activate();		
	}
	else
	{
		InteractSystem->Deactivate();
	}
}

void APrototype2Character::ChargeAttack()
{
	Server_StartAttack();
}

void APrototype2Character::ReleaseAttack()
{
	// 
	Server_ReleaseAttack();
}

void APrototype2Character::ExecuteAttack(float AttackSphereRadius)
{
	// create tarray for hit results
	TArray<FHitResult> outHits;
	
	// start and end locations
	FVector inFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * AttackSphereRadius) + (GetActorForwardVector() * 30.0f);
	FVector sweepStart = inFrontOfPlayer;
	FVector sweepEnd = inFrontOfPlayer;

	// create a collision sphere
	FCollisionShape colSphere = FCollisionShape::MakeSphere(AttackSphereRadius);
	
	UE_LOG(LogTemp, Warning, TEXT("Sphere Radius = %s"), *FString::SanitizeFloat(AttackSphereRadius));
	// draw collision sphere
	//DrawDebugSphere(GetWorld(), inFrontOfPlayer, colSphere.GetSphereRadius(), 50, FColor::Purple, false, 2.0f);
	
	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(outHits, sweepStart, sweepEnd, FQuat::Identity, ECC_Pawn, colSphere);

	if (isHit)
	{
		// loop through TArray
		for (auto& hit : outHits)
		{
			if (auto* hitPlayer = Cast<APrototype2Character>(hit.GetActor()))
			{
				if (hitPlayer != this)
				{
					// screen log information on what was hit
					UE_LOG(LogTemp, Warning, TEXT(" %s  was hit by an attack!"), *hit.GetActor()->GetName());

					FVector attackerLocation = GetActorLocation();
					hitPlayer->GetHit(AttackChargeAmount, attackerLocation);
				}
			}
		}
	}
	
	// Reset Attack Timer
	AttackTimer = AttackTimerTime;

	// Reset Attack variables
	bIsChargingAttack = false;
	AttackChargeAmount = 0.0f;

	// audio
	ChargeAttackAudioComponent->Stop();
	PlaySoundAtLocation(GetActorLocation(), ExecuteCue);

	// Stop the player Interacting while "executing attack"
	InteractTimer = InteractTimerTime;

	bCanAttack = true;
}

void APrototype2Character::Interact()
{
	if (InteractTimer < 0.0f)
	{
		// Reset the Interact Timer when Player Interacts
		InteractTimer = InteractTimerTime;

		if(!HeldItem)
		{
			PlayerHUDRef->UpdatePickupUI(EPickup::None);
		}
		
		if (!bIsChargingAttack)
		{
			Server_TryInteract();
		}

		if(!HeldItem)
		{
			PlayerHUDRef->UpdatePickupUI(EPickup::None);
		}

	}
	
	// Debug draw collision sphere
	//FCollisionShape colSphere = FCollisionShape::MakeSphere(InteractRadius);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), colSphere.GetSphereRadius(), 50, FColor::Purple, false, 3.0f);
}

void APrototype2Character::Sprint()
{
	if (CanSprintTimer < 0.0f && !bIsChargingAttack)
	{
		SprintTimer = SprintTime;
		CanSprintTimer = CanSprintTime;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Time until you can sprint again: %f"), CanSprintTimer);
	}
}

void APrototype2Character::CheckForInteractables()
{
	
	// create tarray for hit results
	TArray<FHitResult> outHits;
	
	// start and end locations
	FVector sweepStart = GetActorLocation();
	FVector sweepEnd = GetActorLocation();

	// create a collision sphere
	FCollisionShape colSphere = FCollisionShape::MakeSphere(InteractRadius);

	// draw collision sphere
	//DrawDebugSphere(GetWorld(), GetActorLocation(), colSphere.GetSphereRadius(), 50, FColor::Purple, false, 0.1f);
	
	// check if something got hit in the sweep
	if (GetWorld()->SweepMultiByChannel(outHits, sweepStart, sweepEnd, FQuat::Identity, ECC_Visibility, colSphere))
	{
		TArray<AActor*> interactableActors;

		// loop through TArray
		for (auto& hit : outHits)
		{
			if (Cast<IInteractInterface>(hit.GetActor()))
			{
				if (hit.GetActor() != HeldItem)
				{
					interactableActors.Add(hit.GetActor());
				}
			}
		}

		float distanceToClosest;
		ClosestInteractableItem = Cast<IInteractInterface>(UGameplayStatics::FindNearestActor(GetActorLocation(), interactableActors, distanceToClosest));
	}
	else
	{
		ClosestInteractableItem = nullptr;
	}
}

void APrototype2Character::GetHit(float AttackCharge, FVector AttackerLocation)
{
	// Disable input
	DisableInput(this->GetLocalViewingPlayerController());

	Server_FireDizzySystem();
	//Server_Ragdoll(true);

	// Drop item
	if (HeldItem)
	{
		Server_DropItem();
	}
	
	// Knockback
	GetCharacterMovement()->Velocity = (GetActorLocation() - AttackerLocation).GetSafeNormal() * AttackCharge * KnockBackAmount;

	// debug attack
	UE_LOG(LogTemp, Warning, TEXT("AttackCharge: %s"), *FString::SanitizeFloat(AttackCharge));
	
	bIsStunned = true;
	StunTimer = 2.0f;

	PlaySoundAtLocation(GetActorLocation(), GetHitCue);
}

void APrototype2Character::Multi_SocketItem_Implementation(UStaticMeshComponent* _object, FName _socket)
{
	_object->SetSimulatePhysics(false);
	_object->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_object->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	_object->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(_socket));
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

		// UI
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this, &APrototype2Character::OpenIngameMenu);

		// Attack
		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ChargeAttack);
		EnhancedInputComponent->BindAction(ReleaseAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ReleaseAttack);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APrototype2Character::Interact);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APrototype2Character::Sprint);
	}
}

void APrototype2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
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
}

void APrototype2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APrototype2Character::OpenIngameMenu()
{
	if (auto* playercontroller = Cast<APrototype2PlayerController>(Controller))
	{
		if (PlayerHUDRef)
			PlayerHUDRef->EnableDisableMenu();
	}
}

void APrototype2Character::UpdateAllPlayerIDs()
{
}

void APrototype2Character::PlaySoundAtLocation(FVector Location, USoundCue* SoundToPlay)
{
	Server_PlaySoundAtLocation(Location,SoundToPlay );
}

void APrototype2Character::Ragdoll(bool _ragdoll)
{
	if (_ragdoll)
	{
		SetReplicateMovement(false);
		
		/* Disable all collision on capsule */
		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetActorEnableCollision(true);

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
		

		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp)
		{
			CharacterComp->StopMovementImmediately();
			CharacterComp->DisableMovement();
			CharacterComp->SetComponentTickEnabled(false);
		}
	}
	else
	{
		SetActorEnableCollision(false);
		GetMesh()->SetAllBodiesSimulatePhysics(false);
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->bBlendPhysics = false;
		SetReplicateMovement(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetRelativeTransform(MeshLocationWhenStunned);
		SetActorTransform(LocationWhenStunned);
		
		
		/* Disable all collision on capsule */
		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		




		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp)
		{
			CharacterComp->SetMovementMode(EMovementMode::MOVE_Walking);
			CharacterComp->SetComponentTickEnabled(true);
		}


	}
	
}

void APrototype2Character::Server_Ragdoll_Implementation(bool _ragdoll)
{
	if (_ragdoll)
	{
		LocationWhenStunned = GetActorTransform();
		MeshLocationWhenStunned = GetMesh()->GetRelativeTransform();
	}

	Multi_Ragdoll(_ragdoll);
}

void APrototype2Character::Multi_Ragdoll_Implementation(bool _ragdoll)
{
	Ragdoll(_ragdoll);
}

void APrototype2Character::PlayNetworkMontage(UAnimMontage* _montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_montage);
	Server_PlayNetworkMontage(_montage);
}

void APrototype2Character::Server_PlayNetworkMontage_Implementation(UAnimMontage* _montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_montage);
	Multi_PlayNetworkMontage(_montage);
}

void APrototype2Character::Multi_PlayNetworkMontage_Implementation(UAnimMontage* _montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_montage);
}

void APrototype2Character::Server_SetPlayerColour_Implementation()
{
	Multi_SetPlayerColour();
}

void APrototype2Character::Server_AddHUD_Implementation()
{
	Client_AddHUD();
}

void APrototype2Character::Multi_Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef)
    	{
    		UE_LOG(LogTemp, Warning, TEXT("Player HUD Created"));
    
    		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), PlayerID), PlayerHudPrefab);
    
    		if (PlayerHUDRef)
    			PlayerHUDRef->AddToViewport();
    	}
}

void APrototype2Character::Server_StartAttack_Implementation()
{
	if (AttackTimer < 0.0f)
	{
		if (HeldItem)
		{
			Server_DropItem();
		}
		
		bIsChargingAttack = true;

		if (Weapon)
		{
			Server_SocketItem(Weapon->Mesh, FName("WeaponHeldSocket"));
		}

		ChargeAttackAudioComponent->Play();
	}
}

void APrototype2Character::Multi_StartAttack_Implementation()
{
	
}

void APrototype2Character::Server_ReleaseAttack_Implementation()
{
	// Create a sphere collider, check if player hit, call player hit
	
	if (bIsChargingAttack && bCanAttack)
	{
		bCanAttack = false;
		
		// Cap attack charge
		if (AttackChargeAmount > MaxAttackCharge)
		{
			AttackChargeAmount = MaxAttackCharge;
		}
		
		int32 attackSphereRadius;
		if (Weapon)
		{
			// Create a larger sphere of effect
			attackSphereRadius = 75.0f + AttackChargeAmount * 30.0f;
		}
		else
		{
			// Create a smaller sphere of effect
			attackSphereRadius = 50.0f;
		}

		// If attack button is clicked without being held
		if (AttackChargeAmount < InstantAttackThreshold)
		{
			// Animation
			if(ExecuteAttackMontage_LongerWindUp)
			{
				PlayNetworkMontage(ExecuteAttackMontage_LongerWindUp);
			}
			// Delayed attack
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this, attackSphereRadius] { ExecuteAttack(attackSphereRadius); }), 0.5f, false);	
		}
		else
		{
			// Animation
			if(ExecuteAttackMontage)
			{
				PlayNetworkMontage(ExecuteAttackMontage);
			}
			
			ExecuteAttack(attackSphereRadius);
		}
	}

	// empty
	Multi_ReleaseAttack();
}

void APrototype2Character::Multi_ReleaseAttack_Implementation()
{
}

void APrototype2Character::Server_PlaySoundAtLocation_Implementation(FVector _location, USoundCue* _soundQueue)
{
	Multi_PlaySoundAtLocation(_location, _soundQueue);
}

void APrototype2Character::Multi_PlaySoundAtLocation_Implementation(FVector _location, USoundCue* _soundQueue)
{
	if (SoundAttenuationSettings)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _soundQueue, _location, 1, 1, 0, SoundAttenuationSettings);
	else
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _soundQueue, _location, 0.5f);
}

void APrototype2Character::Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player HUD Created"));
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(Cast<APrototype2PlayerController>(Controller), PlayerHudPrefab);

		PlayerHUDRef->AddToViewport();
	}
}

void APrototype2Character::Multi_SetPlayerColour_Implementation()
{
	if (auto* gamemode = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (auto* gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			for(auto player : gamestate->PlayerArray)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Colour Assigned Locally"));
				if (auto* character = Cast<APrototype2Character>(player->GetPlayerController()->GetCharacter()))
				{
					character->GetMesh()->SetMaterial(0, gamemode->PlayerMaterials[Cast<APrototype2PlayerState>(player)->Player_ID]);
				}
			}
		}
	}
}

void APrototype2Character::Server_TryInteract_Implementation()
{
	if((GetLocalRole() == IdealNetRole || GetLocalRole() == ROLE_Authority) && ClosestInteractableItem && !HeldItem)
	{
		// If player is holding nothing, and there is something to pickup in range
		if (PickupMontage &&
			ClosestInteractableItem->InterfaceType != EInterfaceType::SellBin &&
			ClosestInteractableItem->InterfaceType != EInterfaceType::GrowSpot)
		{
			// Animation
			PlayNetworkMontage(PickupMontage);
			
			// Call the InteractInterface interact function
			ClosestInteractableItem->Interact(this);

			// Put weapon on back
			Multi_SocketItem(Weapon->Mesh,  FName("WeaponHolsterSocket"));
		}
		else if (ClosestInteractableItem->InterfaceType == EInterfaceType::GrowSpot) // If the player is trying to pick up a plant from grow plot
		{
			// Call the InteractInterface interact function
			ClosestInteractableItem->Interact(this);

			if (HeldItem)
			{
				if (HeldItem->ItemComponent->PickupType == EPickup::Mandrake)
				{
					if (MandrakeScreamCue)
					{
						PlaySoundAtLocation(GetActorLocation(), MandrakeScreamCue);
					}
				}
			}
		}
	}
	else if (HeldItem) // If holding item
	{
		// If Sell Bin or Grow Patch close, interact
		if (ClosestInteractableItem &&
			(ClosestInteractableItem->InterfaceType == EInterfaceType::SellBin ||
			ClosestInteractableItem->InterfaceType == EInterfaceType::GrowSpot))
		{
			ClosestInteractableItem->Interact(this);

			if(ClosestInteractableItem->InterfaceType == EInterfaceType::SellBin && !HeldItem)
			{
				PlaySoundAtLocation(GetActorLocation(), SellCue);
			}
			if(ClosestInteractableItem->InterfaceType == EInterfaceType::GrowSpot && !HeldItem)
			{
				PlaySoundAtLocation(GetActorLocation(), PlantCue);
			}
		}
		else
		{
			Multi_DropItem();
		}
	}
}

void APrototype2Character::Server_DropItem_Implementation()
{
	Multi_DropItem();
}

void APrototype2Character::Multi_DropItem_Implementation()
{
	if(HeldItem)
	{
		// Drop into world
		if(HeldItem)
		{
			HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
			HeldItem->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
			// So that CheckForInteractables() can see it again
			HeldItem->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
		
		HeldItem = nullptr;

		// Set HUD image
		if (PlayerHUDRef)
			PlayerHUDRef->UpdatePickupUI(EPickup::None);

		PlaySoundAtLocation(GetActorLocation(), DropCue);
	}
}

void APrototype2Character::Server_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item)
{

	Multi_PickupItem(itemComponent, _item);
}

void APrototype2Character::Server_SocketItem_Implementation(UStaticMeshComponent* _object, FName _socket)
{

	Multi_SocketItem(_object, _socket);
}

void APrototype2Character::Multi_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item)
{
	if (itemComponent->Mesh)
	{
		itemComponent->Mesh->SetSimulatePhysics(false);
		itemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		// So that CheckForInteractables() cant see it while player is holding it
		itemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
	
	_item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeldItemSocket"));
	
	
	// Assign Players HeldItem
	HeldItem = _item;

	// Set HUD image
	if (PlayerHUDRef)
		PlayerHUDRef->UpdatePickupUI(HeldItem->ItemComponent->PickupType);

	PlaySoundAtLocation(GetActorLocation(), PickUpCue);
}

void APrototype2Character::Server_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& InMaterialsArray)
{
	Multi_ReceiveMaterialsArray(InMaterialsArray);
}

void APrototype2Character::Multi_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& _inMaterialsArray)
{
	USkeletalMeshComponent* meshComponent = GetMesh();
	if (meshComponent && _inMaterialsArray.Num() > 0)
	{
		for (int32 i = 0; i < _inMaterialsArray.Num(); i++)
		{
			meshComponent->SetMaterial(i, _inMaterialsArray[i]);
		}
	}
}

void APrototype2Character::Server_FireDizzySystem_Implementation()
{
	Multi_FireParticleSystem();
}

void APrototype2Character::Multi_FireParticleSystem_Implementation()
{
	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DizzySystem, InteractSystem->GetComponentLocation());
	NiagaraComponent->SetIsReplicated(true);
	// Set the NiagaraComponent to auto-destroy itself after it finishes playing
	NiagaraComponent->SetAutoDestroy(true);
	NiagaraComponent->Activate();
	NiagaraComponent->AttachToComponent(DizzyComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}
