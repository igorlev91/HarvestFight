

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "HarvestHavocGameUserSettings.generated.h"

UENUM(BlueprintType)
enum class EIndicatorUISetting : uint8
{
	ON,
	ONSIDESONLY,
	OFF
};

UCLASS()
class PROTOTYPE2_API UHarvestHavocGameUserSettings : public UGameUserSettings
{
	friend class APrototype2Character;
	friend class UWidget_GameOptions;
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintPure)
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
	UFUNCTION(BlueprintCallable)
	void SetUIIndicators(EIndicatorUISetting _Setting);
	UFUNCTION(BlueprintCallable)
	void SetUIIndicatorSizeLarge(bool _bLargeSize);
	UFUNCTION(BlueprintCallable)
	void SetQuickTipLobbyTutorials(int32 _QuickTipLobbyTutorials);
	UFUNCTION(BlueprintCallable)
	void SetQuickTipClassicTutorials(int32 _QuickTipClassicTutorials);
	UFUNCTION(BlueprintCallable)
	void SetQuickTipBrawlTutorials(int32 _QuickTipBrawlTutorials);
	UFUNCTION(BlueprintCallable)
	void SetQuickTipBlitzTutorials(int32 _QuickTipBlitzTutorials);

	UFUNCTION(BlueprintCallable)
	int32 GetMasterVolume() const { return MasterVolume; }
	UFUNCTION(BlueprintCallable)
	int32 GetMusicVolume() const { return MusicVolume; }
	UFUNCTION(BlueprintCallable)
	int32 GetAmbienceVolume() const { return AmbienceVolume; }
	UFUNCTION(BlueprintCallable)
	int32 GetSFXVolume() const { return SFXVolume; }
	UFUNCTION(BlueprintCallable)
	bool GetPlayerStencil() const { return bPlayerStencil; }
	UFUNCTION(BlueprintCallable)
	EIndicatorUISetting GetUIIndicators() const { return (EIndicatorUISetting)UIIndicators; }
	UFUNCTION(BlueprintCallable)
	bool GetUIIndicatorSizeLarge() const { return UIIndicatorSizeLarge; }
	UFUNCTION(BlueprintCallable)
	bool GetPlayerNames() const { return PlayerNames; }
	UFUNCTION(BlueprintCallable)
	bool GetPlantValueFloatingUI() const { return PlantValueFloatingUI; }
	UFUNCTION(BlueprintCallable)
	bool GetSprintBar() const { return SprintBar; }
	UFUNCTION(BlueprintCallable)
	bool GetQuickTipTutorials() const { return QuickTipTutorials; }
	UFUNCTION(BlueprintCallable)
	int32 GetQuickTipLobbyTutorials() const { return QuickTipLobbyTutorials; }
	UFUNCTION(BlueprintCallable)
	int32 GetQuickTipClassicTutorials() const { return QuickTipClassicTutorials; }
	UFUNCTION(BlueprintCallable)
	int32 GetQuickTipBrawlTutorials() const { return QuickTipBrawlTutorials; }
	UFUNCTION(BlueprintCallable)
	int32 GetQuickTipBlitzTutorials() const { return QuickTipBlitzTutorials; }
	UFUNCTION(BlueprintCallable)
	int32 GetMouseSensitivityScale() const { return MouseSensitivityScale; }
	UFUNCTION(BlueprintCallable)
	bool GetCustomMouseCursor() const { return CustomMouseCursor; }
	UFUNCTION(BlueprintCallable)
	int32 GetControllerMenuSensitivityScale() const { return ControllerMenuSensitivityScale; }

protected:
	/* Custom Graphics Settings */
	UPROPERTY(config, EditAnywhere)
	int32 FieldOfView;
	
	UPROPERTY(config, EditAnywhere)
	int32 MasterGraphics;
	
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

	UPROPERTY(config, EditAnywhere)
	bool bEnemyAlwaysRed;

	/* UI Related */
	UPROPERTY(config, EditAnywhere)
	int32 UIIndicators;

	UPROPERTY(config, EditAnywhere)
	bool UIIndicatorSizeLarge;

	UPROPERTY(config, EditAnywhere)
	bool PlayerNames;

	UPROPERTY(config, EditAnywhere)
	bool PlantValueFloatingUI;

	UPROPERTY(config, EditAnywhere)
	bool SprintBar;

	/* Tutorials */
	UPROPERTY(config, EditAnywhere)
	bool QuickTipTutorials;

	UPROPERTY(config, EditAnywhere)
	int32 QuickTipLobbyTutorials;

	UPROPERTY(config, EditAnywhere)
	int32 QuickTipClassicTutorials;

	UPROPERTY(config, EditAnywhere)
	int32 QuickTipBrawlTutorials;

	UPROPERTY(config, EditAnywhere)
	int32 QuickTipBlitzTutorials;

	/* Mouse sensitivity */
	UPROPERTY(config, EditAnywhere)
	int32 MouseSensitivityScale;

	/* Mouse cursor */
	UPROPERTY(config, EditAnywhere)
	bool CustomMouseCursor;

	/* Controller menu sensitivity */
	UPROPERTY(config, EditAnywhere)
	int32 ControllerMenuSensitivityScale;
};
