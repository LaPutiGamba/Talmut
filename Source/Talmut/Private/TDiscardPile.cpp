#include "TDiscardPile.h"
#include "TCard.h"
#include "Core/TGameState.h"
#include "Kismet/GameplayStatics.h"

void ATDiscardPile::BeginPlay()
{
	Super::BeginPlay();

	TGameState = GetWorld()->GetGameState<ATGameState>();

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerLocation = Pawn->GetActorLocation();

	FVector ForwardVector = Pawn->GetActorForwardVector();
	FVector RightVector = Pawn->GetActorRightVector();
	FVector Offset = ForwardVector * 130 + RightVector + FVector(0, 0, 38.f);

	FVector ActorLocation = PlayerLocation + Offset;

	SetActorLocation(ActorLocation);
	SetActorRotation(Pawn->GetActorRotation());
}

void ATDiscardPile::AddCardToDiscardPile(class ATCard* Card)
{
	Card->ServerMoveCard(GetActorLocation(), GetActorRotation());
}

void ATDiscardPile::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (!TGameState)
		return;

	for (ATCard* Card : TGameState->GetDiscardCards())
		Card->GetCardMesh()->SetRenderCustomDepth(true);
}

void ATDiscardPile::OnEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (!TGameState)
		return;

	for (ATCard* Card : TGameState->GetDiscardCards())
		Card->GetCardMesh()->SetRenderCustomDepth(false);
}