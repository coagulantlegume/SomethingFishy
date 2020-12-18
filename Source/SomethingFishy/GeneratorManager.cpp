// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratorManager.h"

// Sets default values
AGeneratorManager::AGeneratorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGeneratorManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGeneratorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

