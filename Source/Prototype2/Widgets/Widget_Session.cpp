


#include "Widget_Session.h"

#include "Interfaces/OnlineSessionInterface.h"

//#include "IPAddress.h"
//#include "SocketSubsystem.h"
//#include "Sockets.h"
//#include "Components/TextBlock.h"
//#include "Internationalization/FastDecimalFormat.h"
//#include "Kismet/KismetStringLibrary.h"
//
//
//void UWidget_Session::NativeOnInitialized()
//{
//	Super::NativeOnInitialized();
//	RandomIdentifier = rand() % 13376;
//}
//
//void UWidget_Session::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
//{
//	Super::NativeTick(MyGeometry, InDeltaTime);
//
//	if (PingCheckTimer > 0)
//	{
//		PingCheckTimer -= InDeltaTime;
//	}
//	if (PingCheckTimer <= 0)
//	{
//		PingCheckTimer = PingCheckInterval;
//		
//		{
//			//UDestinationIP = DestinationIP;
//			UE_LOG(LogTemp, Warning, TEXT("Recieved Host IP: %s"), *FString::FromInt(UDestinationIP));
//		}
//		
//		uint32 SessionPing{};
//		int32 FinalBytesReceived{};
//		{
//			// Create an ICMP packet for the ping request
//			FICMPHeader ICMPHeader{};
//			ICMPHeader.Type = 13;
//			ICMPHeader.Checksum = CalculateChecksum(&ICMPHeader, sizeof(ICMPHeader));
//			ICMPHeader.Identifier = 13371; // Pass In Server Time
//			ICMPHeader.Sequence = 13371; 
//
//			// Ping packet data and size
//			uint8 PingPacketData[sizeof(ICMPHeader)];
//			FMemory::Memcpy(PingPacketData, &ICMPHeader, sizeof(ICMPHeader));
//			auto Socket = ISocketSubsystem::Get()->CreateSocket(NAME_Stream, TEXT("Timestamp"), false);
//			int32 RecieveBufferSize = sizeof(ICMPHeader);
//			int32 SendBufferSize = sizeof(ICMPHeader);
//			Socket->SetReceiveBufferSize(sizeof(ICMPHeader), RecieveBufferSize);
//			Socket->SetSendBufferSize(sizeof(ICMPHeader), SendBufferSize);
//			Socket->SetNonBlocking(true);
//			Socket->SetReuseAddr(false);
//			Socket->SetLinger(false, 0);
//			Socket->SetRecvErr(true);
//			TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get()->CreateInternetAddr();
//			RemoteAddress->SetIp(UDestinationIP);
//			int32 NumberOfBytesSent{};
//			Socket->SendTo(PingPacketData, sizeof(ICMPHeader), NumberOfBytesSent, *RemoteAddress);
//	
//			uint8 Buffer[sizeof(ICMPHeader)];
//			TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get()->CreateInternetAddr();
//			int32 BytesReceived = 0;
//			Socket->RecvFrom(Buffer, sizeof(Buffer), BytesReceived, *Sender);
//			SentTime = DecodeOriginalTimestamp(Buffer);
//			Socket->Close();
//			ISocketSubsystem::Get()->DestroySocket(Socket);
//		}
//		{
//			// Create an ICMP packet for the ping request
//			FICMPHeader ICMPHeader{};
//			ICMPHeader.Type = 14;
//			ICMPHeader.Checksum = CalculateChecksum(&ICMPHeader, sizeof(ICMPHeader));
//			ICMPHeader.Identifier = 13371; // Pass In Server Time
//			ICMPHeader.Sequence = 13371; 
//
//			// Ping packet data and size
//			uint8 PingPacketData[sizeof(ICMPHeader)];
//			FMemory::Memcpy(PingPacketData, &ICMPHeader, sizeof(ICMPHeader));
//			FSocket* Socket = ISocketSubsystem::Get()->CreateSocket(NAME_Stream, TEXT("Timestamp reply"), false);
//			int32 RecieveBufferSize = sizeof(ICMPHeader);
//			int32 SendBufferSize = sizeof(ICMPHeader);
//			Socket->SetReceiveBufferSize(sizeof(ICMPHeader), RecieveBufferSize);
//			Socket->SetSendBufferSize(sizeof(ICMPHeader), SendBufferSize);
//			Socket->SetNonBlocking(true);
//			Socket->SetReuseAddr(false);
//			Socket->SetLinger(false, 0);
//			Socket->SetRecvErr(true);
//			TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get()->CreateInternetAddr();
//			
//			RemoteAddress->SetIp(UDestinationIP);
//			int32 NumberOfBytesSent{};
//			Socket->SendTo(PingPacketData, sizeof(ICMPHeader), NumberOfBytesSent, *RemoteAddress);
//			uint8 Buffer[sizeof(ICMPHeader)];
//			TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get()->CreateInternetAddr();
//
//			Socket->RecvFrom(Buffer, sizeof(Buffer), FinalBytesReceived, *Sender);
//			RecievedTime = DecodeTransmitTimestamp(Buffer);
//			Socket->Close();
//			ISocketSubsystem::Get()->DestroySocket(Socket);
//
//			SessionPing = RecievedTime - SentTime;
//			FString stringPing = FString::FromInt(SessionPing);
//			FString FinalPing{};
//			FinalPing.AppendChar(stringPing[0]);
//			FinalPing.AppendChar(stringPing[1]);
//			UE_LOG(LogTemp, Warning, TEXT("Session %s Has Ping: %s"), *FString::FromInt(UDestinationIP), *FinalPing);
//			Ping->SetText(FText::FromString(FinalPing));
//		}
//	}
//}
//
//uint16 UWidget_Session::CalculateChecksum(void* Data, int32 Length)
//{
//	unsigned short* buf = static_cast<unsigned short*>(Data);
//	unsigned int sum=0;
//	unsigned short result;
//
//	for ( sum = 0; Length > 1; Length -= 2 )
//		sum += *buf++;
//	if ( Length == 1 )
//		sum += *(unsigned char*)buf;
//	sum = (sum >> 16) + (sum & 0xFFFF);
//	sum += (sum >> 16);
//	result = ~sum;
//	return result;
//}
//
//uint32 UWidget_Session::DecodeOriginalTimestamp(const uint8* PacketData)
//{
//	const FICMPHeader* ICMPHeader = reinterpret_cast<const FICMPHeader*>(PacketData);
//
//	return ICMPHeader->OriginalTimestamp;
//}
//uint32 UWidget_Session::DecodeTransmitTimestamp(const uint8* PacketData)
//{
//	const FICMPHeader* ICMPHeader = reinterpret_cast<const FICMPHeader*>(PacketData);
//
//	return ICMPHeader->TransmitTimestamp;
//}
//
//
//
void UWidget_Session::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
