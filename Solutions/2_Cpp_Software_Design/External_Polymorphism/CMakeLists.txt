#==================================================================================================
#
#  CMakeLists for subchapter "External Polymorphism" of chapter "C++ Software Design"
#
#  Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
#
#  This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
#  context of the C++ training or with explicit agreement by Klaus Iglberger.
#
#==================================================================================================

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

add_executable(Animal_EP_1
   Animal_EP_1.cpp
   )

add_executable(Animal_EP_2
   Animal_EP_2.cpp
   )

add_executable(ExternalPolymorphism_1
   ExternalPolymorphism_1.cpp
   )

add_executable(ExternalPolymorphism_2
   ExternalPolymorphism_2.cpp
   )

set_target_properties(
   Animal_EP_1
   Animal_EP_2
   ExternalPolymorphism_1
   ExternalPolymorphism_2
   PROPERTIES
   FOLDER "C_Cpp_Software_Design/External_Polymorphism"
   )
