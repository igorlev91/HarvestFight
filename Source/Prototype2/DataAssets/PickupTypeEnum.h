﻿#pragma once

/* Enum needed in PlantData and WeaponData */
UENUM()
enum class EPickupDataType : uint8
{
	SeedData,
	WeaponData,
	PlantData,
	BeehiveData,
	FlowerData,
	FertiliserData
};

/* Enum for choosing the correct attack animations */
UENUM(BlueprintType)
enum class EWeaponAnimation : uint8
{
	Punching,
	Leek,
	Aspearagus,
	HoneyStick,
};

/* Enum for passing debuffs into the characters debuff component */
UENUM()
enum class EDebuff : uint8
{
	None = 0,
	Stun,
	Daze, // Slow + "dizzy" movement
	Slow,
	Punch
};
class PROTOTYPE2_API PickupTypeEnum {};
