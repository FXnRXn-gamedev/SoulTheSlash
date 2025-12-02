// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "SoulPlayerEnum.generated.h"



class ASlashEquippableItemMaster;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle				UMETA(DisplayName = "Idle"),
	Moving 				UMETA(DisplayName = "Moving"),
	Jumping 			UMETA(DisplayName = "Jumping"),
	Falling 			UMETA(DisplayName = "Falling"),
	Crouching 			UMETA(DisplayName = "Crouching"),
	Rolling 			UMETA(DisplayName = "Rolling"),
	Dodging 			UMETA(DisplayName = "Dodging"),
	Attacking 			UMETA(DisplayName = "Attacking"),
	Blocking 			UMETA(DisplayName = "Blocking"),
	Hit 				UMETA(DisplayName = "Hit"),
	Dead 				UMETA(DisplayName = "Dead")
};




#pragma region Equippable Enum

UENUM(BlueprintType)
enum class EWeaponEquipSource : uint8
{
	WES_None			UMETA(DisplayName = "WES_None"),
	WES_WorldPickup		UMETA(DisplayName = "Picked from World"),
	WES_Inventory		UMETA(DisplayName = "Eqipped from Inventory"),
	WES_Dropped			UMETA(DisplayName = "Dropeed to World")
};


UENUM(BlueprintType)
enum class EWeaponSwapBehavior : uint8
{
	WSB_DropToWorld     UMETA(DisplayName = "Drop Current to World"),
	WSB_AddToInventory  UMETA(DisplayName = "Add Current to Inventory"),
	WSB_Reject          UMETA(DisplayName = "Cannot Pick Up")
};

UENUM(BlueprintType)
enum class EWeaponTypeEnum : uint8
{
	Sword1				UMETA(DisplayName = "Sword1"),
	Sword2				UMETA(DisplayName = "Sword2"),
	Crossbow			UMETA(DisplayName = "Crossbow"),
	Bow					UMETA(DisplayName = "Bow"),
	Knife				UMETA(DisplayName = "Knife")
};

UENUM(BlueprintType)
enum class EWeaponStateTypeEnum : uint8
{
	Primary				UMETA(DisplayName = "Primary"),
	Secondary			UMETA(DisplayName = "Secondary"),
	Placeholder1		UMETA(DisplayName = "Placeholder1")
	
};

UENUM(BlueprintType)
enum class ESocketEnum : uint8
{
	WeaponBowBack_Socket		UMETA(DisplayName = "WeaponBowBack_Socket"),
	WeaponCrossbowBack_Socket	UMETA(DisplayName = "WeaponCrossbowBack_Socket"),
	WeaponSword_Socket			UMETA(DisplayName = "WeaponSword_Socket"),
	WeaponSword02_Socket 		UMETA(DisplayName = "WeaponSword02_Socket"),
	WeaponKnife_Socket			UMETA(DisplayName = "WeaponKnife_Socket")
};

UENUM(BlueprintType)
enum class EEquipHandEnum : uint8
{
	HoldItem_Sword_r			UMETA(DisplayName = "HoldItem_Sword_r"),
	HoldItem_Bow_l				UMETA(DisplayName = "HoldItem_Bow_l")
};

UENUM()
enum class EMontageAction
{
	MonatgeAction_None			UMETA(DisplayName = "Montage None"),
	MontageAction_Equip			UMETA(DisplayName = "Montage Equip"),
	MontageAction_Unequip		UMETA(DisplayName = "Montage Unequip")
};

USTRUCT(BlueprintType)
struct FMontageActionStruct : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EMontageAction Action;
	
	UPROPERTY(EditAnywhere)
	TMap<EWeaponTypeEnum, UAnimMontage*> Montages;
	
};

// // Accessing the montages:
// for (const FWeaponMontageMap& MontageMap : MontageRef)
// {
// 	for (const auto& Pair : MontageMap.Montages)
// 	{
// 		EWeaponTypeEnum WeaponType = Pair.Key;
// 		UAnimMontage* Montage = Pair.Value;
// 		// Use them...
// 	}
// }

USTRUCT(BlueprintType)
struct FEquippableStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponTypeEnum WeaponType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASlashEquippableItemMaster> ItemActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocketEnum AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipHandEnum EquipHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ActorRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Equipped;
};

USTRUCT(BlueprintType)
struct FWeaponInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EWeaponStateTypeEnum WeaponStateType;
	
	UPROPERTY(EditAnywhere)
	FEquippableStruct EquippableData;
	
	UPROPERTY(EditAnywhere)
	EWeaponEquipSource EquipSource;
	
	UPROPERTY(EditAnywhere)
	FGuid WeaponInstanceID;
	
	UPROPERTY(EditAnywhere)
	FTransform LastWorldTransform;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ASlashEquippableItemMaster> WeaponClass;
	
	FWeaponInstanceData() : 
	EquipSource(EWeaponEquipSource::WES_None), 
	WeaponInstanceID(FGuid::NewGuid()), 
	LastWorldTransform(FTransform::Identity), 
	WeaponClass(nullptr){}
	
	
};

#pragma endregion 