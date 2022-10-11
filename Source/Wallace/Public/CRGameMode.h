// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CRGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WALLACE_API ACRGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:

	ACRGameMode();

	void EndGame();
	void LevelComplete();
	void LoadNextLevel();
	FString CleanLevelString(UObject* WorldContextObject);

protected:

	// Filters asset references so that you can only select from widgets
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "UMG")
		TSubclassOf<UUserWidget> DefaultLevelCompleteWidget;
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "UMG")
		UUserWidget* LevelCompleteWidget;
	
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "UMG")
		TSubclassOf<UUserWidget> DefaultGameCompleteWidget;
	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "UMG")
		UUserWidget* GameCompleteWidget;

	FTimerHandle LevelSwapTimer;

private:

	void BeginPlay() override;

	// An array of strings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Levels");
	TArray<FString> Levels;

	APlayerController* Controller;
	int32 CurrentLevelIndex = 0;
	FString NextLevel;

	void CheckLevel();

};
