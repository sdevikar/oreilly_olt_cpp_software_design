/**************************************************************************************************
*
* \file ObjectOriented.cpp
* \brief C++ Training - Example for an object-oriented approach to draw shapes
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Task 1: Evaluate the given design with respect to changeability and extensibility.
*
* Task 2: Add a new kind of shape (triangle, rectangle, pentagon, ...) and observe how well the
*         new code can be integrated and how many new dependencies are created.
*
* Task 3: Add the new feature to serialize shapes by means of the 'FastSerialization' library.
*         Observe how well the new code can be integrated and how many new dependencies are
*         created.
*
**************************************************************************************************/


//---- <GraphicsLibrary.h> (external) -------------------------------------------------------------

#include <string>
// ... and many more graphics-related headers

namespace gl {

enum class Color
{
   red   = 0xFF0000,
   green = 0x00FF00,
   blue  = 0x0000FF
};

std::string to_string( Color color )
{
   switch( color ) {
      case Color::red:
         return "red (0xFF0000)";
      case Color::green:
         return "green (0x00FF00)";
      case Color::blue:
         return "blue (0x0000FF)";
      default:
         return "unknown";
   }
}

} // namespace gl


//---- <FastSerialization.h> (external) -----------------------------------------------------------

#include <cstddef>
#include <string>
#include <type_traits>
#include <vector>
// ... and many more serialization-related headers

namespace fs {

class Serializer
{
 public:
   std::string to_string() const
   {
      return std::string( buffer_.data(), buffer_.size() );
   }

 private:
   std::vector<char> buffer_;

   template< typename T, typename = std::enable_if_t< std::is_arithmetic_v<T> > >
   //   requires std::is_arithmetic_v<T>  // C++20 concept
   friend Serializer& operator<<( Serializer& serializer, T value )
   {
      size_t const old_size = serializer.buffer_.size();
      serializer.buffer_.resize( old_size + sizeof(T) );

      auto* data = serializer.buffer_.data() + old_size;
      ::new (data) T{value};

      return serializer;
   }
};

} // namespace fs


//---- <Point.h> ----------------------------------------------------------------------------------

struct Point
{
   double x;
   double y;
};


//---- <Shape.h> ----------------------------------------------------------------------------------

class Shape
{
 public:
   virtual ~Shape() = default;

   virtual void draw( /*Graphics-related parameters*/ ) const = 0;
};


//---- <Circle.h> ---------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Point.h>
//#include <GraphicsLibrary.h>

class Circle : public Shape
{
 public:
   explicit Circle( double radius, gl::Color color );

   double radius() const noexcept { return radius_; }
   Point  center() const noexcept { return center_; }

   void draw() const override;

 private:
   double radius_;
   Point center_{};
   gl::Color color_{};
};


//---- <Circle.cpp> -------------------------------------------------------------------------------

//#include <Circle.h>
#include <iostream>

Circle::Circle( double radius, gl::Color color )
   : radius_{ radius }
   , center_{}
   , color_{ color }
{}

void Circle::draw() const
{
   std::cout << "circle: radius=" << radius_
             << ", color = " << gl::to_string(color_) << '\n';
}


//---- <Square.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
//#include <Point.h>
//#include <GraphicsLibrary.h>

class Square : public Shape
{
 public:
   explicit Square( double side, gl::Color color );

   double side  () const noexcept { return side_; }
   Point  center() const noexcept { return center_; }

   void draw() const override;

 private:
   double side_;
   Point center_{};
   gl::Color color_{};
};


//---- <Square.cpp> -------------------------------------------------------------------------------

//#include <Square.h>
#include <iostream>

Square::Square( double side, gl::Color color )
   : side_{ side }
   , center_{}
   , color_{ color }
{}

void Square::draw() const
{
   std::cout << "square: side=" << side_
             << ", color = " << gl::to_string(color_) << '\n';
}


//---- <Shapes.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
#include <memory>
#include <vector>

using Shapes = std::vector<std::unique_ptr<Shape>>;


//---- <DrawAllShapes.h> --------------------------------------------------------------------------

//#include <Shapes.h>

void drawAllShapes( Shapes const& shapes );


//---- <DrawAllShapes.cpp> ------------------------------------------------------------------------

//#include <DrawAllShapes.h>

void drawAllShapes( Shapes const& shapes )
{
   for( auto const& shape : shapes )
   {
      shape->draw();
   }
}


//==== ARCHITECTURAL BOUNDARY =====================================================================


//---- <NewShape.h --------------------------------------------------------------------------------

// Task 2: Add a new kind of shape (triangle, rectangle, pentagon, ...) and observe how well the
//         new code can be integrated and how many new dependencies are created.


//---- <SerializeAllShapes.h> -------------------------------------------------------------------

//#include <FastSerialization.h>

// Task 3: Add the new feature to serialize shapes by means of the 'FastSerialization' library.
//         Observe how well the new code can be integrated and how many new dependencies are
//         created.

//#include <Shapes.h>
//#include <FastSerialization.h>
#include <iostream>

/*
void serializeAllShapes( Shapes const& shapes )
{
   fs::Serializer serializer{};

   // TODO: Serialize all shapes

   std::cout << "Serialized shapes: \"" << serializer.to_string() << "\"\n";
}
*/


//---- <Main.cpp> ---------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <Shapes.h>
//#include <DrawAllShapes.h>
//#include <SerializeAllShapes.h>
#include <cstdlib>
#include <memory>

int main()
{
   Shapes shapes{};

   shapes.emplace_back( std::make_unique<Circle>( 2.3, gl::Color::red ) );
   shapes.emplace_back( std::make_unique<Square>( 1.2, gl::Color::green ) );
   shapes.emplace_back( std::make_unique<Circle>( 4.1, gl::Color::blue ) );

   drawAllShapes( shapes );

   //serializeAllShapes( shapes );

   return EXIT_SUCCESS;
}

