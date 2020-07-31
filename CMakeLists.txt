cmake_minimum_required(VERSION 3.16)
project(JWinService)

set(CMAKE_CXX_STANDARD 11)

add_executable(JWinService src/main.cpp src/win_service.cpp src/win_service_ctrl.cpp src/win_service_ctrl.h src/conf_parser.cpp src/conf_parser.h src/java_process_run.cpp src/java_process_run.h)