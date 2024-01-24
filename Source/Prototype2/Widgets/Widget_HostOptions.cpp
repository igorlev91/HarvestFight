


#include "Widget_HostOptions.h"

/*#include "SocketSubsystem.h"*/
#include "Components/CheckBox.h"
#include "Kismet/KismetStringLibrary.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"

/*#include "JsonObjectConverter.h"*/

void UWidget_HostOptions::NativeOnInitialized()
{
	Super::NativeOnInitialized();
//	GetPublicIPAddress();
}

void UWidget_HostOptions::NativeConstruct()
{
	Super::NativeConstruct();
	StartButton->OnPressed.AddDynamic(this, &UWidget_HostOptions::OnHostPressed);
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

	if(GetOwningPlayer()->HasAuthority())
		GameInstance->bTeams = IsTeamsButton->IsChecked();
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
