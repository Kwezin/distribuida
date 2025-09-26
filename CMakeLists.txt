cmake_minimum_required(VERSION 3.10)
project(grpc_lab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

find_program(PROTOC_EXECUTABLE protoc)
if(NOT PROTOC_EXECUTABLE)
  message(FATAL_ERROR "protoc not found")
endif()

find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
if(NOT GRPC_CPP_PLUGIN)
  message(FATAL_ERROR "grpc_cpp_plugin not found")
endif()

set(PROTO_SRC_FILES ${CMAKE_CURRENT_BINARY_DIR}/notas.pb.cc)
set(PROTO_HDR_FILES ${CMAKE_CURRENT_BINARY_DIR}/notas.pb.h)
set(GRPC_SRC_FILES ${CMAKE_CURRENT_BINARY_DIR}/notas.grpc.pb.cc)
set(GRPC_HDR_FILES ${CMAKE_CURRENT_BINARY_DIR}/notas.grpc.pb.h)

add_custom_command(
  OUTPUT ${PROTO_SRC_FILES} ${PROTO_HDR_FILES} ${GRPC_SRC_FILES} ${GRPC_HDR_FILES}
  COMMAND ${PROTOC_EXECUTABLE}
    --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/notas.proto
  COMMAND ${PROTOC_EXECUTABLE}
    --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
    --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/notas.proto
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/notas.proto
  COMMENT "Gerando código a partir de notas.proto"
)

add_custom_target(generate_grpc_files ALL DEPENDS
  ${PROTO_SRC_FILES} ${PROTO_HDR_FILES} ${GRPC_SRC_FILES} ${GRPC_HDR_FILES}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(servidor_cpp servidor.cpp ${PROTO_SRC_FILES} ${GRPC_SRC_FILES})
target_link_libraries(servidor_cpp PRIVATE protobuf::libprotobuf gRPC::grpc++)

add_executable(cliente_cpp cliente.cpp ${PROTO_SRC_FILES} ${GRPC_SRC_FILES})
target_link_libraries(cliente_cpp PRIVATE protobuf::libprotobuf gRPC::grpc++)
