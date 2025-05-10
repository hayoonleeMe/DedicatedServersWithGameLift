

#include "Lobby/LobbyPlayerInfo.h"

void FLobbyPlayerInfoArray::AddPlayer(const FLobbyPlayerInfo& NewPlayerInfo)
{
	const int32 Index = Players.Add(NewPlayerInfo);
	MarkItemDirty(Players[Index]);
	Players[Index].PostReplicatedAdd(*this);
}

void FLobbyPlayerInfoArray::RemovePlayer(const FString& Username)
{
	for (int32 Index = 0; Index < Players.Num(); ++Index)
	{
		FLobbyPlayerInfo& PlayerInfo = Players[Index];
		if (PlayerInfo.Username == Username)
		{
			PlayerInfo.PreReplicatedRemove(*this);
			Players.RemoveAtSwap(Index);
			MarkArrayDirty();
			return;
		}
	}
}
