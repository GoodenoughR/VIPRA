file(GLOB
  obstacles_srcs
  *.cpp
  *.hpp
  */*.cpp
  */*.hpp
)

target_sources(
  ${VIPRA_PROJECT_NAME}
  PRIVATE
  ${obstacles_srcs}
)