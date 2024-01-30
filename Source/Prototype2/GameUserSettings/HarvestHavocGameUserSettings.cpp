


#include "HarvestHavocGameUserSettings.h"


//UHarvestHavocGameUserSettings::UHarvestHavocGameUserSettings()
//{
//}

UHarvestHavocGameUserSettings* UHarvestHavocGameUserSettings::GetHarvestHavocGameUserSettings()
{
	return Cast<UHarvestHavocGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UHarvestHavocGameUserSettings::SaveSettings()
{
	Super::SaveSettings();

	// Save new variables
	GConfig->SetInt(TEXT("Audio"), TEXT("MasterVolume"), MasterVolume, GGameUserSettingsIni);
	GConfig->SetInt(TEXT("Audio"), TEXT("MusicVolume"), MusicVolume, GGameUserSettingsIni);
	GConfig->SetInt(TEXT("Audio"), TEXT("AmbienceVolume"), AmbienceVolume, GGameUserSettingsIni);
	GConfig->SetInt(TEXT("Audio"), TEXT("SFXVolume"), SFXVolume, GGameUserSettingsIni);
	GConfig->SetBool(TEXT("GameSettings"), TEXT("bPlayerStencil"), bPlayerStencil, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UHarvestHavocGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);

	// Load your new variables
	MasterVolume = GConfig->GetInt(TEXT("Audio"), TEXT("MasterVolume"), MasterVolume, GGameUserSettingsIni);
	MusicVolume = GConfig->GetInt(TEXT("Audio"), TEXT("MusicVolume"), MusicVolume, GGameUserSettingsIni);
	AmbienceVolume = GConfig->GetInt(TEXT("Audio"), TEXT("AmbienceVolume"), AmbienceVolume, GGameUserSettingsIni);
	SFXVolume = GConfig->GetInt(TEXT("Audio"), TEXT("SFXVolume"), SFXVolume, GGameUserSettingsIni);
	bPlayerStencil = GConfig->GetBool(TEXT("GameSettings"), TEXT("bPlayerStencil"), bPlayerStencil, GGameUserSettingsIni);

	UE_LOG(LogTemp, Warning, TEXT("Loaded game user settings"));
}

void UHarvestHavocGameUserSettings::SetMasterVolume(int32 _Volume)
{
	MasterVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetMusicVolume(int32 _Volume)
{
	MusicVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetAmbienceVolume(int32 _Volume)
{
	AmbienceVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetSFXVolume(int32 _Volume)
{
	SFXVolume = _Volume;
}

void UHarvestHavocGameUserSettings::SetPlayerStencil(bool _bEnable)
{
	bPlayerStencil = _bEnable;
}
