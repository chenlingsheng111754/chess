set(ProjectName "Chess")
file(GLOB SrcFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB BusinessFiles ${CMAKE_CURRENT_SOURCE_DIR}/business/*.h ${CMAKE_CURRENT_SOURCE_DIR}/business/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/business/ChessControlCenter/*.h ${CMAKE_CURRENT_SOURCE_DIR}/business/ChessControlCenter/*.cpp)

add_executable(${ProjectName} ${SrcFiles} ${BusinessFiles})

#添加文件路径
include_directories(${CMAKE_SOURCE_DIR}/depend)
include_directories(${CMAKE_SOURCE_DIR}/depend/theron/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/business)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/business/ChessControlCenter)

#添加库	
#方法1
# set(ProjectLibs ${CMAKE_SOURCE_DIR}/depend/theron/lib/Theron.lib)
# target_link_libraries(${ProjectName} ${ProjectLibs})
#方法2
#target_link_libraries(${ProjectName} ${CMAKE_SOURCE_DIR}/depend/theron/lib/Theron.lib)
#方法3
# target_link_libraries(${ProjectName}  
					  # ${CMAKE_SOURCE_DIR}/depend/theron/lib/Theron.lib
					  # ${CMAKE_SOURCE_DIR}/depend/theron/lib/Therond.lib
					# )

target_link_libraries(${ProjectName} ${CMAKE_SOURCE_DIR}/depend/theron/lib/Theron.lib)
target_link_libraries(${ProjectName} ${CMAKE_SOURCE_DIR}/depend/theron/lib/Therond.lib)
