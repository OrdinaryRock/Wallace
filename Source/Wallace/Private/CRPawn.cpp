// Fill out your copyright notice in the Description page of Project Settings.


#include "CRPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CRGameMode.h"
#include "CRObstacle.h"
#include "CREndPoint.h"
#include "Blueprint/UserWidget.h"



// Sets default values
ACRPawn::ACRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	RootComponent = Cube;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Cube);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	ForwardForce = 2000;
	SideForce = 5;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACRPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Cube->OnComponentHit.AddDynamic(this, &ACRPawn::OnHit);
	Cube->OnComponentBeginOverlap.AddDynamic(this, &ACRPawn::OnBeginOverlap);
	GameMode = Cast<ACRGameMode>(GetWorld()->GetAuthGameMode());
	bGameEnded = false;
	Cube->SetSimulatePhysics(true);
	Mass = Cube->GetMass();

	if (DefaultHUD)
	{
		ActiveHUD = CreateWidget<UUserWidget>(GetWorld(), DefaultHUD);
		ActiveHUD->AddToViewport();
	}
}

// Called every frame
void ACRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bGameEnded) {
		FVector Impulse = FVector(Mass * ForwardForce * DeltaTime, 0.0f, 0.0f);
		Cube->AddImpulse(Impulse);

		FVector ActorLocation = GetActorLocation();
		if (ActorLocation.Z < -30)
		{
			GameEnded();
		}
	}

	DeltaSeconds = DeltaTime;
}

// Called to bind functionality to input
void ACRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveRight", this, &ACRPawn::MoveRight);

}

void ACRPawn::OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACRObstacle* Obstacle = Cast<ACRObstacle>(Other);
	if (Obstacle)
	{
		GameEnded();
	}
}

void ACRPawn::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACREndPoint* EndPoint = Cast<ACREndPoint>(OtherActor);
	if (EndPoint)
	{
		GameMode->LevelComplete();
	}

}

void ACRPawn::GameEnded()
{
	if (!bGameEnded)
	{
		bGameEnded = true;
		GetWorldTimerManager().SetTimer(EndGameTimer, this, &ACRPawn::EndGame, 2.0f, false);
	}
}

void ACRPawn::EndGame()
{
	GameMode->EndGame();
}

void ACRPawn::MoveRight(float AxisValue)
{
	if (!bGameEnded)
	{
		FVector Impulse = FVector(0.0f, Mass * SideForce * DeltaSeconds * AxisValue, 0.0f);
		Cube->AddImpulse(Impulse, "", true);
	}
}