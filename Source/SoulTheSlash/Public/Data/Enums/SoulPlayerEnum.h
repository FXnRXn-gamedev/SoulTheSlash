// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "SoulPlayerEnum.generated.h"

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

#pragma endregion 