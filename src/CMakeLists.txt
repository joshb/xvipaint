set(SRCS
	Color.cpp
	Exception.cpp
	Image.cpp
	Painter.cpp
	PaintResponder.cpp
	PaintContext.cpp
	PngImage.cpp
	Util.cpp
)
add_library(xvipaint MODULE ${SRCS})

find_package(PNG)
target_link_libraries(
	xvipaint
	xviweb
	${PNG_LIBRARIES}
)
include_directories(
	${PNG_INCLUDE_DIR}
)
