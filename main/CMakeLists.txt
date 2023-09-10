idf_component_register(SRCS "src/main.cpp" "src/hwInit.c" INCLUDE_DIRS "inc/" REQUIRES slint)
slint_target_sources(${COMPONENT_LIB} ui/appwindow.slint)
