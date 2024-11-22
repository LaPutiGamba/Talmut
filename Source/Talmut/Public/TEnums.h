#pragma once

UENUM(BlueprintType)
enum class EPokerCardValue : uint8
{
	CV_Uninitialized UMETA(DisplayName = "Uninitialized"),
	CV_Ace UMETA(DisplayName = "Ace"),
	CV_Two UMETA(DisplayName = "Two"),
	CV_Three UMETA(DisplayName = "Three"),
	CV_Four UMETA(DisplayName = "Four"),
	CV_Five UMETA(DisplayName = "Five"),
	CV_Six UMETA(DisplayName = "Six"),
	CV_Seven UMETA(DisplayName = "Seven"),
	CV_Eight UMETA(DisplayName = "Eight"),
	CV_Nine UMETA(DisplayName = "Nine"),
	CV_Ten UMETA(DisplayName = "Ten"),
	CV_Jack UMETA(DisplayName = "Jack"),
	CV_Queen UMETA(DisplayName = "Queen"),
	CV_King UMETA(DisplayName = "King")
};

UENUM(BlueprintType)
enum class EStateOfTheGame : uint8
{
	GS_Uninitialized UMETA(DisplayName = "Uninitialized"),
	GS_WaitingForPlayers UMETA(DisplayName = "Lobby"),
	GS_Dealing UMETA(DisplayName = "Dealing"),
	GS_StartShowingInitialCards UMETA(DisplayName = "StartShowingInitialCards"),
	GS_ShowingInitialCards UMETA(DisplayName = "ShowingInitialCards"),
	GS_StartPlaying UMETA(DisplayName = "Start Playing"),
	GS_End UMETA(DisplayName = "End")
};