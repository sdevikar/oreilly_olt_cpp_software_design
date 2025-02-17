#==================================================================================================
#
#  CMakeLists for subchapter "Strategy" of chapter "C++ Software Design"
#
#  Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
#
#  This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
#  context of the C++ training or with explicit agreement by Klaus Iglberger.
#
#==================================================================================================

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

add_executable(FixedVector_Strategy
   FixedVector_Strategy.cpp
   )

add_executable(MultipleStrategies_1
   MultipleStrategies_1.cpp
   )

add_executable(MultipleStrategies_2
   MultipleStrategies_2.cpp
   )

add_executable(MultipleStrategies_Static
   MultipleStrategies_Static.cpp
   )

add_executable(Strategy_Classic
   Strategy_Classic.cpp
   )

add_executable(Strategy_Refactoring
   Strategy_Refactoring.cpp
   )

add_executable(Strategy_Refactoring_Static
   Strategy_Refactoring_Static.cpp
   )

add_executable(UniquePtr_Strategy
   UniquePtr_Strategy.cpp
   )

set_target_properties(
   FixedVector_Strategy
   MultipleStrategies_1
   MultipleStrategies_2
   MultipleStrategies_Static
   Strategy_Classic
   Strategy_Refactoring
   Strategy_Refactoring_Static
   UniquePtr_Strategy
   PROPERTIES
   FOLDER "C_Cpp_Software_Design/Strategy"
   )
