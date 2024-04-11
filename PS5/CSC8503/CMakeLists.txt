set(PROJECT_NAME CSC8503)

################################################################################
# Source groups
################################################################################
set(GameObjects
    "StateGameObject.h" 
    "StateGameObject.cpp"
    "NetworkPlayer.h"
    "NetworkPlayer.cpp"
    "CollectableObject.h"
    "CollectableObject.cpp"
    "AxeObject.h"
    "PickaxeObject.h"
    "BucketObject.h"
    "PlayerObject.h"
    "PlayerObject.cpp"
    "TrainObject.h"
    "TrainObject.cpp"
    "TrainCarriage.h"
    "TrainCarriage.cpp"
    "RailObject.h"
    "RailObject.cpp"
    "TreeObject.h"
    "RockObject.h"
    "PlankObject.h"
    "PlankObject.cpp"
    "StoneObject.h"
    "StoneObject.cpp"
    "AnimalObject.h"
    "AnimalObject.cpp"
    "RobotObject.h"
    "RobotObject.cpp"
)
source_group("GameObjects" FILES ${GameObjects})

set(Header_Files
#    "GameTechRenderer.h"
    "NetworkedGame.h"
    "TutorialGame.h"
    "GameTechRendererInterface.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
 #   "GameTechRenderer.cpp"
    "Main.cpp"
    "NetworkedGame.cpp"
    "TutorialGame.cpp"
    "SetupWorld.cpp"
)

source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${GameObjects}
    ${Header_Files}
    ${Source_Files}
)
################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

set(ROOT_NAMESPACE CSC8503)

################################################################################
# Compile definitions
################################################################################
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "UNICODE;"
        "_UNICODE" 
        "WIN32_LEAN_AND_MEAN"
        "_WINSOCKAPI_"   
        "_WINSOCK2API_"
        "_WINSOCK_DEPRECATED_NO_WARNINGS"
    )
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <vector>
    <map>
    <stack>
    <string>
    <list>
    <thread>
    <atomic>
    <functional>
    <iostream>
    <set>
	
    "../NCLCoreClasses/Vector.h"
    "../NCLCoreClasses/Quaternion.h"
    "../NCLCoreClasses/Plane.h"
    "../NCLCoreClasses/Matrix.h"
    "../NCLCoreClasses/GameTimer.h" 	
)

################################################################################
# Dependencies
################################################################################
include_directories("../OpenGLRendering/")
include_directories("../NCLCoreClasses/")
include_directories("../CSC8503CoreClasses/")

target_link_libraries(${PROJECT_NAME} PRIVATE ${NCLCoreClasses})