/**************************************************************************************************
*
* \file Prototype.cpp
* \brief C++ Training - Example for the Prototype Design Pattern
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Task: Discuss the advantages and disadvantages of the given implementation of the classic
*       Prototype design pattern.
*
**************************************************************************************************/


//---- <Animal.h> ---------------------------------------------------------------------------------

#include <memory>

class Animal
{
 public:
   virtual ~Animal() = default;
   virtual void make_sound() const = 0;
   std::unique_ptr<Animal> clone() {return std::unique_ptr<Animal>(do_clone());}

   private:
   virtual Animal* do_clone() const = 0;
};


//---- <Sheep.h> ----------------------------------------------------------------------------------

//#include <Animal.h>

class Sheep : public Animal
{
 public:
   void make_sound() const override;
   std::unique_ptr<Sheep> clone() {return std::unique_ptr<Sheep>(do_clone());}

   
private:
   Sheep* do_clone() const override;
};


//---- <Sheep.cpp> --------------------------------------------------------------------------------

//#include <Sheep.h>
#include <iostream>

void Sheep::make_sound() const
{
   std::cout << " Sheep::make_sound(): baa!\n";
}

Sheep* Sheep::do_clone() const
{
   std::cout << " Sheep::clone()\n";
   return new Sheep{*this};
}


//---- <Main.cpp> ---------------------------------------------------------------------------------

//#include <Sheep.h>
#include <cstdlib>
#include <memory>

int main()
{
   std::unique_ptr<Animal> sheep( std::make_unique<Sheep>() );
   sheep->make_sound();

   std::unique_ptr<Animal> cloned_sheep( sheep->clone() );
   cloned_sheep->make_sound();

   return EXIT_SUCCESS;
}
