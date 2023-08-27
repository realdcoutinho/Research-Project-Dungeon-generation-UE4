# Research-Project-Dungeon-generation-UE4

##Introduction:
This research project focuses on developing a procedural dungeon generation system within the Unreal Engine 4 environment using C++. The primary objective is to create a dynamic process that generates diverse and randomized dungeons for gameplay.

## Dungeon:
The core element of dungeon creation is the **C_Dungeon** class. This class employs a **UStaticMeshComponent** named m_pStaticBox to represent rooms. During the class constructor's execution, the static mesh is initialized, taking the form of a mesh cube. However, essential attributes like location, dimensions, and visibility remain unset at this stage, leaving them for subsequent configuration.
An important consideration is that UE4 restricts the generation of static meshes during runtime. Consequently, all potential rooms must be pre-created at compile time, regardless of their eventual use. This process is executed within the **C_Generate** constructor, where dungeon instances are instantiated and subsequently added to the m_pDungeonArray.

### Dungeon Configuration:
The **SetDungeons()** function in the **C_Generate** class manages dungeon configuration. It initiates by concealing (setting _bHiddenInGame_) all static meshes within the m_DungeonArray and emptying the cells in **m_pGrid** (details on this below).
To introduce an element of randomness into the dungeon generation, a random integer within the range of 0 to 1000 is obtained from **RandRange**. This integer acts as a seed for an **FRandomStream**. Given the cell dimensions in m_pGrid, the number of rows and columns, and the minimum position, maximum x and y positions are established for each dungeon.
With the foundational parameters set, the next step is to define dungeon attributes. By employing the previously mentioned **FRandomStream** alongside the specified minimum and maximum center positions, a genuinely random center point is computed. The same process is repeated for determining the dungeon's width and depth, utilizing the designated measurement ranges.
Once the random center is calculated, it is incorporated into the m_pGrid, which identifies the corresponding cell and returns both its center location and index. At this juncture, the system examines existing dungeons. A radius is established (maximum size of width and depth plus a margin). If another dungeon already occupies the space defined by the current dungeon's center and the calculated radius, the process restarts. Conversely, if the area is unoccupied, the cell is designated as filled, and the current dungeon's attributes (center, width, and depth) are finalized. Furthermore, the dungeon's visibility is activated.



https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/927ada2c-2ec6-4a06-b244-643f1e367561

## Triangulation: 
Now, having established the static meshes for the dungeons with their visibility and parameters properly configured, the focus shifts towards the **C_Graph** component. Its initial objective revolves around the generation of a **Delaunay Triangulation** using the **Bowyer-Watson Algorithm**. Commencing this process involves the creation of a super triangle. Ensuring that the circumcircle engendered by this super triangle effectively encompasses all the center points of the dungeon locations is of paramount importance, necessitating a significantly larger size.

Subsequently, the compilation of all these dungeon center points ensues, and the **C_Graph** initiates the Triangulation procedure. Below, you will find a representation of the aforementioned algorithm in pseudo-code.

![pseudoCode](https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/4613edca-2d0d-4424-b9fe-dae556a82940)


https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/a66e5b14-517b-436d-98e1-709a5620950f

## Mininmum Spanning Tree: 
The process of triangulation, while significant, proves insufficient to meet the objectives outlined in this project. Merely establishing edges through triangulation falls short of our intent to transform each edge into a **functional** pathway linking the dungeons. Our aspiration encompasses the creation of a comprehensive approach, enabling the identification of a **minimum spanning path** interconnecting all dungeons, characterized by the absence of redundant routes. To achieve this, we turn to an additional algorithm, namely the **Minimum Spanning Tree** **(MST)** algorithm.

A **Minimum Spanning Tree** represents a subset of edges extracted from a connected, **edge-weighted graph**. This subset serves the purpose of interconnecting all vertices devoid of cycles, all the while maintaining the minimal aggregate edge weight possible. Essentially, this algorithm offers a method to determine the most **resource-efficient** means to establish connections among a given set of vertices.

However, prior to embarking on the coding phase for this algorithm, several preparatory measures are imperative. Foremost, a comprehensive inventory of edges to be employed within the **Minimum Spanning Tree** must be assembled.

![getedges](https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/9498b0cc-8e49-44ea-8b05-93eefefd3fc1)

* **Node creation**:
We begin by clearing the _nodesArray_, which is a collection to hold nodes. Then, a map called _nodesMap_ is created. This map will help organize nodes based on their positions. For each _edge_ in the _triangulationEdges_, the code checks if nodes already exist for the start and end points of the edge in the _nodesMap_. If not, new nodes are created and initialized with the corresponding locations. Each edge is then connected to the appropriate nodes by adding the edge to their _connection lists_.
_Unique nodes_ are extracted from the _nodesMap_ and added to the _nodesArray_.
Moving on, a new array called _allEdges_ is set up. For each node in the _nodesArray_, the code looks at the edges in its _connections list_. It identifies the corresponding end node by searching within the _nodesArray_. New edges are then formed using these start and end nodes, and these edges are added to the _allEdges_ array.
After that, each node's _ _connections_ list is updated to contain all the edges from allEdges. Additionally, the **Union-Find** data structure is initialized for each node. This involves setting each node as its own parent and assigning a rank of 0.
This entire process effectively establishes connections between nodes through edges, while also preparing the data structure for subsequent operations.

Having successfully completed the **Triangulation** process, including the extraction and validation of edges, as well as the creation of nodes, we are now prepared to execute the **Minimum Spanning Tree (MST)** algorithm. 
The provided pseudo-code for this operation is presented below:
![image](https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/b075b38b-722a-4bae-b775-093c815606c2)

* **Union helper function**:
The function takes two pointers to _FTriangulationNode_ objects, referred to as **rootA** and **rootB**, as parameters. These nodes represent the root elements of the sets that need to be merged. The core logic of the function revolves around comparing the _ranks_ of these two root nodes. The rank is a measure of the depth or height of a node within its tree structure. It reflects the number of nodes in the longest path from the node to a leaf.

* **FindRoot helper funtion**:
It identifies and returns the root node in a union-find data structure. The loop checks if the current node's parent isn't itself, iteratively updates the node to its parent until the root is found, and then returns the root node's pointer. 

https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/436be407-8162-43aa-9358-6abc31139628

To conclude the procedures within the **C_Graph** class, we invoke the **Path()** function. This is facilitated by utilizing the **GetAllActorsOfClass()** method to locate the **C_Grid** instance within the game world.

The **C_Grid** entity is responsible for _path calculation_ between two given locations. To execute this, an iteration is performed over the edges present in the _mstEdgesArray_. During this iteration, the starting location of each edge is identified and subsequently passed as an argument to the **AStarPath()** function within the **C_Grid** class.

Should there arise any inquiries regarding the specific data types and structures employed throughout these processes, comprehensive information can be referenced in the **DataTypes.h** file situated under the directory path **Source > DungeonGeneration > DataTypes.h**. This file encapsulates the fundamental definitions utilized in the context of this intricate procedure.


## Grid:
Before delving into the intricacies of the **AStartPath()** function that the **C_Graph** has passed to us, it is prudent to provide a brief overview of the class itself—its inception, purpose, and functionalities.

As discerned from our prior discussions, the **C_Grid** class serves two pivotal roles: the first involves the establishment of the _grid structure_, delineating the _placement of dungeons_ and meticulously managing their availability. The second pertains to the critical function of _navigating paths_ between locations, a responsibility it undertakes through interactions with **C_Graph**.

As alluded to earlier, the **Unreal Engine 4 (UE4)** framework imposes constraints on the generation of static meshes during runtime. Correspondingly, the generation of potential path and corridor blocks necessitates pre-creation during compile time, regardless of their subsequent utilization. This integral process transpires within the **CreateCell()** function, which is invoked within the **C_Grid** constructor. It's worth noting that the **C_Grid** class abstains from a conventional **TArray** of **UStaticMeshComponent** objects. Instead, it adopts a more efficient approach via the definition of an **FCell** structure. This structure holistically addresses impending challenges by encapsulating attributes such as _index_, _center_, _position_, _state_, and notably, the _static mesh component_ itself. The culmination of this endeavor results in the incorporation of the newly created **FCell** into an array named **TArray<FCell>**, which subsequently transitions us into configuring its connections through the **CreateConnections()** function.

Navigating through each **FCell** within the array involves leveraging the functions **GetColumnIndex()__ and **GetRowIndex()** to extract the _column_ and _row_ values, respectively, for the current cell under consideration. A distinct **TArray<FVector>** facilitates the representation of possible _2D directions_ in which each cell can be interconnected. Upon meticulous computations, the outcome materializes in the form of a **FGridConnection**, seamlessly added into the connections **TArray<FGridConnection>** intrinsic to the cell.

With those preliminary matters addressed, we can resume our progression from the point where we paused—specifically, at the juncture of the **AStarPath()** function. The subsequent pseudo-code is delineated below:

![image](https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/827b815d-50c4-4bef-8b97-9c1f3da32182)


![image](https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/fca8307c-366c-48b4-868c-545335318702)

Having traversed through comprehensive explanations encompassing various intricacies, navigated through challenges, and meticulously addressed pivotal points, I now take the privilege of unveiling the culminating outcome within the **Unreal Engine 4 (UE4)** environment.

https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/c91c4c8b-d76d-40c9-ab23-7a6d4c188328

https://github.com/realdcoutinho/Research-Project-Dungeon-generation-UE4/assets/95390453/65c6c4d1-9a8a-4506-aec8-f35cb3d7fe00


## Conclusion: 
This project has unfolded as a journey dedicated to crafting a **procedural dungeon generation** system within the confines of **Unreal Engine 4 (UE4)**, leveraging the power of **C++** as the driving force. Beyond the project's inherent technical challenges, it has provided me with a profound learning opportunity to enhance my skills as a programmer, particularly as a **UE4** developer.
The project's primary aim was to create an innovative and dynamic process that engenders a diverse array of _randomized_ dungeons, enriching gameplay experiences. Various critical aspects of dungeon generation have been addressed. The creation of dungeons was meticulously managed by the **C_Dungeon** class, carefully configuring room representations using **UStaticMeshComponent** elements. The generation process, while constrained by **UE4**'s restrictions on dynamic mesh generation, was efficiently handled through pre-creation during compile time. The concept of **Triangulation** was employed to establish interconnections between dungeons, laying the foundation for the subsequent **Minimum Spanning Tree (MST)** algorithm.
The incorporation of the **MST** algorithm significantly elevated the project's complexity by introducing a _minimum spanning path_ that seamlessly interconnects all dungeons, devoid of redundancies. This approach not only aligns with the project's core objective but also paves the way for intriguing future work.
One potential avenue for future exploration involves refining the Minimum Spanning Tree algorithm further. A specific aspect could be the exploration of different algorithms for the MST calculation, possibly focusing on efficient methodologies for calculating the **diameter of the MST**. The diameter of the MST carries substantial implications for the overall connectivity and traversal efficiency of the dungeon network.
This research journey has bestowed upon me a solid groundwork for mastering procedural dungeon generation within **UE4**. It serves as a testament to the latent possibilities that emerge when algorithmic creativity intertwines with the art of game development. This accomplishment beckons forth an ongoing voyage of exploration and innovation in the captivating realm of dynamic gameplay environments, fueling my personal growth as a developer

## Sources:
https://www.youtube.com/watch?v=TlLIOgWYVpI
https://www.youtube.com/watch?v=rBY2Dzej03A&t=260s
https://www.gamedeveloper.com/programming/procedural-dungeon-generation-algorithm
https://www.reddit.com/r/gamedev/comments/1dlwc4/procedural_dungeon_generation_algorithm_explained/
https://www.youtube.com/watch?v=jsmMtJpPnhU&t=1s
https://www.youtube.com/watch?v=cplfcGZmX7I
https://www.youtube.com/watch?v=GctAunEuHt4&t=1s
https://www.youtube.com/watch?v=4ySSsESzw2Y&t=241s
https://en.wikipedia.org/wiki/Bowyer–Watson_algorithm
https://www.newcastle.edu.au/__data/assets/pdf_file/0018/22482/07_An-implementation-of-Watsons-algorithm-for-computing-two-dimensional-Delaunay-triangulations.pdf
https://www.youtube.com/watch?v=-L-WgKMFuhE
https://www.youtube.com/watch?v=71CEj4gKDnE
https://mat.uab.cat/~alseda/MasterOpt/AStar-Algorithm.pdf
