// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Dungeon.h"

// Sets default values for this component's properties
UC_Dungeon::UC_Dungeon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	FString IntAsString = FString::Printf(TEXT("%d"), 1);
	FString StaticMeshComponentName = FString::Printf(TEXT("StaticMeshComponent_%s"), *IntAsString);

	m_pStaticBox = CreateDefaultSubobject<UStaticMeshComponent>(*StaticMeshComponentName);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MeshAsset.Succeeded())
	{
		m_pStaticBox->SetStaticMesh(MeshAsset.Object);
	}

}


// Called when the game starts
void UC_Dungeon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UC_Dungeon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UC_Dungeon::SetVariables(const FVector center, const int32 x, const int32 y)
{
	m_Center = center;
	m_pStaticBox->SetRelativeLocation(m_Center);
	m_pStaticBox->SetRelativeScale3D(FVector{ x / 100.0f, y / 100.0f, 1.0f });
}

void UC_Dungeon::SetVisibility(bool isVisible)
{
	m_pStaticBox->bHiddenInGame = isVisible;
	m_pStaticBox->MarkRenderStateDirty();
}