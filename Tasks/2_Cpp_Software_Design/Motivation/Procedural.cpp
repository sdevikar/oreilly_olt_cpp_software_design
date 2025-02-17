/**************************************************************************************************
*
* \file Procedural.cpp
* \brief C++ Training - Example for a procedural approach to draw shapes
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Task 1: Evaluate the given design with respect to changeability and extensibility.
*
* Task 2: Add the new feature to serialize shapes by means of the 'FastSerialization' library.
*         Observe how well the new code can be integrated and how many new dependencies are
*         created.
*
* Task 3: Add a new kind of shape (triangle, rectangle, pentagon, ...) and observe how well the
*         new code can be integrated and how many new dependencies are created.
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

enum class ShapeType
{
   circle,
   square
};

class Shape
{
 protected:
   explicit Shape( ShapeType type )
      : type_{ type }
   {}

 public:
   virtual ~Shape() = default;

   ShapeType getType() const noexcept { return type_; }

 private:
   ShapeType type_;
};


//---- <Circle.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
//#include <Point.h>

class Circle : public Shape
{
 public:
   explicit Circle( double radius );

   double radius() const noexcept { return radius_; }
   Point  center() const noexcept { return center_; }

 private:
   double radius_;
   Point center_{};
};


//---- <Circle.cpp> -------------------------------------------------------------------------------

//#include <Circle.h>

Circle::Circle( double radius )
   : Shape( ShapeType::circle )
   , radius_{ radius }
   , center_{}
{}


//---- <Square.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
//#include <Point.h>

class Square : public Shape
{
 public:
   explicit Square( double side );

   double side  () const noexcept { return side_; }
   Point  center() const noexcept { return center_; }

 private:
   double side_;
   Point center_{};
};


//---- <Square.cpp> -------------------------------------------------------------------------------

//#include <Square.h>

Square::Square( double side )
   : Shape( ShapeType::square )
   , side_{ side }
   , center_{}
{}


//---- <Draw.h> -----------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <GraphicsLibrary.h>

void draw( Circle const& circle, gl::Color color );
void draw( Square const& square, gl::Color color );


//---- <Draw.cpp> ---------------------------------------------------------------------------------

//#include <Draw.h>
#include <iostream>

void draw( Circle const& circle, gl::Color color )
{
   std::cout << "circle: radius=" << circle.radius()
             << ", color = " << gl::to_string(color) << '\n';
}

void draw( Square const& square, gl::Color color )
{
   std::cout << "square: side=" << square.side()
             << ", color = " << gl::to_string(color) << '\n';
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
//#include <Circle.h>
//#include <Square.h>
//#include <Draw.h>

void drawAllShapes( Shapes const& shapes )
{
   for( auto const& shape : shapes )
   {
      switch( shape->getType() )
      {
         case ShapeType::circle:
            draw( *static_cast<Circle const*>( shape.get() ), gl::Color::red );
            break;
         case ShapeType::square:
            draw( *static_cast<Square const*>( shape.get() ), gl::Color::green );
            break;
      }
   }
}


//==== ARCHITECTURAL BOUNDARY =====================================================================


//---- <SerializeAllShapes.h> -------------------------------------------------------------------

//#include <FastSerialization.h>

// Task 2: Add the new feature to serialize shapes by means of the 'FastSerialization' library.
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


//---- <NewShape.h --------------------------------------------------------------------------------

// Task 3: Add a new kind of shape (triangle, rectangle, pentagon, ...) and observe how well the
//         new code can be integrated and how many new dependencies are created.


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

   shapes.emplace_back( std::make_unique<Circle>( 2.3 ) );
   shapes.emplace_back( std::make_unique<Square>( 1.2 ) );
   shapes.emplace_back( std::make_unique<Circle>( 4.1 ) );

   drawAllShapes( shapes );

   //serializeAllShapes( shapes );

   return EXIT_SUCCESS;
}

