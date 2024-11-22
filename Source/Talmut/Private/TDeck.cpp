#include "TDeck.h"
#include "TCard.h"
#include "Components/BoxComponent.h"
#include "Core/TGameState.h"
#include "Kismet/GameplayStatics.h"

ATDeck::ATDeck()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneRootComponent);

	DeckCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DeckCollision"));
	DeckCollision->SetupAttachment(RootComponent);
	DeckCollision->SetCollisionResponseToAllChannels(ECR_Block);
	DeckCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DeckCollision->SetBoxExtent(FVector(14.f, 10.f, 16.f));
	DeckCollision->SetRelativeLocation(FVector(0.f, 0.f, 16.f));
	DeckCollision->SetIsReplicated(true);
}

void ATDeck::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DeckCollision->OnBeginCursorOver.AddDynamic(this, &ATDeck::OnBeginCursorOver);
	DeckCollision->OnEndCursorOver.AddDynamic(this, &ATDeck::OnEndCursorOver);
}

void ATDeck::BeginPlay()
{
	Super::BeginPlay();

	TGameState = GetWorld()->GetGameState<ATGameState>();

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerLocation = Pawn->GetActorLocation();

	FVector ForwardVector = Pawn->GetActorForwardVector();
	FVector RightVector = Pawn->GetActorRightVector();
	FVector Offset = ForwardVector * 130 + RightVector * 25 + FVector(0, 0, 38.f);

	FVector ActorLocation = PlayerLocation + Offset;

	SetActorLocation(ActorLocation);
	SetActorRotation(Pawn->GetActorRotation());
}

void ATDeck::EnableBoxCollision(bool bEnable)
{
	DeckCollision->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ATDeck::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (!TGameState)
		return;

	for (ATCard* Card : TGameState->GetDeckCards())
		Card->GetCardMesh()->SetRenderCustomDepth(true);
}

void ATDeck::OnEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (!TGameState)
		return;

	for (ATCard* Card : TGameState->GetDeckCards())
		Card->GetCardMesh()->SetRenderCustomDepth(false);
}