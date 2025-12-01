// FXnRXn copyright notice


#include "Component/PlayerCombatComponent.h"

#include "AnimInstance/SlashCharacteAnimInstanceBase.h"
#include "Components/SphereComponent.h"
#include "Item/SlashEquippableItemMaster.h"


#pragma region Constructor

UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	OwnerCharacter = nullptr;
	PlayerAnimInst = nullptr;
}

#pragma endregion





// Called when the game starts
void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (OwnerCharacter == nullptr)
	{
		OwnerCharacter = Cast<ASlashPlayerCharacter>(GetOwner());
	}
	if (PlayerAnimInst == nullptr && OwnerCharacter)
	{
		PlayerAnimInst = Cast<USlashCharacteAnimInstanceBase>(OwnerCharacter->GetMesh()->GetAnimInstance());
	}
}

#pragma region Equip/Unequip

void UPlayerCombatComponent::SetWeapon(ASlashEquippableItemMaster* NewWeapon)
{
	if (!NewWeapon) return;
	//MainWeaponItem = NewWeapon;

	
	EWeaponTypeEnum NewWeaponType = NewWeapon->Weapon;
	// Check if we already have a weapon of this type
	FWeaponEquippableStruct* ExistingWeaponStruct = WeaponEquippableSetup.Find(NewWeaponType);
	
	// If we have an existing weapon of the same type, drop it
	// if (ExistingWeaponStruct && ExistingWeaponStruct->ActorRef && ExistingWeaponStruct->AttachSocket == NewWeapon->)
	// {
	// 	ASlashEquippableItemMaster* OldWeapon = Cast<ASlashEquippableItemMaster>(ExistingWeaponStruct->ActorRef);
	// 	if (OldWeapon)
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("DROP WEAPON"));
	// 		//DropWeapon(OldWeapon);
	// 	}
	// }
	
	// Set the new weapon
	MainWeaponItem = NewWeapon;
	
	// // Update the weapon setup with the new weapon
	// if (ExistingWeaponStruct)
	// {
	// 	ExistingWeaponStruct->ActorRef = NewWeapon;
	// }
	// else
	// {
	// 	// If this weapon type doesn't exist in the setup, add it
	// 	FWeaponEquippableStruct NewWeaponStruct;
	// 	NewWeaponStruct.WeaponType = NewWeaponType;
	// 	NewWeaponStruct.ItemActor = NewWeapon->GetClass();
	// 	NewWeaponStruct.ActorRef = NewWeapon;
	// 	NewWeaponStruct.Equipped = false; // Start as unequipped
	// 	// Set socket and hand based on weapon type
	// 	switch (NewWeaponType)
	// 	{
	// 	case EWeaponTypeEnum::Sword1:
	// 	case EWeaponTypeEnum::Sword2:
	// 		NewWeaponStruct.AttachSocket = ESocketEnum::WeaponSword_Socket;
	// 		NewWeaponStruct.EquipHand = EEquipHandEnum::HoldItem_Sword_r;
	// 		break;
	// 	case EWeaponTypeEnum::Bow:
	// 		NewWeaponStruct.AttachSocket = ESocketEnum::WeaponBowBack_Socket;
	// 		NewWeaponStruct.EquipHand = EEquipHandEnum::HoldItem_Bow_l;
	// 		break;
	// 	case EWeaponTypeEnum::Crossbow:
	// 		NewWeaponStruct.AttachSocket = ESocketEnum::WeaponCrossbowBack_Socket;
	// 		NewWeaponStruct.EquipHand = EEquipHandEnum::HoldItem_Sword_r;
	// 		break;
	// 	case EWeaponTypeEnum::Knife:
	// 		NewWeaponStruct.AttachSocket = ESocketEnum::WeaponKnife_Socket;
	// 		NewWeaponStruct.EquipHand = EEquipHandEnum::HoldItem_Sword_r;
	// 		break;
	// 	}
 //        
	// 	WeaponEquippableSetup.Add(NewWeaponType, NewWeaponStruct);
	// }
}

void UPlayerCombatComponent::DropWeapon(ASlashEquippableItemMaster* WeaponToDrop)
{
	if (!WeaponToDrop || !OwnerCharacter)
	{
		return;
	}
	// Detach from character
	WeaponToDrop->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Enable physics and collision
	WeaponToDrop->MainMesh->SetSimulatePhysics(true);
	WeaponToDrop->MainMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	WeaponToDrop->InnerSphereColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponToDrop->OuterSphereColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Set weapon state to Dropped
	WeaponToDrop->SetWeaponState(EWeaponState::Dropped);

	// Enable pickup
	WeaponToDrop->bCanBePickedUp = true;
	WeaponToDrop->bIsEquipped = false;

	// Position the weapon in front of the character
	FVector DropLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 100.0f;
	WeaponToDrop->SetActorLocation(DropLocation);

	// Clear reference if it's the main weapon
	EWeaponTypeEnum DroppedWeaponType = WeaponToDrop->Weapon;
	FWeaponEquippableStruct* FoundWeaponItemStruct = WeaponEquippableSetup.Find(DroppedWeaponType);
	if (FoundWeaponItemStruct && FoundWeaponItemStruct->ActorRef == WeaponToDrop)
	{
		FoundWeaponItemStruct->ActorRef = nullptr;
		FoundWeaponItemStruct->Equipped = false;
	}	

	// If this was the main weapon, clear it
	if (MainWeaponItem == WeaponToDrop)
	{
		MainWeaponItem = nullptr;
	}
}




bool UPlayerCombatComponent::HasWeaponOfType(EWeaponTypeEnum WeaponType) const
{
	const FWeaponEquippableStruct* WeaponStruct = WeaponEquippableSetup.Find(WeaponType);
	return WeaponStruct && WeaponStruct->ActorRef && IsValid(WeaponStruct->ActorRef);
}

void UPlayerCombatComponent::AttachEquippableWeapon(EWeaponTypeEnum WeaponType)
{
	FWeaponEquippableStruct* FoundWeaponItem = WeaponEquippableSetup.Find(WeaponType);

	if (!FoundWeaponItem || !GetWorld() || !OwnerCharacter)
	{
		return; // Item not found or invalid
	}
	// Check if actor is already spawned
	if (FoundWeaponItem->ActorRef && IsValid(FoundWeaponItem->ActorRef))
	{
		UE_LOG(LogTemp, Log, TEXT("AttachEquippableWeapon: Item already spawned for ItemType: %d"), (int32)WeaponType);

		// Disable physics for the new weapon when attaching
		ASlashEquippableItemMaster* WeaponActor = Cast<ASlashEquippableItemMaster>(FoundWeaponItem->ActorRef);
		if (WeaponActor)
		{
			WeaponActor->MainMesh->SetSimulatePhysics(false);
			WeaponActor->SetWeaponState(EWeaponState::Holstered);
		}
        
		// Just attach if not equipped
		if (!FoundWeaponItem->Equipped)
		{
			AttachItemSocket(*FoundWeaponItem, FoundWeaponItem->ActorRef);
		}
		return;
	}

	// Only spawn if we have a valid ItemActor class and no existing actor
	if (!FoundWeaponItem->ItemActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachEquippableWeapon: No ItemActor class specified for ItemType: %d"), (int32)WeaponType);
		return;
	}


	// Spawn New Item
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = OwnerCharacter;

	ASlashEquippableItemMaster* SpawnedItem = GetWorld()->SpawnActor<ASlashEquippableItemMaster>(
		FoundWeaponItem->ItemActor, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedItem)
	{
		FoundWeaponItem->ActorRef = SpawnedItem;  // Store the actual actor instance, not the class
		SpawnedItem->bIsEquipped = true;

		// Disable physics for the spawned weapon
		SpawnedItem->TogglePhysics(false);
		SpawnedItem->SetWeaponState(EWeaponState::Holstered);
        
		AttachItemSocket(*FoundWeaponItem, SpawnedItem);
		// Attach Item if needed
		// if (!FoundWeaponItem->Equipped)
		// {
		// 	SpawnedItem->bIsEquipped = true;
		// 	AttachItemSocket(*FoundWeaponItem, SpawnedItem);
		// }

	}
	else
	{
		// If spawning failed, keep the original entry
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn equippable item for key: %d"), (int32)WeaponType);
	}
}

void UPlayerCombatComponent::AttachItemSocket(FWeaponEquippableStruct EquippableStruct, AActor* Actor)
{

	if (!Actor || !OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachItemSocket: Invalid Actor or OwnerCharacter"));
		return;
	}
	// This switch block is the C++ equivalent of the "Switch on Socket_Enum" node
	FName SocketToAttachTo;

	// Set up the attachment rules, matching the pins on the Blueprint node
	const FAttachmentTransformRules AttachmentRules
	(
		EAttachmentRule::SnapToTarget, // Location Rule
		EAttachmentRule::SnapToTarget, // Rotation Rule
		EAttachmentRule::KeepRelative, // Scale Rule
		true                           // bWeldSimulatedBodies
	);

	

	switch (EquippableStruct.WeaponType)
	{
	case EWeaponTypeEnum::Sword1:
		SocketToAttachTo = FName("WeaponSword_Socket");
		break;
	case EWeaponTypeEnum::Sword2:
		SocketToAttachTo = FName("WeaponSword02_Socket");
		break;
	case EWeaponTypeEnum::Crossbow:
		SocketToAttachTo = FName("WeaponCrossbowBack_Socket");
		break;
	case EWeaponTypeEnum::Bow:
		SocketToAttachTo = FName("WeaponBowBack_Socket");
		break;
	case EWeaponTypeEnum::Knife:
		SocketToAttachTo = FName("WeaponKnife_Socket");
		break;
	}

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachItemSocket: OwnerCharacter has no mesh"));
		return;
	}
	// ✅ Attach the weapon actor to the character’s skeletal mesh
	Actor->AttachToComponent(Mesh, AttachmentRules, SocketToAttachTo);
}

void UPlayerCombatComponent::EquipWeapon(EWeaponTypeEnum ItemType)
{
	FWeaponEquippableStruct* FoundWeaponItem = WeaponEquippableSetup.Find(ItemType);
	FName HandToAttachTo;
	// Set up the attachment rules, matching the pins on the Blueprint node
	const FAttachmentTransformRules AttachmentRules
	(
		EAttachmentRule::SnapToTarget, // Location Rule
		EAttachmentRule::SnapToTarget, // Rotation Rule
		EAttachmentRule::KeepRelative, // Scale Rule
		true                           // bWeldSimulatedBodies
	);

	if (FoundWeaponItem)
	{
		switch (ItemType)
		{
		case EWeaponTypeEnum::Sword1:
			HandToAttachTo = FName("HoldItem_Sword_r");
			break;
		case EWeaponTypeEnum::Sword2:
			HandToAttachTo = FName("HoldItem_Sword_r");
			break;
		case EWeaponTypeEnum::Crossbow:
			HandToAttachTo = FName("HoldItem_Sword_r");
			break;
		case EWeaponTypeEnum::Bow:
			HandToAttachTo = FName("HoldItem_Bow_l");
			break;
		case EWeaponTypeEnum::Knife:
			HandToAttachTo = FName("HoldItem_Sword_r");
			break;
		}

		USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
		if (!Mesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("AttachItemSocket: OwnerCharacter has no mesh"));
			return;
		}

		FoundWeaponItem->ActorRef->AttachToComponent(Mesh, AttachmentRules, HandToAttachTo);
		SetEquipStatus(ItemType, true);
		
		ASlashEquippableItemMaster* WeaponItem = Cast<ASlashEquippableItemMaster>(FoundWeaponItem->ActorRef);
		WeaponItem->SetWeaponState(EWeaponState::Equipped);
	}
}

void UPlayerCombatComponent::UnequipWeapon(EWeaponTypeEnum ItemType)
{
	FWeaponEquippableStruct* FoundWeaponItem = WeaponEquippableSetup.Find(ItemType);
	if (FoundWeaponItem)
	{
		AttachItemSocket(*FoundWeaponItem, FoundWeaponItem->ActorRef);
		SetEquipStatus(ItemType, false);
	}
}

void UPlayerCombatComponent::SetEquipStatus(EWeaponTypeEnum ItemType, bool bEquipped)
{
	
}

#pragma endregion


#pragma region Equip/Unequip Animation

UAnimMontage* UPlayerCombatComponent::GetEquipMontage(FWeaponEquippableStruct EquippableStruct)
{

	switch (EquippableStruct.AttachSocket)
	{
	case ESocketEnum::WeaponBowBack_Socket:
		return EquipBowAnimMontage;
	case ESocketEnum::WeaponSword02_Socket:
		return EquipBowAnimMontage;
	case ESocketEnum::WeaponCrossbowBack_Socket:
		return EquipCrossbowAnimMontage;
	case ESocketEnum::WeaponSword_Socket:
		return EquipSwordAnimMontage;
	case ESocketEnum::WeaponKnife_Socket:
		return EquipKnifeAnimMontage;
	default:
		return nullptr;
	}
}

UAnimMontage* UPlayerCombatComponent::GetUnequipMontage(FWeaponEquippableStruct EquippableStruct)
{

	switch (EquippableStruct.AttachSocket)
	{
	case ESocketEnum::WeaponBowBack_Socket:
		return UnequipBowAnimMontage;
	case ESocketEnum::WeaponSword02_Socket:
		return UnequipBowAnimMontage;
	case ESocketEnum::WeaponCrossbowBack_Socket:
		return UnequipCrossbowAnimMontage;
	case ESocketEnum::WeaponSword_Socket:
		return UnequipSwordAnimMontage;
	case ESocketEnum::WeaponKnife_Socket:
		return UnequipKnifeAnimMontage;
	default:
		return nullptr;
	}
}

#pragma endregion 