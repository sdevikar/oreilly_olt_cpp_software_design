/**************************************************************************************************
*
* \file Car_Bridge.cpp
* \brief C++ Training - Example for the Bridge Design Pattern
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Task: The fictional electric car manufacturer 'ElectricHero (EH)' has used the Bridge design
*       for its 'ElectricCar' implementation. What would have to be changed to implement it
*       in terms of the Strategy design pattern?
*
**************************************************************************************************/


//---- <ElectricCarImpl.h> ----------------------------------------------------------------------------

namespace eh {

class ElectricCarImpl
{
 public:
   virtual ~ElectricCarImpl() = default;

   virtual void start() = 0;
   virtual void stop() = 0;

   virtual void drawPower() = 0;
   virtual void charge() = 0;

   // ... more car-specific functions

 private:
   // ... potentially some car-specific data members (wheels, drivetrain, ...)
};

} // namespace eh


//---- <ElectricCar.h> ----------------------------------------------------------------------------

//#include <ElectricCarImpl.h>
#include <memory>
#include <utility>

namespace eh {

class ElectricCar
{
 protected:
   explicit ElectricCar( std::unique_ptr<ElectricCarImpl> impl )
      : pimpl_{ std::move(impl) }
   {}

 public:
   virtual ~ElectricCar() = default;
   virtual void drive() = 0;
   // ... more car-specific functions

 protected:
   ElectricCarImpl*       getImpl()       { return pimpl_.get(); }
   ElectricCarImpl const* getImpl() const { return pimpl_.get(); }

 private:
   std::unique_ptr<ElectricCarImpl> pimpl_;  // Pointer-to-IMPLementation (pimpl)
};

} // namespace eh


//---- <BatteryGen1.h> ----------------------------------------------------------------------------

namespace eh {

class BatteryGen1
{
 public:
   BatteryGen1( double charge );

   void drawPower();
   void charge();

 private:
   double charge_{};  // Electrical charge in kWh
   // ...
};

} // namespace eh


//---- <BatteryGen1.cpp> --------------------------------------------------------------------------

//#include <BatteryGen1.h>
#include <iostream>

namespace eh {

BatteryGen1::BatteryGen1( double charge )
   : charge_{charge}
{}

void BatteryGen1::drawPower()
{
   std::cout << "Drawing power from the 'BatteryGen1' (charge=" << charge_ << ")...\n";
}

void BatteryGen1::charge()
{
   std::cout << "Charging the 'BatteryGen1'...\n";
}

} // namespace eh


//---- <ElectricEngineGen1.h> ---------------------------------------------------------------------

namespace eh {

class ElectricEngineGen1
{
 public:
   void start();
   void stop();

 private:
   // ...
};

} // namespace eh


//---- <ElectricEngineGen1.cpp> -------------------------------------------------------------------

//#include <ElectricEngineGen1.h>
#include <iostream>

namespace eh {

void ElectricEngineGen1::start()
{
   std::cout << "Starting the 'ElectricEngineGen1'...\n";
}

void ElectricEngineGen1::stop()
{
   std::cout << "Stopping the 'ElectricEngineGen1'...\n";
}

} // namespace eh


//---- <Model10.h> --------------------------------------------------------------------------------

//#include <ElectricCar.h>

namespace eh {

class Model10 : public ElectricCar
{
 public:
   Model10();

   void drive();

   // ... more car or EH-specific functions

 private:
   // ... possibly some EH-specific data members that don't need to be hidden
};

} // namespace eh


//---- <Model10Impl.h> ----------------------------------------------------------------------------

//#include <ElectricCarImpl.h>
//#include <BatteryGen1.h>
//#include <ElectricEngineGen1.h>

namespace eh {

class Model10Impl : public ElectricCarImpl
{
 public:
   explicit Model10Impl( double charge )
      : battery_{charge}
      , engine_{}
   {}

   void start() override { engine_.start(); }
   void stop() override { engine_.stop(); }

   void drawPower() override { battery_.drawPower(); }
   void charge() override { battery_.charge(); }

   // ... more car or EH-specific functions

 private:
   BatteryGen1 battery_;
   ElectricEngineGen1 engine_;
   // ... more EH-specific data members (wheels, drivetrain, ...)
};

} // namespace eh


//---- <Model10.cpp> -------------------------------------------------------------------------

//#include <Model10.h>
//#include <Model10Impl.h>
#include <iostream>
#include <memory>

namespace eh {

Model10::Model10()
   : ElectricCar{ std::make_unique<Model10Impl>( 100.0 ) }
{}

void Model10::drive()
{
   getImpl()->start();
   getImpl()->drawPower();
   std::cout << "Driving the 'ElectricCar'...\n";
   getImpl()->stop();
}

} // namespace eh


//==== ARCHITECTURAL BOUNDARY =====================================================================


//---- <Main.cpp> ---------------------------------------------------------------------------------

//#include <Model10.h>
#include <cstdlib>

int main()
{
   eh::Model10 model10{};
   model10.drive();

   return EXIT_SUCCESS;
}
