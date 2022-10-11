// Fill out your copyright notice in the Description page of Project Settings.


#include "CRGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ACRGameMode::ACRGameMode()
{
	CurrentLevelIndex = 0;
}


void ACRGameMode::BeginPlay()
{
	Super::BeginPlay();

	Controller = GetWorld()->GetFirstPlayerController();


	FInputModeGameOnly InputMode;
	Controller->SetInputMode(InputMode);
	Controller->bShowMouseCursor = false;



	CheckLevel();




}

void ACRGameMode::CheckLevel()
{
	//FString CurrentLevelName = GetWorld()->GetMapName();

	FString CurrentLevelName = CleanLevelString(GetWorld());

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, TEXT("Index Before Find: ") + FString::FromInt(CurrentLevelIndex));

	Levels.Find(CurrentLevelName, CurrentLevelIndex);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, TEXT("Index After Find: ") + FString::FromInt(CurrentLevelIndex));


	if (CurrentLevelIndex < Levels.Num() - 1)
	{
		NextLevel = Levels[CurrentLevelIndex + 1];


	}
	else
	{
		NextLevel = "End";
	}
}

void ACRGameMode::EndGame()
{
	FString CurrentLevelName = CleanLevelString(GetWorld());
	FName LevelToLoad = FName(*CurrentLevelString);

	UGameplayStatics::OpenLevel(this, LevelToLoad, true);
}

void ACRGameMode::LevelComplete()
{
	if (DefaultLevelCompleteWidget) {
		LevelCompleteWidget = CreateWidget<UUserWidget>(GetWorld(), DefaultLevelCompleteWidget);
		LevelCompleteWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No default level complete widget selected!"));
	}

	GetWorldTimerManager().SetTimer(LevelSwapTimer, this, &ACRGameMode::LoadNextLevel, 2.0f, false);
}

void ACRGameMode::LoadNextLevel()
{

	if (Levels.Contains(NextLevel))
	{
		FName LevelToLoad = FName(*NextLevel);
		UGameplayStatics::OpenLevel(this, LevelToLoad, true);
	}
	else
	{
		if (LevelCompleteWidget)
		{
			LevelCompleteWidget->RemoveFromParent();

			if (DefaultGameCompleteWidget) {
				GameCompleteWidget = CreateWidget<UUserWidget>(GetWorld(), DefaultGameCompleteWidget);
				GameCompleteWidget->AddToViewport();

				FInputModeUIOnly InputMode;
				Controller->SetInputMode(InputMode);
				Controller->bShowMouseCursor = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No default game complete widget selected!!!"))
			}
		}
	}
}

FString ACRGameMode::CleanLevelString(UObject* WorldContextObject)
{
	if (GEngine)
	{
		FString Prefix = GEngine->GetWorldFromContextObject(WorldContextObject)->StreamingLevelsPrefix;
		FString LevelName = GetWorld()->GetMapName();
		return LevelName.RightChop(Prefix.Len());
	}
	return "No map found...";
}
