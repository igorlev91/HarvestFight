﻿#pragma once

/* Enum needed in PlantData and WeaponData */
UENUM()
enum class EPickupDataType : uint8
{
	WeaponData,
	PlantData,
	BeehiveData,
	FlowerData,
};

class PROTOTYPE2_API PickupTypeEnum {};
