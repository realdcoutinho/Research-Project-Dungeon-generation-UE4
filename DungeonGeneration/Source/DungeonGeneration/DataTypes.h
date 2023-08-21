// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <queue>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include "Math/Vector.h"
#include "DrawDebugHelpers.h"

#include "DataTypes.generated.h"



// Upgraded Edge struct
struct FTriangulationEdge : public FEdge
{
    float _cost; //leght of the FTriangulationEdge

    FTriangulationNode* _startNode; // Pointer to the start node
    FTriangulationNode* _endNode;   // Pointer to the end node

    FTriangulationEdge() {};

    FTriangulationEdge(FVector V1, FVector V2) //constructor given two points
        :FEdge(V1, V2)
    {
        _cost = CalculateCost(Vertex[0], Vertex[1]);
    }

    FTriangulationEdge(FVector V1, FVector V2, FTriangulationNode* StartNode, FTriangulationNode* EndNode) //contructor with two center points and nodes
        : FEdge(V1, V2), _startNode(StartNode), _endNode(EndNode)
    {
        _cost = CalculateCost(Vertex[0], Vertex[1]);
    }

    float CalculateCost(const FVector V1, const FVector V2)
    {
        return FVector::Distance(V1, V2);
    }
};

/// <summary>
/// TRIANGLE STRUCT
/// </summary>
USTRUCT(BlueprintType)
struct FTriangle
{
    GENERATED_BODY()

    TArray<FTriangulationEdge> _edgesArray; //edges of Triangle
    TArray<FVector> _vertices; //each point of triangle (center point of a particular cell)

    float _circumRadius; //the radius of a circumpherence that emcompases the three points of the triangle
    FVector _circumCenter; //the center of said circumference

    FTriangle() {};

    FTriangle(FVector V1, FVector V2, FVector V3) //triangle constructor
    {
        _vertices.Add(V1); //first point will be added

        bool isCounterClockwise = IsCounterClockwise(V1, V2, V3); //figure if triangle is being added clock or counter clockwise
        //set in the respective order
        FVector vector2 = isCounterClockwise ? V2 : V3;
        FVector vector3 = isCounterClockwise ? V3 : V2;
        // add to vertices in the respective order
        _vertices.Add(vector2); 
        _vertices.Add(vector3);

        //given the vertices, create the edges between each triangle
        _edgesArray.Add(FTriangulationEdge(_vertices[0], _vertices[1]));
        _edgesArray.Add(FTriangulationEdge(_vertices[1], _vertices[2]));
        _edgesArray.Add(FTriangulationEdge(_vertices[2], _vertices[0]));

        //calculations
        _circumCenter = CalculateCircumCenter();
        _circumRadius = CalculateRadius();
    }

    //calculations
    bool IsCounterClockwise(const FVector& pointA, const FVector& pointB, const FVector& pointC)
    {
        float result = (pointB.X - pointA.X) * (pointC.Y - pointA.Y) - (pointC.X - pointA.X) * (pointB.Y - pointA.Y);
        return result > 0;
    }

    //calculate circumcenter
    FVector CalculateCircumCenter()
    {
        FVector pA = _vertices[0];
        FVector pB = _vertices[1];
        FVector pC = _vertices[2];

        // Calculate intermediate values
        float Dx = (pA.X * pA.X + pA.Y * pA.Y) * (pB.Y - pC.Y) + (pB.X * pB.X + pB.Y * pB.Y) * (pC.Y - pA.Y) + (pC.X * pC.X + pC.Y * pC.Y) * (pA.Y - pB.Y);
        float Dy = (pA.X * pA.X + pA.Y * pA.Y) * (pC.X - pB.X) + (pB.X * pB.X + pB.Y * pB.Y) * (pA.X - pC.X) + (pC.X * pC.X + pC.Y * pC.Y) * (pB.X - pA.X);
        float D = 2 * ((pA.X - pB.X) * (pB.Y - pC.Y) - (pB.X - pC.X) * (pA.Y - pB.Y));

        // Calculate circumcenter coordinates
        float x = Dx / D;
        float y = Dy / D;

        return FVector(x, y, 0);
    }

    //calculate radius
    float CalculateRadius()
    {
        return FVector::Distance(_vertices[0], _circumCenter);
    }

    //bool ContainsEdge(const FTriangulationEdge& edge)
    //{
    //    for (const FVector& vertex : _vertices)
    //    {
    //        if (vertex == edge.Vertex[0] || vertex == edge.Vertex[1])
    //        {
    //            return true;
    //        }
    //    }
    //    return false;
    //}

    bool operator==(const FTriangle& Other) const
    {
        // Check if both triangles have the same number of vertices
        if (_vertices.Num() != Other._vertices.Num())
            return false;

        // Check if all vertices of the triangles match
        int numOfEqualVertices{ 0 };
        for (int32 i = 0; i < _vertices.Num(); ++i)
        {
            for (int32 j = 0; j < Other._vertices.Num(); ++j)
            {
                if (_vertices[i] != Other._vertices[j])
                    ++numOfEqualVertices;
            }
        }
        if (numOfEqualVertices == 3)
        {
            return true;
        }

        // Check if both triangles have the same circumCenter and circumRadius
        if (_circumCenter != Other._circumCenter || _circumRadius != Other._circumRadius)
            return false;

        // If all checks pass, the triangles are equal
        return true;
    }

};



USTRUCT(BlueprintType)
struct FTriangulationNode
{
    GENERATED_BODY()

    FVector _location;
    TArray<FTriangulationEdge> _connections; //list of connections to other FTriangulationNodes

    FTriangulationNode* _parent; // Parent node in the Union-Find data structure
    int32 _rank;            // Rank for Union-Find optimization

    FTriangulationNode()
        : _parent(this), _rank(0)
    {
    }

    void AddConnection(FTriangulationEdge& edge)
    {
        _connections.AddUnique(edge);
    }

    void AddConnection(const TArray<FTriangulationEdge>& edges)
    {
        for(const FTriangulationEdge& edge : edges)
        {
            _connections.AddUnique(edge);
        }
    }

    void AddLocation(FVector location)
    {
        _location = location;
    }

    bool operator==(const FTriangulationNode& Other) const
    {
        return (Other._location == _location);
    }
};













//
//
//
///// <summary>
///// TRIANGLE GRAPH
///// </summary>
//USTRUCT(BlueprintType)
//struct FTriangulationGraph
//{
//    GENERATED_BODY()
//
//    TArray<FTriangle> _triangles;
//    TArray<FVector> _points;
//    int numPoints;
//    TArray<FTriangulationEdge> _totalEdges;
//    TArray<FTriangulationEdge> _mstEdges;
//    TArray<FTriangulationNode> _nodes;
//    TMap<FVector, FTriangulationNode> _nodesMap;
//    TArray<FTriangulationEdge> _longestPath;
//
//
//
//
//    void SetPointsArray(TArray<FVector>& points)
//    {
//        DeletePoints();
//        _points = points;
//    }
//
//    void AddPoint(FVector& point)
//    {
//        _points.Add(point);
//    }
//
//    void DeletePoints()
//    {
//        _points.Empty();
//    }
//
//    bool isPointInsideCircumcircle(const FVector& p, const FTriangle& t)
//    {
//        //float distance = std::sqrtf((p.X - t._circumCenter.X) * (p.X - t._circumCenter.X) + (p.Y - t._circumCenter.Y) * (p.Y - t._circumCenter.Y));
//
//        float distance = FVector::DistSquaredXY(p, t._circumCenter);
//        return distance < (t._circumRadius * t._circumRadius);
//    }
//
//    bool sharesEdge(const FTriangle& t, const FEdge& e)
//    {
//        // Check if the triangle shares the specified edge.
//        // Return true if the triangle shares the edge; otherwise, false.
//        for (int i{ 0 }; i < t._edgesArray.Num(); ++i)
//        {
//            if (t._edgesArray[i] == e)
//                return true;
//        }
//        return false;
//    }
//
//    bool hasCommonVertex(const FTriangle& t1, const FTriangle& t2)
//    {
//        for (int i{ 0 }; i < t1._vertices.Num(); ++i)
//        {
//            if ((t1._vertices[i] == t2._vertices[0]) || (t1._vertices[i] == t2._vertices[1]) || (t1._vertices[i] == t2._vertices[2]))
//                return true;
//        }
//        return false;
//    }
//
//
//    FTriangle _currentTriangle;
//    FTriangle _superTriangle;
//    TArray<FTriangle> _triangulation;
//    TArray<FTriangle> _tri;
//    void CreateSuperTriangle(int32 increment, int32 numRooms, int32 margin)
//    {
//        float i = static_cast<float>(increment);
//        float n = static_cast<float>(numRooms);
//        float m = static_cast<float>(margin);
//        FVector v0 = { -1 * (n * i + m), -1 * (n * i + m), 0 };
//        FVector v1 = { (n * i + m), 0, 0 };
//        FVector v2 = { -1 * (n * i + m), 1 * (n * i + m), 0 };
//
//        _currentTriangle = FTriangle(v0, v1, v2);
//        _superTriangle = FTriangle(v0 * 100, v1 * 100, v2 * 100);
//    }
//
//    void TriangulationNotOptimzed()
//    {
//        TArray<FTriangle> triangulation;
//        triangulation.Add(_superTriangle);
//
//        for (FVector& point : _points)
//        {
//            TArray<FTriangle> badTriangles;
//
//            // Find all the triangles that are no longer valid due to the insertion
//            for (FTriangle& triangle : triangulation)
//            {
//                if (isPointInsideCircumcircle(point, triangle))
//                {
//                    badTriangles.Add(triangle);
//                }
//            }
//
//            TArray<FTriangulationEdge> polygon;
//
//            // Find the boundary of the polygonal hole
//            for (FTriangle& triangle : badTriangles)
//            {
//                for (FTriangulationEdge& edge : triangle._edgesArray)
//                {
//                    bool shared = false;
//                    for (FTriangle& otherTriangle : badTriangles)
//                    {
//                        if (&triangle != &otherTriangle) // Skip the current triangle
//                        {
//                            if (sharesEdge(otherTriangle, edge))
//                            {
//                                shared = true;
//                                break;
//                            }
//                        }
//                    }
//
//                    if (!shared)
//                    {
//                        polygon.Add(edge);
//                    }
//                }
//            }
//
//            // Remove bad triangles from the data structure
//            for (FTriangle& triangle : badTriangles)
//            {
//                triangulation.Remove(triangle);
//            }
//
//            // Re-triangulate the polygonal hole
//            for (FTriangulationEdge& edge : polygon)
//            {
//                FTriangle newTri(edge.Vertex[0], edge.Vertex[1], point);
//                triangulation.Add(newTri);
//            }
//        }
//
//        // Clean up by removing triangles containing a vertex from the original super-triangle
//
//
//
//        TArray<FTriangle> trianglesToRemove;
//
//        //for (int m{ 0 }; m < triangulation.Num(); ++m)
//        //{
//        //    FTriangle currentTriangle = triangulation[m];
//        //    for (int n{ 0 }; n < _superTriangle._vertices.Num(); ++n)
//        //    {
//        //        if (hasCommonVertex(currentTriangle, _superTriangle))
//        //        {
//        //            --m;
//        //            triangulation.Remove(currentTriangle);
//        //            break;
//        //        }
//        //    }
//        //}
//
//        for (FTriangle& triangle : triangulation)
//        {
//            for (FVector& vertex : triangle._vertices)
//            {
//                // Assuming the super-triangle vertices are stored in an array named "superTriangleVertices"
//                if (hasCommonVertex(triangle, _superTriangle))
//                {
//                    trianglesToRemove.Add(triangle);
//                    break;
//                }
//            }
//        }
//
//        for (FTriangle& triangle : trianglesToRemove)
//        {
//            triangulation.Remove(triangle);
//        }
//
//        _tri = triangulation;
//    }
//
//    FTriangulationNode GetConnectionNode(FTriangulationEdge& connection, FVector& local)
//    {
//        FVector pointA = connection.Vertex[0];
//        FVector pointB = connection.Vertex[1];
//
//        FTriangulationNode node;
//        if (pointA != local)
//        {
//            node = _nodesMap[pointA];
//        }
//        if (pointB != local)
//        {
//            node = _nodesMap[pointB];
//        }
//        return node;
//    }
//
//    TArray<FTriangulationEdge> FindMinimumSpanningTreeNotOptmized(TArray<FTriangulationNode>& nodes)
//    {
//        TArray<FTriangulationNode> unVisitedNodes;
//        TArray<FTriangulationNode> visitedNodes;
//
//        TArray<FTriangulationEdge> _allEdges;
//
//        for (FTriangulationNode& node : nodes)
//        {
//            unVisitedNodes.AddUnique(node);
//        }
//        visitedNodes.AddUnique(unVisitedNodes[0]);
//        unVisitedNodes.RemoveAt(0);
//
//        while (unVisitedNodes.Num() > 0)
//        {
//            TMap<int32, FTriangulationNode> _mapNodes;
//            TMap<int32, FTriangulationEdge> _mapEdges;
//            for (FTriangulationNode& visited : visitedNodes)
//            {
//
//                for (FTriangulationEdge edge : visited._connections)
//                {
//                    FTriangulationNode node = GetConnectionNode(edge, visited._location);
//                    for (FTriangulationNode& unVisited : unVisitedNodes)
//                    {
//                        if (unVisited._location == node._location)
//                        {
//                            _mapNodes.Add(edge._cost, node);
//                            _mapEdges.Add(edge._cost, edge);
//                        }
//                    }
//                }
//            }
//
//            float cost = BIG_NUMBER;
//            FTriangulationEdge newEdge;
//            FTriangulationNode newNode;
//            for (const auto& Pair : _mapNodes)
//            {
//                if (cost > Pair.Key)
//                {
//                    cost = Pair.Key;
//                    newNode = Pair.Value;
//                }
//            }
//
//            newEdge = _mapEdges[cost];
//
//
//            visitedNodes.AddUnique(newNode);
//            //for (FUpgradedNode& unvisited : unVisitedNodes)
//            //{
//            //    if (unvisited._location == newNode._location)
//            //    {
//            //        unVisitedNodes.Remove(unvisited);
//            //    }
//            //}
//
//            unVisitedNodes.RemoveAllSwap([&](const FTriangulationNode& unvisited) {
//                return unvisited._location == newNode._location;
//                });
//
//            _allEdges.AddUnique(newEdge);
//            if (unVisitedNodes.Num() == 0)
//            {
//                break;
//            }
//
//        }
//        return _allEdges;
//    }
//
//
//    // Helper function to find the root of a node in the Union-Find data structure.
//    FTriangulationNode* FindRoot(FTriangulationNode* node)
//    {
//        while (node->_parent != node)
//        {
//            node = node->_parent;
//        }
//        return node;
//    }
//
//    // Helper function to perform union operation in the Union-Find data structure.
//    void Union(FTriangulationNode* rootA, FTriangulationNode* rootB)
//    {
//        //if (rootA != rootB)
//        //{
//            if (rootA->_rank < rootB->_rank)
//            {
//                rootA->_parent = rootB;
//            }
//            else if (rootA->_rank > rootB->_rank)
//            {
//                rootB->_parent = rootA;
//            }
//            else
//            {
//                rootB->_parent = rootA;
//                rootA->_rank++;
//            }
//        //}
//    }
//
//
////TArray<FUpgradedEdge> DfsFindLongestPath(FUpgradedNode* node, TSet<FUpgradedNode*>& visited, TMap<FUpgradedNode*, TArray<FUpgradedEdge>>& mstGraph)
////{
////    visited.Add(node);
////    TArray<FUpgradedEdge> longestPath;
////
////    for (const FUpgradedEdge& edge : mstGraph[node])
////    {
////        FUpgradedNode* neighbor = (node == edge._startNode) ? edge._endNode : edge._startNode;
////        if (!visited.Contains(neighbor))
////        {
////            TArray<FUpgradedEdge> path = DfsFindLongestPath(neighbor, visited, mstGraph);
////            if (path.Num() > longestPath.Num())
////                longestPath = path;
////        }
////    }
////
////    if (longestPath.Num() > 0)
////        longestPath.Insert(edge, 0);
////
////    return longestPath;
////}
////
////// Function to find the longest simple path in the MST.
////TArray<FUpgradedEdge> FindLongestSimplePathInMST(const TArray<FUpgradedEdge>& minimumSpanningTree, const TArray<FUpgradedNode>& nodes)
////{
////    // Build the MST as an adjacency list for efficient traversal.
////    TMap<FUpgradedNode*, TArray<FUpgradedEdge>> mstGraph;
////    for (const FUpgradedEdge& edge : minimumSpanningTree)
////    {
////        mstGraph[edge._startNode].Add(edge);
////        mstGraph[edge._endNode].Add(edge);
////    }
////
////    // Find the longest simple path starting from each node in the MST and keep the longest one.
////    TArray<FUpgradedEdge> longestPath;
////    for (const FUpgradedNode& node : nodes)
////    {
////        TSet<FUpgradedNode*> visited;
////        TArray<FUpgradedEdge> path = DfsFindLongestPath(const_cast<FUpgradedNode*>(&node), visited, mstGraph);
////        if (path.Num() > longestPath.Num())
////            longestPath = path;
////    }
////
////    return longestPath;
////}
//
//
//
//    TArray<FTriangulationEdge> FindMinimumSpanningTree(TArray<FTriangulationNode>& nodes)
//    {
//
//        // Create a copy of the edges in the graph.
//        TArray<FTriangulationEdge> allEdges;
//        for (FTriangulationNode& node : nodes)
//        {
//            TArray<FTriangulationEdge> newSet;
//            for (FTriangulationEdge& edge : node._connections)
//            {
//                FTriangulationNode* startNode = &node;
//                FTriangulationNode* endNode = nullptr;
//
//                // Find the corresponding node for the end vertex of the edge.
//                for (FTriangulationNode& otherNode : nodes)
//                {
//                    if (otherNode._location == edge.Vertex[1])
//                    {
//                        endNode = &otherNode;
//                        break;
//                    }
//                }
//
//                if (endNode)
//                {
//                    FTriangulationEdge newEdge = FTriangulationEdge(edge.Vertex[0], edge.Vertex[1], startNode, endNode);
//                    allEdges.Add(newEdge);
//                    newSet.AddUnique(newEdge);
//                }
//            }
//
//            node._connections = newSet;
//
//            // Initialize the Union-Find data structure.
//            node._parent = &node;
//            node._rank = 0;
//
//        }
//
//        // Sort the edges based on their cost in non-decreasing order.
//        // further explain here
//        allEdges.Sort([](const FTriangulationEdge& EdgeA, const FTriangulationEdge& EdgeB) { return EdgeA._cost < EdgeB._cost; });
//
//        TArray<FTriangulationEdge> minimumSpanningTree;
//        
//
//        //// Initialize the Union-Find data structure.
//        //for (FUpgradedNode& node : nodes)
//        //{
//        //    node._parent = &node;
//        //    node._rank = 0;
//        //}
//
//        int edgeCount = 0;
//        int numNodes = nodes.Num();
//        for (FTriangulationEdge& edge : allEdges)
//        {
//            FTriangulationNode* rootA = FindRoot(edge._startNode);
//            FTriangulationNode* rootB = FindRoot(edge._endNode);
//
//            // Check if including this edge will create a cycle.
//            if (rootA != rootB)
//            {
//                minimumSpanningTree.Add(edge);
//                Union(rootA, rootB);
//                edgeCount++;
//                if (edgeCount == numNodes - 1)
//                {
//                    break; // Minimum spanning tree found.
//                }
//            }
//        }
//
//
//        //TArray<FUpgradedEdge> longestPath = FindLongestSimplePathInMST(minimumSpanningTree, nodes);
//
//
//
//        _mstEdges = minimumSpanningTree;
//        //TArray<FUpgradedNode> longestPath = FindLongestPath(nodes);
//
//        return minimumSpanningTree;
//
//    }
//
//
//    void TriangulationAlgorithm()
//    {
//        // Create an empty triangle graph data structure
//        TArray<FTriangle> triangulation;
//        triangulation.Add(_superTriangle);
//
//
//        // Add a super-triangle to the triangulation (large enough to contain all points)
//        // You need to define a large enough triangle here, depending on your use case.
//        // For simplicity, we will skip adding the super-triangle in this example.
//
//        // Add all the points one by one to the triangulation
//        for (FVector& point : _points)
//        {
//            TArray<FTriangle> badTriangles;
//
//            // Find all the triangles that are no longer valid due to the insertion
//            for (FTriangle& triangle : triangulation)
//            {
//                if (isPointInsideCircumcircle(point, triangle))
//                {
//                    badTriangles.Add(triangle);
//                }
//            }
//
//            TArray<FTriangulationEdge> polygon;
//
//            for (const FTriangle& triangle : badTriangles)
//            {
//                for (const FTriangulationEdge& edge : triangle._edgesArray)
//                {
//                    bool shared = std::any_of(badTriangles.begin(), badTriangles.end(), [&](const FTriangle& otherTriangle) 
//                        {
//                            return &triangle != &otherTriangle && sharesEdge(otherTriangle, edge);
//                        });
//
//                    if (!shared)
//                    {
//                        polygon.Add(edge);
//                    }
//                }
//            }
//
//            // Remove bad triangles from the data structure
//            for (FTriangle& triangle : badTriangles)
//            {
//                triangulation.Remove(triangle);
//            }
//
//            // Re-triangulate the polygonal hole
//            for (FTriangulationEdge& edge : polygon)
//            {
//                FTriangle newTri(edge.Vertex[0], edge.Vertex[1], point);
//                triangulation.Add(newTri);
//            }
//        }
//
//        triangulation.RemoveAllSwap([&](const FTriangle& triangle) {
//            for (const FVector& vertex : triangle._vertices) {
//                if (hasCommonVertex(triangle, _superTriangle)) {
//                    return true;
//                }
//            }
//            return false;
//        });
//
//        _tri = triangulation;
//
//        GetEdges();
//        CreateNodes();
//        _mstEdges = FindMinimumSpanningTree(_nodes);
//
//    }
//
//    void GetEdges()
//    {
//        TArray<FTriangulationEdge> edges;
//
//
//        for (const FTriangle& triangle : _tri)
//        {
//            for (const FTriangulationEdge& edge : triangle._edgesArray)
//            {
//                edges.AddUnique(edge);
//            }
//        }
//
//        _totalEdges = edges;
//    }
//
//    void CreateNodes()
//    {
//        _nodes.Empty();
//        _nodesMap.Empty();
//        // Step 1: Given an array of edges (assuming it's named "edges")
//        TArray<FTriangulationEdge> edges = _totalEdges; // Your array of edges
//
//        // Step 2: Create a map to store nodes based on their locations
//        TMap<FVector, FTriangulationNode> nodesMap;
//
//        for (FTriangulationEdge& edge : edges)
//        {
//            // Step 3: Create or find the nodes corresponding to the edge's start and end points
//            FTriangulationNode* node1 = _nodesMap.Find(edge.Vertex[0]);
//            FTriangulationNode* node2 = _nodesMap.Find(edge.Vertex[1]);
//
//            if (!_nodesMap.Find(edge.Vertex[0]))
//            {
//                FTriangulationNode newNode;
//                FVector loc = edge.Vertex[0];
//                newNode.AddLocation(loc);
//                _nodesMap.Add(edge.Vertex[0], newNode);
//                node1 = &_nodesMap[edge.Vertex[0]];
//            }
//
//            if (!_nodesMap.Find(edge.Vertex[1]))
//            {
//                FTriangulationNode newNode;
//                FVector loc = edge.Vertex[1];
//                newNode.AddLocation(loc);
//                _nodesMap.Add(edge.Vertex[1], newNode);
//                node2 = &_nodesMap[edge.Vertex[1]];
//            }
//
//            //// Step 4: Add the edge to the connection list of both nodes
//            node1->AddConnection(edge);
//            node2->AddConnection(edge);
//        }
//
//        // Step 5: Extract the unique nodes from the map into an array
//        /*TArray<FUpgradedNode> nodes;*/
//        for (const auto& pair : _nodesMap)
//        {
//            _nodes.AddUnique(pair.Value);
//        }
//        //nodes;
//        _nodes;
//      }
//
//
//
//    /*TArray<FUpgradedNode> GetConnectingNodes(FUpgradedNode& node)
//    {
//
//        TArray<FVector> locations;
//
//        for (FUpgradedEdge& edges : node._connections)
//        {
//            if (edges.Vertex[0] != node._location)
//            {
//                locations.AddUnique(edges.Vertex[0]);
//            }
//            if (edges.Vertex[1] != node._location)
//            {
//                locations.AddUnique(edges.Vertex[0]);
//            }
//        }
//
//        TArray<FUpgradedNode> nodes;
//        for (FVector& loc : locations)
//        {
//            FUpgradedNode* node = _nodesMap.Find(loc);
//
//            if (node)
//            {
//                nodes.AddUnique(*node);
//            }
//        }
//        return nodes;
//    }*/
//};
//
//
//
//
//





///// <summary>
///// ROOM STRUCT
///// </summary>
//USTRUCT(BlueprintType)
//struct FRoom
//{
//    GENERATED_BODY()
//
//        UPROPERTY(EditAnywhere, BlueprintReadWrite)
//        int32 _x;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//        int32 _y;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//        FVector _center;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//
//        UStaticMeshComponent* _pStaticBox;
//
//    void SetVariables(const FVector center, const int32 x, const int32 y)
//    {
//        _center = center;
//        _x = x;
//        _y = y;
//    }
//
//    void SetBounds(UStaticMeshComponent* mesh)
//    {
//        _pStaticBox = mesh;
//        _pStaticBox->SetRelativeLocation(_center);
//        FVector newScale = FVector(_x, _y, 100.0f); // Adjust the scale factors as needed.
//        _pStaticBox->SetRelativeScale3D(newScale / 100);
//    }
//
//    //DEBUG PURPOSES
//    FVector m_TopLeft;
//    FVector m_TopRight;
//    FVector m_BottomLeft;
//    FVector m_BottomRight;
//    void SetCoordinates()
//    {
//        m_TopLeft = { _center.X - _x / 2, _center.Y + _y / 2, _center.Z };
//        m_TopRight = { _center.X + _x / 2, _center.Y + _y / 2, _center.Z };
//        m_BottomLeft = { _center.X - _x / 2, _center.Y - _y / 2, _center.Z };
//        m_BottomRight = { _center.X + _x / 2, _center.Y - _y / 2, _center.Z };
//    }
//
//};