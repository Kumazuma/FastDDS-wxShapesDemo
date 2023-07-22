# CMakeList.txt: ShapesDemo에 대한 CMake 프로젝트, 여기에 소스를 포함하고
# 프로젝트 특정 논리를 정의합니다.
#

find_program(fastddsgen NAMES fastddsgen.bat REQUIRED)

# 이 프로젝트의 실행 파일에 소스를 추가합니다.
set(GENERATED_SOURCE_FILES "Shape.h" "Shape.cxx" "ShapePubSubTypes.h" "ShapePubSubTypes.cxx" "ShapesDemoFrameBase.h" "ShapesDemoFrameBase.cpp" "PublicationDialogBase.h" "PublicationDialogBase.cpp"
 "SubscriptionDialogBase.h" "SubscriptionDialogBase.cpp" "ReaderSampleListDialogBase.h" "ReaderSampleListDialogBase.cpp")
add_executable (ShapesDemo WIN32 ${GENERATED_SOURCE_FILES} "ShapesDemo.cpp" "ShapesDemo.h" "Model.h" "Model.cpp" "Controller.h" "Controller.cpp" "ShapesDemoFrame.h" "ShapesDemoFrame.cpp" "CanvasPanel.h" "CanvasPanel.cpp" "resource.rc" "Subscriber.h" "Subscriber.cpp" "PublicationDialog.h" "PublicationDialog.cpp" "SubscriptionDialog.h" "SubscriptionDialog.cpp" "ReaderSampleListDialog.h" "ReaderSampleListDialog.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShapesDemo PROPERTY CXX_STANDARD 20)
endif()

include(${WX_CMAKE})

find_package(fastrtps REQUIRED)
find_package(fastcdr REQUIRED)

target_link_libraries(ShapesDemo PRIVATE wx::base wx::core wx::aui fastrtps fastcdr)
target_compile_definitions(ShapesDemo PRIVATE wxUSE_NO_MANIFEST)
set_target_properties(ShapesDemo PROPERTIES VS_DPI_AWARE "PerMonitor")

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
	target_compile_options(ShapesDemo PRIVATE "$<$<CONFIG:Release>:/Zi>")
	target_link_options(ShapesDemo PRIVATE "$<$<CONFIG:Release>:/DEBUG>")
	target_link_options(ShapesDemo PRIVATE "$<$<CONFIG:Release>:/OPT:REF>")
	target_link_options(ShapesDemo PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>")
endif()
add_custom_command(TARGET ShapesDemo PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/resource/ $<TARGET_FILE_DIR:ShapesDemo>)

# TODO: 필요한 경우 테스트를 추가하고 대상을 설치합니다.
