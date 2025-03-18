#pragma once

#include "HTTPRequestTypes.generated.h"

namespace HTTPStatusMessage
{
	extern DEDICATEDSERVERS_API const FString SomethingWentWrong;
}

USTRUCT()
struct FDSMetaData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 httpStatusCode{};

	UPROPERTY()
	FString requestId{};

	UPROPERTY()
	int32 attempts{};

	UPROPERTY()
	double totalRetryDelay{};

	void Dump() const;
};

USTRUCT()
struct FDSListFleetsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FString> FleetIds{};

	UPROPERTY()
	FString NextToken{};

	void Dump() const;
};

USTRUCT()
struct FDSGameSession
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreationTime{};

	UPROPERTY()
	FString CreatorId{};

	UPROPERTY()
	int32 CurrentPlayerSessionCount{};

	UPROPERTY()
	FString DnsName{};

	UPROPERTY()
	FString FleetArn{};

	UPROPERTY()
	FString FleetId{};

	UPROPERTY()
	TMap<FString, FString> GameProperties{};

	UPROPERTY()
	FString GameSessionData{};

	UPROPERTY()
	FString GameSessionId{};
	
	UPROPERTY()
	FString IpAddress{};

	UPROPERTY()
	FString Location{};

	UPROPERTY()
	FString MatchmakerData{};

	UPROPERTY()
	int32 MaximumPlayerSessionCount{};

	UPROPERTY()
	FString Name{};

	UPROPERTY()
	FString PlayerSessionCreationPolicy{};

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status{};
	
	UPROPERTY()
	FString StatusReason{};
	
	UPROPERTY()
	FString TerminationTime{};

	void Dump() const;
};

USTRUCT()
struct FDSPlayerSession
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreationTime{};
	
	UPROPERTY()
	FString DnsName{};
	
	UPROPERTY()
	FString FleetArn{};
	
	UPROPERTY()
	FString FleetId{};
	
	UPROPERTY()
	FString GameSessionId{};
	
	UPROPERTY()
	FString IpAddress{};
	
	UPROPERTY()
	FString PlayerData{};
	
	UPROPERTY()
	FString PlayerId{};
	
	UPROPERTY()
	FString PlayerSessionId{};
	
	UPROPERTY()
	int32 Port{};
	
	UPROPERTY()
	FString Status{};
	
	UPROPERTY()
	FString TerminationTime{};
	
	void Dump() const;
};

USTRUCT()
struct FDSCodeDeliveryDetails
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString AttributeName;
	
	UPROPERTY()
	FString DeliveryMedium;
	
	UPROPERTY()
	FString Destination;
	
	void Dump() const;
};

USTRUCT()
struct FDSSignUpResponse
{
	GENERATED_BODY()
	
	UPROPERTY()
	FDSCodeDeliveryDetails CodeDeliveryDetails;
	
	UPROPERTY()
	bool UserConfirmed;
	
	UPROPERTY()
	FString UserSub;
	
	void Dump() const;
};

USTRUCT()
struct FDSNewDeviceMetaData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString DeviceGroupKey;
	
	UPROPERTY()
	FString DeviceKey;
	
	void Dump() const;
};

USTRUCT()
struct FDSAuthenticationResult
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString AccessToken;
	
	UPROPERTY()
	int32 ExpiresIn;
	
	UPROPERTY()
	FString IdToken;
	
	UPROPERTY()
	FDSNewDeviceMetaData NewDeviceMetadata;
	
	UPROPERTY()
	FString RefreshToken;
	
	UPROPERTY()
	FString TokenType;
	
	void Dump() const;
};

// ChallengeParameters struct
USTRUCT()
struct FDSChallengeParameters
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<FString, FString> Parameters;
	
	void Dump() const;
};

USTRUCT()
struct FDSInitiateAuthResponse
{
	GENERATED_BODY()
	
	UPROPERTY()
	FDSAuthenticationResult AuthenticationResult;
	
	UPROPERTY()
	FString ChallengeName;
	
	UPROPERTY()
	FDSChallengeParameters ChallengeParameters;
	
	UPROPERTY()
	FString Session;

	UPROPERTY()
	FString Email;
	
	void Dump() const;
};
