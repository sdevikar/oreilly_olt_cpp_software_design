/**************************************************************************************************
*
* \file ExternalPolymorphism_2.cpp
* \brief C++ Training - Programming Task for the External Polymorphism Design Pattern
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Step 1: Refactor the given Strategy-based solution and extract the polymorphic behavior of
*         all shapes by means of the External Polymorphism design pattern. Note that the general
*         behavior should remain unchanged.
*
* Step 2: Switch from one to another graphics library. Discuss the feasibility of the change:
*         how easy is the change? How many pieces of code on which level of the architecture
*         have to be touched?
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


//---- <Circle.h> ---------------------------------------------------------------------------------

class Circle
{
 public:
   explicit Circle( double radius )
      : radius_{ radius }
      , center_{}
   {}

   double radius() const { return radius_; }
   Point  center() const { return center_; }

 private:
   double radius_;
   Point center_;
};


//---- <Square.h> ---------------------------------------------------------------------------------

class Square
{
 public:
   explicit Square( double side )
      : side_{ side }
      , center_{}
   {}

   double side() const { return side_; }
   Point center() const { return center_; }

 private:
   double side_;
   Point center_;
};


//==== ARCHITECTURAL BOUNDARY =====================================================================


//---- <Draw.h> -----------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <GraphicsLibrary.h>

void free_draw( Circle const& circle, gl::Color color );
void free_draw( Square const& square, gl::Color color );


//---- <Draw.cpp> ---------------------------------------------------------------------------------

//#include <Draw.h>
#include <iostream>

void free_draw( Circle const& circle, gl::Color color )
{
   std::cout << "circle: radius=" << circle.radius()
             << ", color = " << gl::to_string(color) << '\n';
}

void free_draw( Square const& square, gl::Color color )
{
   std::cout << "square: side=" << square.side()
             << ", color = " << gl::to_string(color) << '\n';
}


//---- <GLDrawer.h> -------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <GraphicsLibrary.h>
#include <iostream>

class GLDrawer
{
 public:
   explicit GLDrawer( gl::Color color ) : color_{color} {}

   void operator()( Circle const& circle ) const
   {
      std::cout << "circle: radius=" << circle.radius()
                << ", color = " << gl::to_string(color_) << '\n';
   }

   void operator()( Square const& square ) const
   {
      std::cout << "square: side=" << square.side()
                << ", color = " << gl::to_string(color_) << '\n';
   }

 private:
   gl::Color color_{};
};


//---- <FSSerializer.h> ---------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <FastSerialization.h>

class FSSerializer
{
 public:
   std::string operator()( Circle const& circle ) const
   {
      fs::Serializer serializer{};
      serializer << typeid(Circle).hash_code() << circle.radius()
                 << circle.center().x << circle.center().y;
      return serializer.to_string();
   }

   std::string operator()( Square const& square ) const
   {
      fs::Serializer serializer{};
      serializer << typeid(Square).hash_code() << square.side()
                 << square.center().x << square.center().y;
      return serializer.to_string();
   }
};


//---- <Shape.h> ----------------------------------------------------------------------------------

//#include <functional>
//#include <stdexcept>
#include <string>

class ShapeConcept
{
 public:
   virtual ~ShapeConcept() = default;

   virtual void draw() const = 0;
   virtual std::string serialize() const = 0;
};

template< typename ShapeT
        , typename DrawStrategy
        , typename SerializationStrategy >
class ShapeModel : public ShapeConcept
{
 public:
   ShapeModel( ShapeT const& shape, DrawStrategy drawer, SerializationStrategy serializer )
      : shape_{ shape }
      , drawer_{ std::move(drawer) }
      , serializer_{ std::move(serializer) }
   {}

   void draw() const override { drawer_(shape_); }
   std::string serialize() const override { return serializer_(shape_); }

 private:
   ShapeT shape_;
   DrawStrategy drawer_;
   SerializationStrategy serializer_;
};

// Alternative implementation using std::function
//   Advantages: Runtime flexibility
//   Disadvantages: One more pointer indirection, i.e. less performance
/*
template< typename ShapeT >
class ShapeModel : public ShapeConcept
{
 public:
   using DrawStrategy = std::function<void(ShapeT const&)>;
   using SerializationStrategy = std::function<std::string(ShapeT const&)>;

   ShapeModel( ShapeT const& shape, DrawStrategy drawer, SerializationStrategy serializer )
      : shape_{ shape }
      , drawer_{ std::move(drawer) }
      , serializer_{ std::move(serializer) }
   {
      if( !drawer_ ) {
         throw std::invalid_argument( "Invalid draw strategy" );
      }
      if( !serializer_ ) {
         throw std::invalid_argument( "Invalid serialization strategy" );
      }
   }

   void draw() const override { drawer_(shape_); }
   std::string serialize() const override { return serializer_(shape_); }

 private:
   ShapeT shape_;
   DrawStrategy drawer_;
   SerializationStrategy serializer_;
};
*/


//---- <Shapes.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
#include <memory>
#include <vector>

using Shapes = std::vector<std::unique_ptr<ShapeConcept>>;


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


//---- <SerializeAllShapes.h> ---------------------------------------------------------------------

//#include <Shapes.h>

void serializeAllShapes( Shapes const& shapes );


//---- <SerializeAllShapes.cpp> -------------------------------------------------------------------

//#include <SerializeAllShapes.h>
#include <iostream>

void serializeAllShapes( Shapes const& shapes )
{
   std::string serialized_shapes{};

   for( auto const& shape : shapes )
   {
      serialized_shapes += shape->serialize();
   }

   std::cout << "Serialized shapes: \"" << serialized_shapes << "\"\n";
}


//---- <Main.cpp> ---------------------------------------------------------------------------------

//#include <Circle>
//#include <Square>
//#include <DrawAllShapes>
//#include <GLDrawer>
#include <cstdlib>

template< typename ShapeT, typename DrawStrategy, typename SerializationStrategy >
auto make_shape_model( ShapeT const& shape, DrawStrategy drawer, SerializationStrategy serializer )
{
   using Model = ShapeModel<ShapeT,DrawStrategy,SerializationStrategy>;
   return std::make_unique<Model>( shape, std::move(drawer), std::move(serializer) );
}

int main()
{
   Shapes shapes{};

   shapes.emplace_back(
      make_shape_model( Circle{2.3}, GLDrawer(gl::Color::red), FSSerializer{} ) );
   shapes.emplace_back(
      make_shape_model( Square{1.2}, GLDrawer(gl::Color::green), FSSerializer{} ) );
   shapes.emplace_back(
      make_shape_model( Circle{4.1}, GLDrawer(gl::Color::blue), FSSerializer{} ) );

   drawAllShapes( shapes );

   serializeAllShapes( shapes );

   return EXIT_SUCCESS;
}

