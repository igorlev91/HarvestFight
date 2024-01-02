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
#include "InteractInterface.h"
#include "PickUpItem.h"
#include "Prototype2PlayerController.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "DynamicMesh/ColliderMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Widget_InteractionPanel.h"
#include "Widgets/Widget_IngameMenu.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Widget_PlayerHUD.h"



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
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
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

}

void APrototype2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2Character, Weapon);
	DOREPLIFETIME(APrototype2Character, HeldItem);
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
	
	// Setup Weapon
	if (WeaponPrefab)
	{
		Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponPrefab);
	
		Weapon->ItemComponent->Mesh->SetSimulatePhysics(false);
		Weapon->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Weapon->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
		// Attach to socket
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponHolsterSocket"));
	}
	
	Server_AddHUD();
}

void APrototype2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Stun
	if (bIsStunned)
	{
		bIsChargingAttack = false;

		// Set animation to stun
		
		StunTimer -= DeltaSeconds;
		if (StunTimer < 0.0f)
		{
			bIsStunned = false;

			// Enable input
		}
	}
	
	// Attack
	if (bIsChargingAttack)
	{
		AttackChargeAmount += DeltaSeconds;
	}

	// Check if anything is around to be interacted with
	CheckForInteractables();
}

void APrototype2Character::ChargeAttack()
{
	if (HeldItem)
	{
		Server_DropItem();
	}
	
	bIsChargingAttack = true;

	if (Weapon)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponHeldSocket"));
	}
	
	// Animation
	if(ChargeAttackMontage)
	{
		PlayNetworkMontage(ChargeAttackMontage);
	}
}

void APrototype2Character::ReleaseAttack()
{
	// Cap attack charge
	if (AttackChargeAmount > MaxAttackCharge)
	{
		AttackChargeAmount = MaxAttackCharge;
	}

	// Create a sphere collider, check if player hit, call player hit
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

	ExecuteAttack(attackSphereRadius);
	
	// Reset Attack variables
	bIsChargingAttack = false;
	AttackChargeAmount = 0.0f;
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
	DrawDebugSphere(GetWorld(), inFrontOfPlayer, colSphere.GetSphereRadius(), 50, FColor::Purple, false, 2.0f);
	
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

	// Animation
	if(ExecuteAttackMontage)
	{
		PlayNetworkMontage(ExecuteAttackMontage);
	}
}

void APrototype2Character::Interact()
{
	if (!bIsChargingAttack)
	{
		Server_TryInteract();
	}
	
	// Debug draw collision sphere
	//FCollisionShape colSphere = FCollisionShape::MakeSphere(InteractRadius);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), colSphere.GetSphereRadius(), 50, FColor::Purple, false, 3.0f);
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
	bool isHit = GetWorld()->SweepMultiByChannel(outHits, sweepStart, sweepEnd, FQuat::Identity, ECC_Visibility, colSphere);

	if (isHit)
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
	StunTimer = AttackCharge;
}

void APrototype2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		////Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

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

void APrototype2Character::Server_AddHUD_Implementation()
{
	Client_AddHUD();
}

void APrototype2Character::Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab)
	{
		if (Controller)
		{
			if (auto* playerController = Cast<APrototype2PlayerController>(Controller))
			{
				PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(playerController, PlayerHudPrefab);

				if (PlayerHUDRef)
					PlayerHUDRef->AddToViewport();
			}
		}
	}
}

void APrototype2Character::Server_TryInteract_Implementation()
{
	// If player is holding nothing, and there is something to pickup in range
	if(ClosestInteractableItem && !HeldItem)
	{
		if (PickupMontage &&
			ClosestInteractableItem->InterfaceType != EInterfaceType::SellBin &&
			ClosestInteractableItem->InterfaceType != EInterfaceType::GrowSpot)
		{
			// Animation
			PlayNetworkMontage(PickupMontage);
			
			// Call the InteractInterface interact function
			ClosestInteractableItem->Interact(this);

			// Put weapon on back
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponHolsterSocket"));
		}
		else if (ClosestInteractableItem->InterfaceType == EInterfaceType::GrowSpot)
		{
			// Call the InteractInterface interact function
			ClosestInteractableItem->Interact(this);
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
		}
		else
		{
			// If item wasn't sold, drop it
			Server_DropItem();
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
		HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
		HeldItem->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		// So that CheckForInteractables() can see it again
		HeldItem->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		
		HeldItem = nullptr;
	}
}

void APrototype2Character::Server_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item)
{
	Multi_PickupItem(itemComponent, _item);
}

void APrototype2Character::Multi_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item)
{
	itemComponent->Mesh->SetSimulatePhysics(false);
	itemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// So that CheckForInteractables() cant see it while player is holding it
	itemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	// Attach to socket
	_item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeldItemSocket"));
	
	// Assign Players HeldItem
	HeldItem = _item;	
}
