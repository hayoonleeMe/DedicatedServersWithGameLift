#include "HTTPRequestTypes.h"

#include "DedicatedServers/DedicatedServers.h"

void FDSMetaData::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("MetaData:"));
	UE_LOG(LogDedicatedServers, Log, TEXT("httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogDedicatedServers, Log, TEXT("requestId: %s"), *requestId);
	UE_LOG(LogDedicatedServers, Log, TEXT("attempts: %d"), attempts);
	UE_LOG(LogDedicatedServers, Log, TEXT("totalRetryDelay: %f"), totalRetryDelay);
}
