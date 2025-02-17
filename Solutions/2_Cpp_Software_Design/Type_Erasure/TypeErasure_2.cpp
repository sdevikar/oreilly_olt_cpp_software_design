/**************************************************************************************************
*
* \file TypeErasure_2.cpp
* \brief C++ Training - Programming Task for Type Erasure
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Step 1: Implement the 'Shape' class by means of Type Erasure. 'Shape' may require all types to
*         provide a 'free_draw()' function that draws them to the screen.
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

//#include <Point.h>

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

//#include <Point.h>

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

class Circle;
class Square;

void free_draw( Circle const& circle );
void free_draw( Square const& square );

struct FreeDraw
{
   template< typename T >
   void operator()( T const& shape ) const
   {
      free_draw(shape);
   }
};


//---- <Draw.cpp> ---------------------------------------------------------------------------------

//#include <Draw.h>
//#include <Circle.h>
//#include <Square.h>
//#include <GraphicsLibrary.h>
#include <iostream>

void free_draw( Circle const& circle )
{
   std::cout << "circle: radius=" << circle.radius() << std::endl;
}

void free_draw( Square const& square )
{
   std::cout << "square: side=" << square.side() << std::endl;
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

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

// Pre-C++20 constraint to formulate the requirement that every shape needs to be drawable
// by the given 'DrawStrategy'
/*
template< typename ShapeT, typename DrawStrategy, typename = void >
struct is_drawable
   : public std::false_type
{};

template< typename ShapeT, typename DrawStrategy >
struct is_drawable< ShapeT, DrawStrategy
                  , std::void_t< decltype( std::declval<DrawStrategy>()( std::declval<ShapeT>() ) ) > >
   : public std::true_type
{};

template< typename ShapeT, typename DrawStrategy >
inline constexpr bool is_drawable_v = is_drawable<ShapeT,DrawStrategy>::value;

template< typename ShapeT, typename DrawStrategy >
using enable_if_is_drawable =
   std::enable_if_t< is_drawable_v<ShapeT,DrawStrategy>, bool >;
*/

// C++20 concept to formulate the requirement that every shape needs to be drawable by the
// given 'DrawStrategy'
/*
template< typename ShapeT, typename DrawStrategy >
concept Drawable =
   requires ( ShapeT shape, DrawStrategy drawer )
   {
      drawer( shape );
   };
*/

class Shape
{
 public:
   template< typename ShapeT, typename DrawStrategy, typename SerializationStrategy >  // Strategy design pattern
   Shape( ShapeT const& shape, DrawStrategy const& drawer, SerializationStrategy const& serializer )
      : pimpl_{ std::make_unique<Model<ShapeT,DrawStrategy,SerializationStrategy>>( shape, drawer, serializer ) }
   {}

   Shape( Shape const& other )
      : pimpl_{ other.pimpl_->clone() }
   {}

   Shape& operator=( const Shape& other )
   {
      // Copy-and-swap idiom
      Shape tmp( other );
      std::swap( pimpl_, tmp.pimpl_ );
      return *this;
   }

   ~Shape() = default;
   Shape( Shape&& ) = default;
   Shape& operator=( Shape&& ) = default;

   std::string serialize() const
   {
      return pimpl_->do_serialize();
   }

 private:
   friend void free_draw( Shape const& shape )
   {
      shape.pimpl_->do_draw();
   }

   struct Concept  // External Polymorphism design pattern
   {
      virtual ~Concept() = default;
      virtual void do_draw() const = 0;
      virtual std::string do_serialize() const = 0;
      virtual std::unique_ptr<Concept> clone() const = 0;  // Prototype design pattern
   };

   template< typename ShapeT, typename DrawStrategy, typename SerializationStrategy >
   struct Model final : public Concept
   {
      explicit Model( ShapeT const& shape, DrawStrategy const& drawer
                    , SerializationStrategy const& serializer )
         : shape_{ shape }
         , drawer_{ drawer }
         , serializer_{ serializer }
      {}

      void do_draw() const final { drawer_(shape_); }
      std::string do_serialize() const override { return serializer_(shape_); }
      std::unique_ptr<Concept> clone() const final { return std::make_unique<Model>(*this); }

      ShapeT shape_;
      DrawStrategy drawer_;
      SerializationStrategy serializer_;
   };

   std::unique_ptr<Concept> pimpl_;  // Bridge design pattern (pimpl idiom)
};


//---- <Shapes.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
#include <vector>

using Shapes = std::vector<Shape>;


//---- <DrawAllShapes.h> --------------------------------------------------------------------------

//#include <Shapes.h>

void drawAllShapes( Shapes const& shapes );


//---- <DrawAllShapes.cpp> ------------------------------------------------------------------------

//#include <DrawAllShapes.h>
//#include <Draw.h>

void drawAllShapes( Shapes const& shapes )
{
   for( auto const& shape : shapes )
   {
      free_draw( shape );
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
      serialized_shapes += shape.serialize();
   }

   std::cout << "Serialized shapes: \"" << serialized_shapes << "\"\n";
}


//---- <Main.cpp> ---------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <Shapes.h>
//#include <DrawAllShapes.h>
//#include <GLDrawer.h>
//#include <FSSerializer.h>
#include <cstdlib>

int main()
{
   Shapes shapes{};

   auto lambda_drawer = [color = gl::Color::blue]( Circle const& circle ){
      std::cout << "circle: radius=" << circle.radius()
                << ", color = " << gl::to_string(color) << std::endl;
   };

   shapes.emplace_back( Circle{ 2.3 }, FreeDraw{}, FSSerializer{} );
   shapes.emplace_back( Square{ 1.2 }, GLDrawer{gl::Color::green}, FSSerializer{} );
   shapes.emplace_back( Circle{ 4.1 }, lambda_drawer, FSSerializer{} );

   drawAllShapes( shapes );

   serializeAllShapes( shapes );

   return EXIT_SUCCESS;
}

