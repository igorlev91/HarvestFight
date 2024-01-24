
#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Session.generated.h"


UCLASS()
class PROTOTYPE2_API UWidget_Session : public UUserWidget
{
	GENERATED_BODY()


/*public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Ping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 DestinationIP{};
	UPROPERTY(VisibleAnywhere)
	uint32 UDestinationIP{1722172436};

private:
	
	
	// Define the structure of an ICMP header
	struct FICMPHeader
	{
		uint8 Type{14};         // ICMP message type
		uint8 Code{0};         // ICMP message code
		uint16 Checksum{};    // ICMP checksum
		uint16 Identifier{};  // Identifier for this packet
		uint16 Sequence{};    // Sequence number for this packet
		uint32 OriginalTimestamp{};
		uint32 ReceiveTimestamp{};
		uint32 TransmitTimestamp{};
	};
	
	uint16 CalculateChecksum(void* Data, int32 Length);
	uint32 DecodeOriginalTimestamp(const uint8* PacketData);
	uint32 DecodeTransmitTimestamp(const uint8* PacketData);


	UPROPERTY(EditAnywhere)
	float PingCheckInterval{1.0f};
	UPROPERTY(VisibleAnywhere)
	float PingCheckTimer{};

	uint32 RecievedTime{};
	uint32 SentTime{};
	//
	uint16 RandomIdentifier{};*/
};
