set(PROJECT_NAME CSC8503CoreClasses)

################################################################################
# Source groups
################################################################################

set(Collision_Detection
	"AABBVolume.h"
	"CapsuleVolume.h"    
	"CollisionDetection.h"
	"CollisionVolume.h"
	"OBBVolume.h"
	"SphereVolume.h"
	"CapsuleVolume.cpp"   
	"CollisionDetection.cpp"
        "OcTree.h"
)
source_group("Collision Detection" FILES ${Collision_Detection})

set(Physics
	"Constraint.h"  
	"OrientationConstraint.h" 
	"PhysicsObject.h"
	"PhysicsSystem.h"
	"PositionConstraint.h" 
	"Ray.h"
	"State.h"
	"OrientationConstraint.cpp" 
	"PhysicsObject.cpp"
	"PhysicsSystem.cpp"
	"PositionConstraint.cpp" 
)
source_group("Physics" FILES ${Physics})

set(Networking
	"GameClient.h"   
	"GameServer.h"
	"GameClient.cpp"   
	"GameServer.cpp"
)
source_group("Networking" FILES ${Networking})

set(AI_Behaviour_Tree
    "BehaviourAction.h"
    "BehaviourNode.h"
    "BehaviourNodeWithChildren.h"
    "BehaviourSelector.h"
    "BehaviourSelector.cpp"
    "BehaviourSequence.h"
    "BehaviourSequence.cpp"
)
source_group("AI\\Behaviour Trees" FILES ${AI_Behaviour_Tree})

set(AI_Pushdown_Automata
    "PushdownMachine.h"
    "PushdownMachine.cpp"
    "PushdownState.h"
    "PushdownState.cpp"
)
source_group("AI\\Pushdown Automata" FILES ${AI_Pushdown_Automata})

set(AI_State_Machine
    "State.h"
    "StateMachine.h"  
    "StateMachine.cpp"
    "StateMachine.h"
    "StateTransition.h"
)
source_group("AI\\State Machine" FILES ${AI_State_Machine})

set(AI_Pathfinding
    "NavigationGrid.h"
    "NavigationGrid.cpp"  
    "NavigationMesh.cpp"
    "NavigationMesh.h"
    "NavigationMap.h"
    "NavigationPath.h"
)
source_group("AI\\Pathfinding" FILES ${AI_Pathfinding})

set(Header_Files
	"Debug.h"
	"GameObject.h"
	"GameWorld.h"
	"RenderObject.h"
	"Transform.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
	"Debug.cpp"
	"GameObject.cpp"
	"GameWorld.cpp"
	"RenderObject.cpp"
	"Transform.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
	${Collision_Detection}
	${Physics}
	${Networking}
    ${AI_Behaviour_Tree}
    ${AI_Pushdown_Automata}
    ${AI_State_Machine}
    ${AI_Pathfinding}
)

set_source_files_properties(${ALL_FILES} PROPERTIES LANGUAGE CXX)


################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
#use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE CSC8503CoreClasses)

target_precompile_headers(${PROJECT_NAME} PRIVATE    
	<fstream>
    <sstream>
	<iostream>
	<iosfwd>
	
    <memory>
	<string>
	<thread>
    <filesystem>
    <functional>
	<assert.h>  
	<chrono>	
  
    <unordered_set>
	<set>
    <vector>
	<unordered_map>
    <map> 
	<stack> 

	
	"../NCLCoreClasses/Vector.h"
    "../NCLCoreClasses/Quaternion.h"
    "../NCLCoreClasses/Plane.h"
    "../NCLCoreClasses/Matrix.h"
)

include_directories("../NCLCoreClasses/")
include_directories("./")