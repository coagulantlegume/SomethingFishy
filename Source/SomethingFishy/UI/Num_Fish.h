// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Num_Fish.generated.h"

/**
 * 
 */
UCLASS()
class SOMETHINGFISHY_API UNum_Fish : public UUserWidget
{
	GENERATED_BODY()
public:
	UNum_Fish(const FObjectInitializer& initializer);

	virtual void NativeConstruct() override;

	void FishValueMore(int value);

	void FishValueLess(int value);

	void BaitValueMore(int value);

	void BaitValueLess(int value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* fishText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* baitText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim))
	UWidgetAnimation* NumFishLess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim))
	UWidgetAnimation* NumFishMore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* NumBaitLess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* NumBaitMore;
};
