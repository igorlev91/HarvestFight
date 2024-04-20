


#include "Widget_HostOptions.h"

/*#include "SocketSubsystem.h"*/
#include "Widget_OptionSelector.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

/*#include "JsonObjectConverter.h"*/

void UWidget_HostOptions::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	GameSpeed = MEDIUM_GAME;
	DefaultGameSpeed = EGameSpeed::MEDIUM_GAME;
	TempGameSpeed = MEDIUM_GAME;

	/* Extra Settings Setup */
	
	/* Set game speed control */
	GameLength_Control->OptionText->SetText(FText::FromString("Game Speed"));
	UpdateGameSpeedText();

	/* Set stealing control */
	Stealing_Control->OptionText->SetText(FText::FromString("Stealing (from plots)"));
	UpdateStealingText();

	/* Set fertiliser control */
	Fertiliser_Control->OptionText->SetText(FText::FromString("Fertiliser Spawn (if available)"));
	UpdateFertiliserText();

	/* Set cement control */
	Cement_Control->OptionText->SetText(FText::FromString("Cement Spawn (if available)"));
	UpdateCementText();
}

void UWidget_HostOptions::NativeOnInitialized()
{
	Super::NativeOnInitialized();
//	GetPublicIPAddress();

}

void UWidget_HostOptions::NativeConstruct()
{
	Super::NativeConstruct();
	StartButton->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnHostPressed);

	/* Extra Settings Setup */
	
	/* Game speed control buttons */
	if (GameLength_Control)
	{
		GameLength_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnGameSpeedControlLeftButtonPressed);
		GameLength_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnGameSpeedControlRightButtonPressed);
	}

	/* Stealing control buttons */
	if (Stealing_Control)
	{
		Stealing_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnStealingControlButtonPressed);
		Stealing_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnStealingControlButtonPressed);
	}

	/* Fertiliser control buttons */
	if (Fertiliser_Control)
	{
		Fertiliser_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnFertiliserControlButtonPressed);
		Fertiliser_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnFertiliserControlButtonPressed);
	}
	
	/* Cement control buttons */
	if (Cement_Control)
	{
		Cement_Control->ButtonLeft->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnCementControlButtonPressed);
		Cement_Control->ButtonRight->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnCementControlButtonPressed);
	}
}

void UWidget_HostOptions::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	HoverButton(bIsButtonHovered, ButtonToPulse, PulseTime, InDeltaTime, FVector2D(1.0f, 1.0f), ButtonScale);
}

void UWidget_HostOptions::OnHostPressed()
{
	auto GameInstance = GetGameInstance<UPrototypeGameInstance>();
	if (!GameInstance)
		return;

	GameInstance->bTeams = IsTeamsButton->IsChecked();
}

void UWidget_HostOptions::ResetDefaults()
{
	TempGameSpeed = DefaultGameSpeed;
	UpdateGameSpeedText();
	SetGameSpeedControl();
	TempStealingSetting = DefaultStealingSetting;
	UpdateStealingText();
	SetStealingControl();
	TempFertiliserSetting = DefaultFertiliserSetting;
	UpdateFertiliserText();
	SetFertiliserControl();
	TempCementSetting = DefaultCementSetting;
	UpdateCementText();
	SetCementControl();
}

void UWidget_HostOptions::ResetSetting()
{
	TempGameSpeed = GameSpeed;
	UpdateGameSpeedText();
	SetGameSpeedControl();
	TempStealingSetting = StealingSetting;
	UpdateStealingText();
	SetStealingControl();
	TempFertiliserSetting = FertiliserSetting;
	UpdateFertiliserText();
	SetFertiliserControl();
	TempCementSetting = CementSetting;
	UpdateCementText();
	SetCementControl();
}

void UWidget_HostOptions::ConfirmSetting()
{
	SetStealingControl();
	SetFertiliserControl();
	SetCementControl();
}

void UWidget_HostOptions::OnGameSpeedControlLeftButtonPressed()
{
	switch (TempGameSpeed)
	{
	case EGameSpeed::SHORT_GAME:
		{
			TempGameSpeed = EGameSpeed::LONG_GAME;
			break;
		}
	case EGameSpeed::MEDIUM_GAME:
		{
			TempGameSpeed = EGameSpeed::SHORT_GAME;
			break;
		}
	case EGameSpeed::LONG_GAME:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
		default:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
	}

	UpdateGameSpeedText();
}

void UWidget_HostOptions::OnGameSpeedControlRightButtonPressed()
{
	switch (TempGameSpeed)
	{
	case EGameSpeed::SHORT_GAME:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
	case EGameSpeed::MEDIUM_GAME:
		{
			TempGameSpeed = EGameSpeed::LONG_GAME;
			break;
		}
	case EGameSpeed::LONG_GAME:
		{
			TempGameSpeed = EGameSpeed::SHORT_GAME;
			break;
		}
	default:
		{
			TempGameSpeed = EGameSpeed::MEDIUM_GAME;
			break;
		}
	}
	UpdateGameSpeedText();
}

void UWidget_HostOptions::UpdateGameSpeedText()
{
	switch (TempGameSpeed)
	{
	case EGameSpeed::SHORT_GAME:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Short"));
			break;
		}
	case EGameSpeed::MEDIUM_GAME:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	case EGameSpeed::LONG_GAME:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Long"));
			break;
		}
	default:
		{
			GameLength_Control->OptionValueText->SetText(FText::FromString("Medium"));
			break;
		}
	}
}

void UWidget_HostOptions::SetGameSpeedControl()
{
	GameSpeed = TempGameSpeed;
}

void UWidget_HostOptions::OnStealingControlButtonPressed()
{
	TempStealingSetting = !TempStealingSetting;
	UpdateStealingText();
}

void UWidget_HostOptions::UpdateStealingText()
{
	if (TempStealingSetting)
		Stealing_Control->OptionValueText->SetText(FText::FromString("On"));
	else
		Stealing_Control->OptionValueText->SetText(FText::FromString("Off"));
}

void UWidget_HostOptions::SetStealingControl()
{
	StealingSetting = TempStealingSetting;
}

void UWidget_HostOptions::OnFertiliserControlButtonPressed()
{
	TempFertiliserSetting = !TempFertiliserSetting;
	UpdateFertiliserText();
}

void UWidget_HostOptions::UpdateFertiliserText()
{
	if (TempFertiliserSetting)
		Fertiliser_Control->OptionValueText->SetText(FText::FromString("On"));
	else
		Fertiliser_Control->OptionValueText->SetText(FText::FromString("Off"));
}

void UWidget_HostOptions::SetFertiliserControl()
{
	FertiliserSetting = TempFertiliserSetting;
}

void UWidget_HostOptions::OnCementControlButtonPressed()
{
	TempCementSetting = !TempCementSetting;
	UpdateCementText();
}

void UWidget_HostOptions::UpdateCementText()
{
	if (TempCementSetting)
		Cement_Control->OptionValueText->SetText(FText::FromString("On"));
	else
		Cement_Control->OptionValueText->SetText(FText::FromString("Off"));
}

void UWidget_HostOptions::SetCementControl()
{
	CementSetting = TempCementSetting;
}

/*void UWidget_HostOptions::GetPublicIPAddress()
{
	/*TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UWidget_HostOptions::OnPublicIPAddressResponse);

	// Set the URL to the IP retrieval service
	HttpRequest->SetURL(TEXT("https://api.ipify.org?format=json"));
	HttpRequest->SetVerb(TEXT("GET"));

	// Send the request
	HttpRequest->ProcessRequest();#1#
}

void UWidget_HostOptions::OnPublicIPAddressResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	/*if (bWasSuccessful && Response.IsValid())
	{
		FIPAddressResponse ParsedResponse;
		FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &ParsedResponse, 0, 0);
		FString IPAddress = ParsedResponse.ip;
		while (IPAddress.Contains("."))
		{
			int32 PointLocation{};
			IPAddress.FindLastChar('.', PointLocation);
			IPAddress.RemoveAt(PointLocation);
		}
		PublicIPAdress = IPAddress;
		PrivateIPAdress = "";
		UE_LOG(LogTemp, Warning, TEXT("Public IP Address: %s"), *(PrivateIPAdress + PublicIPAdress));
	}
	else
	{
		uint32 LocalIP{}; // Default to loopback address

		FOutputDeviceNull OutputDevice;
		bool bBindAll{true};
		TSharedPtr<FInternetAddr> LocalAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(OutputDevice, bBindAll);
		if (LocalAddress.IsValid())
		{
			LocalAddress->GetIp(LocalIP);
			PrivateIPAdress = FString::FromInt(LocalIP);
			PublicIPAdress = "";
			UE_LOG(LogTemp, Warning, TEXT("Private IP Address: %s"), *(PrivateIPAdress + PublicIPAdress));
		}
	}#1#
}*/
