

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "HarvestHavocGameUserSettings.generated.h"

UCLASS()
class PROTOTYPE2_API UHarvestHavocGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	//UHarvestHavocGameUserSettings();
	
	UFUNCTION(BlueprintCallable)
	static UHarvestHavocGameUserSettings* GetHarvestHavocGameUserSettings();
	
	virtual void SaveSettings() override;
	virtual void LoadSettings(bool bForceReload = false) override;

	void SetMasterVolume(int32 _Volume);
	void SetMusicVolume(int32 _Volume);
	void SetAmbienceVolume(int32 _Volume);
	void SetSFXVolume(int32 _Volume);
	void SetPlayerStencil(bool _bEnable);

	int32 GetMasterVolume() { return MasterVolume; }
	int32 GetMusicVolume() { return MusicVolume; }
	int32 GetAmbienceVolume() { return AmbienceVolume; }
	int32 GetSFXVolume() { return SFXVolume; }
	bool GetPlayerStencil() { return bPlayerStencil; }

//protected:
	/* Custom Audio Settings */
	UPROPERTY(EditAnywhere)
	int32 MasterVolume{0};
	
	UPROPERTY(EditAnywhere)
	int32 MusicVolume{0};

	UPROPERTY(EditAnywhere)
	int32 AmbienceVolume{0};

	UPROPERTY(EditAnywhere)
	int32 SFXVolume{0};

	/* Custom Game Settings */
	UPROPERTY(EditAnywhere)
	bool bPlayerStencil{false};
};
