

set(PRECOMPILE_TARGET "PreCompile")

add_custom_target(${PRECOMPILE_TARGET} ALL

COMMAND ${CMAKE_COMMAND} -E echo "********************************************************"
COMMAND ${CMAKE_COMMAND} -E echo "**** Precompile begin ****"

#COMMAND ${HEADER_TOOL} 

COMMAND ${CMAKE_COMMAND} -E echo "**** Precompile finished ****"
COMMAND ${CMAKE_COMMAND} -E echo "********************************************************"
)

