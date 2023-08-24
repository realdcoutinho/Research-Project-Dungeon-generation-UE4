// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataTypes.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"


#include "C_Grid.h"


#include "C_Graph.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONGENERATION_API UC_Graph : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UC_Graph();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	TArray<FVector> m_Locations;

	void SetPointsArray(TArray<FVector>& points);
	void AddPoint(FVector& point);
	void CreateSuperTriangle(int32 increment, int32 numRooms, int32 margin);
	void DeletePoints();

	void TriangulationAlgorithm();

	void Path();




private:


	FTriangle m_SuperTriangle;
	TArray<FTriangle> m_TriangulationTrianglesArray;
	TArray<FTriangulationEdge> m_TriangulationEdgesArray;
	TArray<FTriangulationEdge> m_MSTEdgesArray;;

	TArray<FTriangulationNode> m_NodesArray;

	void GetEdges();
	void CreateNodes();
	void FindMinimumSpanningTree(TArray<FTriangulationEdge>& edges);

public:

	//DEBUGDRAW
	void DrawDebugMTS() const;
	void DrawDebugTriangulation() const;

private:

	//HELPERS
	bool isPointInsideCircumcircle(const FVector& p, const FTriangle& t) const;
	bool sharesEdge(const FTriangle& t, const FEdge& e) const;
	bool hasCommonVertex(const FTriangle& t1, const FTriangle& t2) const;
	void Union(FTriangulationNode* rootA, FTriangulationNode* rootB);
	FTriangulationNode* FindRoot(FTriangulationNode* node);
};
