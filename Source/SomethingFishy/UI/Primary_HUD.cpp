// Fill out your copyright notice in the Description page of Project Settings.


#include "Primary_HUD.h"
#include "Num_Fish.h"

#include "Components/WidgetComponent.h"

APrimary_HUD::APrimary_HUD()
{

}

void APrimary_HUD::DrawHUD()
{
   Super::DrawHUD();
}

void APrimary_HUD::BeginPlay()
{
   Super::BeginPlay();

   if (NumFishClass)
   {
      FishWidget = CreateWidget<UNum_Fish>(GetWorld(), NumFishClass);
      if (FishWidget)
      {
         FishWidget->AddToViewport();
      }
   }
}

void APrimary_HUD::Tick(float DeltaSeconds)
{
   Super::Tick(DeltaSeconds);
}

void APrimary_HUD::FishValueMore(int value)
{
   if (FishWidget)
   {
      FishWidget->FishValueMore(value);
   }
}

void APrimary_HUD::FishValueLess(int value)
{
   if (FishWidget)
   {
      FishWidget->FishValueLess(value);
   }
}

void APrimary_HUD::BaitValueMore(int value)
{
   if (FishWidget)
   {
      FishWidget->BaitValueMore(value);
   }
}

void APrimary_HUD::BaitValueLess(int value)
{
   if (FishWidget)
   {
      FishWidget->BaitValueLess(value);
   }
}