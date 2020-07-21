// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Primary_HUD.generated.h"

/**
 * 
 */
UCLASS()
class SOMETHINGFISHY_API APrimary_HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	APrimary_HUD();
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void FishValueMore(int value);
		void FishValueLess(int value);

		void BaitValueMore(int value);
		void BaitValueLess(int value);

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> NumFishClass;

private:
	class UNum_Fish* FishWidget;
};
