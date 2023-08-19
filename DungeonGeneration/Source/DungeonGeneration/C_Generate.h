// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include <vector>
#include "Math/Vector.h" // Include the FVector header
#include "Math/RandomStream.h"
#include "Engine.h"
#include "Components/StaticMeshComponent.h" // Include the StaticMeshComponent header.
#include "DataTypes.h"
#include "C_Grid.h"
#include "C_Dungeon.h"
#include "C_Graph.h"

#include "C_Generate.generated.h"



UCLASS()
class DUNGEONGENERATION_API AC_Generate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Generate();

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
        bool m_NewSeed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Number of Rooms", meta = (ClampMin = "3", ClampMax = "20"))
    int32 m_NumberRooms;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrawDebug")
    //    bool m_DrawDebug = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrawDebug")
        bool m_DrawDebugTriangulation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrawDebug")
        bool m_DrawDebugMST = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrawDebug")
        bool m_DrawDebugAStar = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrawDebug")
        bool m_DrawDebugGrid = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

    void CreateMeshes();
    void SetCells();
    void DrawDebugFunctions() const;

    int32 m_MaxNumRooms;
    int32 m_Seed = 0;
    int32 m_Margin = 0;

    AC_Grid* m_pGrid = nullptr;
    UC_Graph* m_pGraph = nullptr;
    TArray<UC_Dungeon*> m_pDungeonArray;
};
