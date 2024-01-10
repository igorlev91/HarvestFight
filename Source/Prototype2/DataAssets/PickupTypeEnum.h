#pragma once

/* Enum needed in PlantData and WeaponData */
UENUM()
enum class EPickupDataType : uint8
{
	WeaponData,
	PlantData,
	BeehiveData,
	FlowerData,
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
class PROTOTYPE2_API PickupTypeEnum {};
