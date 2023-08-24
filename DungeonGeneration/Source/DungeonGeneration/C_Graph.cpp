// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Graph.h"

// Sets default values for this component's properties
UC_Graph::UC_Graph()
{

	PrimaryComponentTick.bCanEverTick = true;
}



// Called when the game starts
void UC_Graph::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UC_Graph::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UC_Graph::SetPointsArray(TArray<FVector>& points)
{
	DeletePoints();
	m_Locations = points;
}

void UC_Graph::AddPoint(FVector& point)
{
	m_Locations.Add(point);
}

void UC_Graph::DeletePoints()
{
	m_Locations.Empty();
}

void UC_Graph::CreateSuperTriangle(int32 increment, int32 numRooms, int32 margin)
{
	float i = static_cast<float>(increment);
	float n = static_cast<float>(numRooms);
	float m = static_cast<float>(margin);
	FVector v0 = { -1 * (n * i + m), -1 * (n * i + m), 0 };
	FVector v1 = { (n * i + m), 0, 0 };
	FVector v2 = { -1 * (n * i + m), 1 * (n * i + m), 0 };

    //SUPER TRINAGLE HAS TO BE REALLY BIG
	m_SuperTriangle = FTriangle(v0 * 100, v1 * 100, v2 * 100);
}

void UC_Graph::TriangulationAlgorithm()
{
    // Create an empty triangle graph data structure
    m_TriangulationTrianglesArray.Empty();

    // Add a super-triangle to the triangulation (large enough to contain all points)
    // You need to define a large enough triangle here, depending on your use case.
    m_TriangulationTrianglesArray.Add(m_SuperTriangle);

    // Add all the points one by one to the triangulation
    for (FVector& point : m_Locations)
    {
        //define an empty array called bad triangles
        TArray<FTriangle> badTriangles;

        // Find all the triangles that are no longer valid due to the insertion
        for (FTriangle& triangle : m_TriangulationTrianglesArray)
        {
            //for every point in locations, inside a triangle
            if (isPointInsideCircumcircle(point, triangle))
            {
                //add said point to badTriangles
                badTriangles.Add(triangle);
            }
        }

        //define an empty array of edges
        TArray<FTriangulationEdge> polygon;

        
        for (const FTriangle& triangle : badTriangles)
        {
            //for every edge of all bad triangles
            for (const FTriangulationEdge& edge : triangle._edgesArray)
            {
                //if edge is not shared by any other edge of any other triangle
                bool shared = std::any_of(badTriangles.begin(), badTriangles.end(), [&](const FTriangle& otherTriangle)
                    {
                        return &triangle != &otherTriangle && sharesEdge(otherTriangle, edge);
                    });

              
                if (!shared)
                {   
                    //add said edge to polygonArray
                    polygon.Add(edge);
                }
            }
        }

        // Remove bad triangles from the data structure
        for (FTriangle& triangle : badTriangles)
        {
            m_TriangulationTrianglesArray.Remove(triangle);
        }

        // Re-triangulate the polygonal hole
        for (FTriangulationEdge& edge : polygon)
        {
            FTriangle newTri(edge.Vertex[0], edge.Vertex[1], point);
            m_TriangulationTrianglesArray.Add(newTri);
        }
    }

    //finally, if any triangle still in the array has a commmon vertex with the original supoer triangle, remove said triangle from the array
    m_TriangulationTrianglesArray.RemoveAllSwap([&](const FTriangle& triangle) 
    {
        for (const FVector& vertex : triangle._vertices) 
        {
            if (hasCommonVertex(triangle, m_SuperTriangle)) 
            {
                return true;
            }
        }
         return false;
    });

    //jump into next step
    GetEdges();
 }

void UC_Graph::GetEdges()
{
    //empty the m_TriangulationEdgesArray 
    m_TriangulationEdgesArray.Empty();

    //loop over all triangles
    for (const FTriangle& triangle : m_TriangulationTrianglesArray)
    {
        //for each edge 
        for (const FTriangulationEdge& edge : triangle._edgesArray)
        {
            //add only unique edges to the m_TriangulationEdgesArray
            //you whould not have any duplicates
            m_TriangulationEdgesArray.AddUnique(edge);
        }
    }

    //jump into next step
    CreateNodes();
}

void UC_Graph::CreateNodes()
{
    //empty the array
    m_NodesArray.Empty();

    // Step 1: Create a map to store nodes based on their locations
    TMap<FVector, FTriangulationNode> nodesMap;

    // Step 2: Given an array of edges (assuming it's named "m_TriangulationEdgesArray")

    //go over all edges, now without any duplicates
    for (FTriangulationEdge& edge : m_TriangulationEdgesArray)
    {
        // Step 3: Create or find the nodes corresponding to the edge's start and end points
        //first try to find
        FTriangulationNode* node1 = nodesMap.Find(edge.Vertex[0]);
        FTriangulationNode* node2 = nodesMap.Find(edge.Vertex[1]);

        //if node 1 not found
        if (!node1)
        {
            //create new node
            FTriangulationNode newNode;
            //get nodes start location == edge[0]
            FVector loc = edge.Vertex[0];
            //add location
            newNode.AddLocation(loc);
            //add location and node to map
            nodesMap.Add(edge.Vertex[0], newNode);
            //assign it
            node1 = &nodesMap[edge.Vertex[0]];
        }

        //if node 2 not found
        if (!node2)
        {
            //create new node
            FTriangulationNode newNode;
            //get nodes start location == edge[1]
            FVector loc = edge.Vertex[1];
            //add location
            newNode.AddLocation(loc);
            //add location and node to map
            nodesMap.Add(edge.Vertex[1], newNode);
            //assign it
            node2 = &nodesMap[edge.Vertex[1]];
        }

        //// Step 4: Add the edge to the connection list of both nodes
        node1->AddConnection(edge);
        node2->AddConnection(edge);
    }

    // Step 5: Extract the unique nodes from the map into an array
    for (const auto& pair : nodesMap)
    {
        m_NodesArray.AddUnique(pair.Value);
    }

    TArray<FTriangulationEdge> allEdges;

    //for all nodes in the array
    for (FTriangulationNode& node : m_NodesArray)
    {
        for (FTriangulationEdge& edge : node._connections)
        {
            FTriangulationNode* startNode = &node;
            FTriangulationNode* endNode = nullptr;

            // Find the corresponding node for the end vertex of the edge.
            for (FTriangulationNode& otherNode : m_NodesArray)
            {
                if (otherNode._location == edge.Vertex[1])
                {
                    endNode = &otherNode;
                    break;
                }
            }

            if (endNode != nullptr)
            {
                FTriangulationEdge newEdge = FTriangulationEdge(edge.Vertex[0], edge.Vertex[1], startNode, endNode);
                allEdges.Add(newEdge);
                //newSet.AddUnique(newEdge);
            }
        }

        node._connections = allEdges;

        // Initialize the Union-Find data structure.
        node._parent = &node;
        node._rank = 0;

    }

    //jump into next step
    FindMinimumSpanningTree(allEdges);
}


void UC_Graph::FindMinimumSpanningTree(TArray<FTriangulationEdge>& edges)
{
    //empty array
    m_MSTEdgesArray.Empty();
    
    // Sort the edges based on their cost in non-decreasing order.
    // further explain here
    edges.Sort([](const FTriangulationEdge& EdgeA, const FTriangulationEdge& EdgeB) { return EdgeA._cost < EdgeB._cost; });


    int edgeCount = 0;
    //for each each
    for (FTriangulationEdge& edge : edges)
    {
        //find root of starting node
        FTriangulationNode* rootA = FindRoot(edge._startNode);
        //find root of end node
        FTriangulationNode* rootB = FindRoot(edge._endNode);

        // Check if including this edge will create a cycle.
        if (rootA != rootB)
        {
            //if they are not the same
            //add to MSTArray
            m_MSTEdgesArray.Add(edge);
            //unite both root Nodes
            Union(rootA, rootB);
            //increase the edge count
            edgeCount++;
            //if edge count is greater than number of array, exit loop
            if (edgeCount == (m_NodesArray.Num() - 1))
            {
                break; // Minimum spanning tree found.
            }
        }
    }

    Path();
}


void UC_Graph::Path()
{
    //get grid
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AC_Grid::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        AC_Grid* pGrid = Cast<AC_Grid>(FoundActors[0]);

        //for all the edges in Minimum Spanning Tree
        for (const FTriangulationEdge& edge : m_MSTEdgesArray)
        {
            //get its start node
            FTriangulationNode* start = edge._startNode;
            //get the end node
            FTriangulationNode* end = edge._endNode;

            //find their locations
            FVector startPos = start->_location;
            FVector endPos = end->_location;


            //Chose Algorithim for each path
            pGrid->AStartPath(startPos, endPos);
        }
    }
}

bool UC_Graph::isPointInsideCircumcircle(const FVector& p, const FTriangle& t) const
{
    return (FVector::DistSquaredXY(p, t._circumCenter)) < (t._circumRadius * t._circumRadius);
}

bool UC_Graph::sharesEdge(const FTriangle& t, const FEdge& e) const
{
    // Check if the triangle shares the specified edge.
    // Return true if the triangle shares the edge; otherwise, false.
    for (const FEdge& edge : t._edgesArray)
    {
        if (edge == e)
            return true;
    }
    return false;
}

bool UC_Graph::hasCommonVertex(const FTriangle& t1, const FTriangle& t2) const
{
    for (const FVector& vertice : t1._vertices)
    {
        if ((vertice == t2._vertices[0]) || (vertice == t2._vertices[1]) || (vertice == t2._vertices[2]))
            return true;
    }
    return false;

}

// Helper function to perform union operation in the Union-Find data structure.
void UC_Graph::Union(FTriangulationNode* rootA, FTriangulationNode* rootB)
{
    //if rank of start < end
    if (rootA->_rank < rootB->_rank)
    {
        //B parents A
        rootA->_parent = rootB;
    }

    //if rank of start > end
    else if (rootA->_rank > rootB->_rank)
    {
        //A parents B
        rootB->_parent = rootA;
    }
    else
    {
        //else, meaning they are the same
        //A parents B, and increases A's rank
        rootB->_parent = rootA;
        rootA->_rank++;
    }
}

FTriangulationNode* UC_Graph::FindRoot(FTriangulationNode* node)
{
    //is nodes parent == node?
    //if not
    // node == node's parent
    while (node->_parent != node)
    {
        node = node->_parent;
    }
    //when node found, return node
    return node;
}


void UC_Graph::DrawDebugMTS() const
{
    for (const FTriangulationEdge& e : m_MSTEdgesArray)
    {
        FVector A = { e.Vertex[0].X,  e.Vertex[0].Y, 300.0f };
        FVector B = { e.Vertex[1].X,  e.Vertex[1].Y, 300.0f };
        DrawDebugLine(GetWorld(), A, B, FColor::Cyan, false, -1.f, 0, 75.f);
    }
}

void UC_Graph::DrawDebugTriangulation() const
{
    for (const FTriangulationEdge& e : m_TriangulationEdgesArray)
    {
        FVector A = { e.Vertex[0].X,  e.Vertex[0].Y, 100.0f };
        FVector B = { e.Vertex[1].X,  e.Vertex[1].Y, 100.0f };
        DrawDebugLine(GetWorld(), A, B, FColor::Red, false, -1.f, 0, 50.f);
    }
}