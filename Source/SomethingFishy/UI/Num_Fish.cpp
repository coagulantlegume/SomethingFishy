// Fill out your copyright notice in the Description page of Project Settings.


#include "Num_Fish.h"

#include "Runtime/UMG/Public/Umg.h"

UNum_Fish::UNum_Fish(const FObjectInitializer& initializer) : UUserWidget(initializer)
{

}

void UNum_Fish::NativeConstruct()
{
   Super::NativeConstruct();
}

void UNum_Fish::FishValueMore(int value)
{
   if (NumFishMore)
   {
      PlayAnimation(NumFishMore, 0.f, 1, EUMGSequencePlayMode::Forward, 1);
   }

   fishText->SetText(FText::FromString(FString::FromInt(value) + " fish"));
}

void UNum_Fish::FishValueLess(int value)
{
   if (NumFishLess)
   {
      PlayAnimation(NumFishLess, 0.f, 1, EUMGSequencePlayMode::Forward, 1);
   }

   fishText->SetText(FText::FromString(FString::FromInt(value) + " fish"));
}

void UNum_Fish::BaitValueMore(int value)
{
   if (NumBaitMore)
   {
      PlayAnimation(NumBaitMore, 0.f, 1, EUMGSequencePlayMode::Forward, 1);
   }

   baitText->SetText(FText::FromString(FString::FromInt(value) + " bait"));
}

void UNum_Fish::BaitValueLess(int value)
{
   if (NumBaitLess)
   {
      PlayAnimation(NumBaitLess, 0.f, 1, EUMGSequencePlayMode::Forward, 1);
   }

   baitText->SetText(FText::FromString(FString::FromInt(value) + " bait"));
}