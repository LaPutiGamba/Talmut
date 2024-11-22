#include "TCard.h"
#include "TDiscardPile.h"
#include "Core/TGameState.h"
#include "Kismet/GameplayStatics.h"

ATCard::ATCard()
{
	PrimaryActorTick.bCanEverTick = false;

	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
	RootComponent = CardMesh;

	CardMovingVelocity = 1.f;
	CardRotationVelocity = 1.f;

	bIsSelected = false;

	bReplicates = true;
}

void ATCard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CardMesh->OnBeginCursorOver.AddDynamic(this, &ATCard::OnBeginCursorOver);
	CardMesh->OnEndCursorOver.AddDynamic(this, &ATCard::OnEndCursorOver);
}

void ATCard::EnableCardCollision(bool bEnable)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s: %s EnableCardCollision: %s"), *GetDebugStringForWorld(GetWorld()), *GetName(), bEnable ? TEXT("TRUE") : TEXT("FALSE"));
	CardMesh->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void ATCard::MulticastInitialize_Implementation(UTexture* CardTexture, uint8 CardOffset)
{
	SetCardTexture(CardTexture);
	SetCardValue(CardTexture);

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector PlayerLocation = Pawn->GetActorLocation();

	FVector ForwardVector = Pawn->GetActorForwardVector();
	FVector RightVector = Pawn->GetActorRightVector();
	FVector Offset = ForwardVector * 130 + RightVector * 25 + FVector(0, 0, 38.f + CardOffset * 0.2f);

	FVector ActorLocation = PlayerLocation + Offset;
	FRotator ActorRotation = FRotator(Pawn->GetActorRotation().Pitch, Pawn->GetActorRotation().Yaw, 180.f);

	SetActorLocation(ActorLocation);
	SetActorRotation(ActorRotation);

	CardMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATCard::MoveCard(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	GetWorldTimerManager().SetTimer(MoveCardTimerHandle, [this, TargetLocation, TargetRotation]() {
		FVector CurrentLocation = GetActorLocation();
		FRotator CurrentRotation = GetActorRotation();

		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, 0.1f, CardMovingVelocity);
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, 0.1f, CardRotationVelocity);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);

		if (FVector::Dist(NewLocation, TargetLocation) < 0.1f && NewRotation.Equals(TargetRotation, 0.1f))
			GetWorld()->GetTimerManager().ClearTimer(MoveCardTimerHandle);
	}, 0.01f, true);
}

void ATCard::ServerMoveCard_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	MulticastMoveCard(TargetLocation, TargetRotation);
}

void ATCard::MulticastMoveCard_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	GetWorldTimerManager().SetTimer(MoveCardTimerHandle, [this, TargetLocation, TargetRotation]() {
		FVector CurrentLocation = GetActorLocation();
		FRotator CurrentRotation = GetActorRotation();

		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, 0.1f, CardMovingVelocity);
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, 0.1f, CardRotationVelocity);
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);

		if (FVector::Dist(NewLocation, TargetLocation) < 0.1f && NewRotation.Equals(TargetRotation, 0.1f))
			GetWorld()->GetTimerManager().ClearTimer(MoveCardTimerHandle);
	}, 0.01f, true);
}

void ATCard::ServerMoveDrawedCard_Implementation(const FVector& TargetLocation, FRotator TargetRotation, APawn* OwningPawn)
{
	MulticastMoveDeckCard(TargetLocation, TargetRotation, OwningPawn);
}

void ATCard::MulticastMoveDeckCard_Implementation(const FVector& TargetLocation, FRotator TargetRotation, APawn* OwningPawn)
{
	if (OwningPawn->IsLocallyControlled()) {
		TargetRotation.Roll = 0.f;
		MoveCard(TargetLocation, TargetRotation);
	} else {
		TargetRotation.Roll = 180.f;
		MoveCard(TargetLocation, TargetRotation);
	}

	CardMesh->SetRenderCustomDepth(false);
}

void ATCard::ServerMoveDiscardedCard_Implementation(ATPawn* Pawn)
{
	if (auto GameState = GetWorld()->GetGameState<ATGameState>()) {
		GameState->GetDiscardCards().Add(this);

		switch (CardValue) {
		case EPokerCardValue::CV_Jack:
			Pawn->IncreaseJackActivated();
			break;
		case EPokerCardValue::CV_Queen:
			Pawn->IncreaseQueenActivated();
			Pawn->ClientStartQueenActivated();
			break;
		case EPokerCardValue::CV_King:
			Pawn->IncreaseKingActivated();
			Pawn->ClientStartKingActivated();
			break;
		default:
			break;
		}

		MulticastMoveDiscardedCard(Pawn);
	}
}

void ATCard::MulticastMoveDiscardedCard_Implementation(class ATPawn* Pawn)
{
	if (auto GameState = GetWorld()->GetGameState<ATGameState>()) {
		FVector TargetLocation = GameState->DiscardPile->GetActorLocation();
		TargetLocation.Z = TargetLocation.Z + (GameState->GetDiscardCards().Num() * 0.2f);
		FRotator TargetRotation = GameState->DiscardPile->GetActorRotation();

		MoveCard(TargetLocation, TargetRotation);
	}
}

void ATCard::SetCardTexture(UTexture* CardTexture)
{
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(CardMesh->GetMaterial(0), this);
	DynamicMaterial->SetTextureParameterValue("CardTexture", CardTexture);
	CardMesh->SetMaterial(0, DynamicMaterial);
}

void ATCard::SetCardValue(UTexture* CardTexture)
{
	// Get card value from texture name
	FString TextureName = CardTexture->GetName();
	FString CardValueStr = TextureName.Mid(TextureName.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd) + 1);

	int32 CardValueInt;

	if (CardValueStr == "J")
		CardValueInt = 11;
	else if (CardValueStr == "Q")
		CardValueInt = 12;
	else if (CardValueStr == "K")
		CardValueInt = 13;
	else
		CardValueInt = FCString::Atoi(*CardValueStr);

	// Set card value
	CardValue = static_cast<EPokerCardValue>(CardValueInt);
}

void ATCard::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	CardMesh->SetRenderCustomDepth(true);
}

void ATCard::OnEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (!bIsSelected)
		CardMesh->SetRenderCustomDepth(false);
}