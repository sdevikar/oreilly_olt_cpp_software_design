/**************************************************************************************************
*
* \file Visitor_Classic.cpp
* \brief C++ Training - Programming Task for the Visitor Design Pattern
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Step 1: Refactor the given 'Shape' hierarchy by means of the visitor design pattern to enable
*         the easy addition of operations (e.g. drawing, rotating, serialization, ...).
*
* Step 2: Switch from one to another graphics library. Discuss the feasibility of the change:
*         how easy is the change? How many pieces of code on which level of the architecture
*         have to be touched?
*
* Step 3: Implement the 'area()' operations as a classic visitor. Hint: the area of a circle
*         is 'radius*radius*M_PI', the area of a square is 'side*side'.
*
* Task 4: Add the new feature to serialize shapes by means of the 'FastSerialization' library.
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


//---- <GraphicsFramework.h> (external) -----------------------------------------------------------

#include <sstream>
#include <string>
// ... and many more graphics-related headers

namespace gf {

enum class Color : int
{
   yellow  = 0xFFFF00,
   cyan    = 0x00FFFF,
   magenta = 0xFF00FF
};

using Brightness = unsigned int;

std::string print_string( Color color, Brightness brightness )
{
   std::ostringstream oss;

   switch( color ) {
      case Color::yellow:
         oss << "yellow (0xFFFF00)";
         break;
      case Color::cyan:
         oss << "cyan (0x00FFFF)";
         break;
      case Color::magenta:
         oss << "magenta (0xFF00FF)";
         break;
      default:
         oss << "unknown";
         break;
   }

   oss << ", brightness=" << brightness;

   return oss.str();
}

} // namespace gf


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


//---- <ShapeVisitor.h> ---------------------------------------------------------------------------

class Circle;
class Square;

class ShapeVisitor
{
 public:
   virtual void visit( Circle const& ) const = 0;
   virtual void visit( Square const& ) const = 0;
};


//---- <Shape.h> ----------------------------------------------------------------------------------

//#include <ShapeVisitor.h>

class Shape
{
 public:
   virtual ~Shape() = default;

   virtual void accept( ShapeVisitor const& ) const = 0;
};


//---- <Circle.h> ---------------------------------------------------------------------------------

//#include <Point.h>
//#include <Shape.h>

class Circle : public Shape
{
 public:
   explicit Circle( double radius )
      : radius_{ radius }
      , center_{}
   {}

   double radius() const { return radius_; }
   Point  center() const { return center_; }

   void accept( ShapeVisitor const& v ) const override { v.visit( *this ); }

 private:
   double radius_;
   Point center_;
};


//---- <Square.h> ---------------------------------------------------------------------------------

//#include <Point.h>
//#include <Shape.h>

class Square : public Shape
{
 public:
   explicit Square( double side )
      : side_{ side }
      , center_{}
   {}

   double side() const { return side_; }
   Point center() const { return center_; }

   void accept( ShapeVisitor const& v ) const override { v.visit( *this ); }

 private:
   double side_;
   Point center_;
};


//---- <Shapes.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
#include <memory>
#include <vector>

using Shapes = std::vector<std::unique_ptr<Shape>>;


//==== ARCHITECTURAL BOUNDARY =====================================================================


//---- <Draw.h> -----------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <ShapeVisitor.h>
//#include <GraphicsLibrary.h>
#include <iostream>

class GLDrawer : public ShapeVisitor
{
 public:
   explicit GLDrawer( gl::Color color ) : color_{color} {}

   void visit( Circle const& circle ) const override
   {
      std::cout << "circle: radius=" << circle.radius()
                << ", color = " << gl::to_string(color_) << '\n';
   }

   void visit( Square const& square ) const override
   {
      std::cout << "square: side=" << square.side()
                << ", color = " << gl::to_string(color_) << '\n';
   }

 private:
   gl::Color color_{};
};


//---- <DrawAllShapes.h> --------------------------------------------------------------------------

//#include <Shapes.h>

void drawAllShapes( Shapes const& shapes );


//---- <DrawAllShapes.cpp> ------------------------------------------------------------------------

//#include <DrawAllShapes.h>
//#include <GLDrawer.h>

void drawAllShapes( Shapes const& shapes )
{
   for( auto const& shape : shapes )
   {
      shape->accept( GLDrawer{gl::Color::red} );
   }
}


//---- <FSSerializer.h> ---------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <ShapeVisitor.h>
//#include <FastSerialization.h>

class FSSerializer : public ShapeVisitor
{
 public:
   void visit( Circle const& circle ) const override
   {
      serializer_ << typeid(Circle).hash_code() << circle.radius()
                  << circle.center().x << circle.center().y;
   }

   void visit( Square const& square ) const override
   {
      serializer_ << typeid(Square).hash_code() << square.side()
                  << square.center().x << square.center().y;
   }

   std::string to_string() const { return serializer_.to_string(); }

 private:
   mutable fs::Serializer serializer_;
};


//---- <SerializeAllShapes.h> ---------------------------------------------------------------------

//#include <Shapes.h>

void serializeAllShapes( Shapes const& shapes );


//---- <SerializeAllShapes.cpp> ------------------------------------------------------------------------

//#include <SerializeAllShapes.h>
//#include <FSSerializer.h>
#include <iostream>

void serializeAllShapes( Shapes const& shapes )
{
   FSSerializer serializer{};

   for( auto const& shape : shapes )
   {
      shape->accept( serializer );
   }

   std::cout << "Serialized shapes: \"" << serializer.to_string() << "\"\n";
}


//---- <Area.h> -----------------------------------------------------------------------------------

//#include <ShapeVisitor.h>
#define _USE_MATH_DEFINES
#include <cmath>

class Area : public ShapeVisitor
{
 public:
   void visit( Circle const& circle ) const override
   {
      area_ = circle.radius() * circle.radius() * M_PI;
   }

   void visit( Square const& square ) const override
   {
      area_ = square.side() * square.side();
   }

   double getArea() const { return area_; }

 private:
   mutable double area_;
   //double& area_;  // Alternatively the area could be returned via a reference,
                     // initialized in a constructor.
};


//---- <Main.cpp> ---------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <Shapes.h>
//#include <DrawAllShapes.h>
#include <cstdlib>

int main()
{
   Shapes shapes{};

   shapes.emplace_back( std::make_unique<Circle>( 2.3 ) );
   shapes.emplace_back( std::make_unique<Square>( 1.2 ) );
   shapes.emplace_back( std::make_unique<Circle>( 4.1 ) );

   drawAllShapes( shapes );

   serializeAllShapes( shapes );

   return EXIT_SUCCESS;
}