

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "HarvestHavocGameUserSettings.generated.h"

UCLASS()
class PROTOTYPE2_API UHarvestHavocGameUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UHarvestHavocGameUserSettings* GetHarvestHavocGameUserSettings();
	
	virtual void SaveSettings() override;
	virtual void LoadSettings(bool bForceReload = false) override;

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(int32 _Volume);
	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(int32 _Volume);
	UFUNCTION(BlueprintCallable)
	void SetAmbienceVolume(int32 _Volume);
	UFUNCTION(BlueprintCallable)
	void SetSFXVolume(int32 _Volume);
	UFUNCTION(BlueprintCallable)
	void SetPlayerStencil(bool _bEnable);

	UFUNCTION(BlueprintPure)
	int32 GetMasterVolume() const { return MasterVolume; }
	UFUNCTION(BlueprintPure)
	int32 GetMusicVolume() const { return MusicVolume; }
	UFUNCTION(BlueprintPure)
	int32 GetAmbienceVolume() const { return AmbienceVolume; }
	UFUNCTION(BlueprintPure)
	int32 GetSFXVolume() const { return SFXVolume; }
	UFUNCTION(BlueprintPure)
	bool GetPlayerStencil() const { return bPlayerStencil; }

protected:
	/* Custom Audio Settings */
	UPROPERTY(config, EditAnywhere)
	int32 MasterVolume;
	
	UPROPERTY(config, EditAnywhere)
	int32 MusicVolume;

	UPROPERTY(config, EditAnywhere)
	int32 AmbienceVolume;

	UPROPERTY(config, EditAnywhere)
	int32 SFXVolume;

	/* Custom Game Settings */
	UPROPERTY(config, EditAnywhere)
	bool bPlayerStencil;
};
