/**************************************************************************************************
*
* \file MultipleStrategies_2.cpp
* \brief C++ Training - Programming Task for the Strategy Design Pattern
*
* Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
*
* This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
* context of the C++ training or with explicit agreement by Klaus Iglberger.
*
* Step 1: Consider the following shape classes with 'draw()', 'serialize()' and 'print()'
*         functions. How would you externalize the implementation details for all three
*         functions? Think about solutions for both dynamic and static polymorphism and
*         discuss pros and cons.
*
* Step 2: Think about adding a new types (e.g. 'Rectangle', different implementations of either
*         the 'draw()', 'serialize()' or 'print()' functions). How much effort would this be?
*         How many places would have to be touched and how would this relate to the OCP?
*
* Step 3: Think about adding a new operation. How much effort would this be? How many places
*         would have to be touched and how would this relate to the OCP?
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


//---- <JSONLibrary.h> (external) -----------------------------------------------------------------

#include <iostream>
#include <string>
// ... and many more JSON-related headers

namespace jl {

class JSONWriter
{
 public:
   void start_element( std::string const& element_name ) const
   {
      std::cout << element_name << " {\n";
   }

   template< typename Value >
   void add_key( std::string const& key, Value const& value ) const
   {
      std::cout << "   " << key << ": " << value << "\n";
   }

   void end_element() const
   {
      std::cout << "}\n";
   }
};

} // namespace jl


//---- <Point.h> ----------------------------------------------------------------------------------

struct Point
{
   double x;
   double y;
};


//---- <Shape.h> ----------------------------------------------------------------------------------

#include <string>

class Shape
{
 public:
   virtual ~Shape() = default;

   virtual void draw() const = 0;  // To draw the shape on the screen
   virtual std::string serialize() const = 0;  // To serialize the shape into a byte/character sequence
   virtual void print() const = 0;  // To print the shape in different formats (JSON, XML, ...)
};


//---- <ShapeStrategy.h> --------------------------------------------------------------------------

template< typename ShapeT >
class ShapeStrategy
{
 public:
   virtual ~ShapeStrategy() = default;

   virtual void draw( ShapeT const& shape ) const = 0;
   virtual std::string serialize( ShapeT const& shape ) const = 0;
   virtual void print( ShapeT const& shape ) const = 0;
};

template< typename ShapeT
        , typename DrawStrategy
        , typename SerializationStrategy
        , typename PrintStrategy >
class ConcreteShapeStrategy : public ShapeStrategy<ShapeT>
{
 public:
   ConcreteShapeStrategy( DrawStrategy drawer, SerializationStrategy serializer, PrintStrategy printer )
      : drawer_{ drawer }
      , serializer_{ serializer }
      , printer_{ printer }
   {}

   void draw( ShapeT const& shape ) const override { drawer_(shape); }
   std::string serialize( ShapeT const& shape ) const override { return serializer_(shape); }
   void print( ShapeT const& shape ) const override { printer_(shape); }

 private:
   DrawStrategy drawer_;
   SerializationStrategy serializer_;
   PrintStrategy printer_;
};


//---- <Circle.h> ---------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Point.h>
//#include <ShapeStrategy.h>
#include <memory>

class Circle : public Shape
{
 public:
   explicit Circle( double radius, std::unique_ptr<ShapeStrategy<Circle>>&& strategy );

   double radius() const noexcept { return radius_; }
   Point  center() const noexcept { return center_; }

   void draw() const override;
   std::string serialize() const override;
   void print() const override;

 private:
   double radius_;
   Point center_{};
   std::unique_ptr<ShapeStrategy<Circle>> strategy_;
};


//---- <Circle.cpp> -------------------------------------------------------------------------------

//#include <Circle.h>
#include <stdexcept>

Circle::Circle( double radius, std::unique_ptr<ShapeStrategy<Circle>>&& strategy )
   : radius_{ radius }
   , center_{}
   , strategy_{ std::move(strategy) }
{
   if( !strategy_ ) {
      throw std::invalid_argument( "Invalid shape strategy: nullptr passed" );
   }
}

void Circle::draw() const
{
   strategy_->draw(*this);
}

std::string Circle::serialize() const
{
   return strategy_->serialize(*this);
}

void Circle::print() const
{
   strategy_->print(*this);
}


//---- <Square.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
//#include <Point.h>
//#include <ShapeStrategy.h>
#include <memory>

class Square : public Shape
{
 public:
   explicit Square( double side, std::unique_ptr<ShapeStrategy<Square>>&& strategy );

   double side  () const noexcept { return side_; }
   Point  center() const noexcept { return center_; }

   void draw() const override;
   std::string serialize() const override;
   void print() const override;

 private:
   double side_;
   Point center_{};
   std::unique_ptr<ShapeStrategy<Square>> strategy_;
};


//---- <Square.cpp> -------------------------------------------------------------------------------

//#include <Square.h>
#include <stdexcept>

Square::Square( double side, std::unique_ptr<ShapeStrategy<Square>>&& strategy )
   : side_{ side }
   , center_{}
   , strategy_{ std::move(strategy) }
{
   if( !strategy_ ) {
      throw std::invalid_argument( "Invalid shape strategy: nullptr passed" );
   }
}

void Square::draw() const
{
   strategy_->draw(*this);
}

std::string Square::serialize() const
{
   return strategy_->serialize(*this);
}

void Square::print() const
{
   strategy_->print(*this);
}


//---- <Shapes.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
#include <memory>
#include <vector>

using Shapes = std::vector<std::unique_ptr<Shape>>;


//==== ARCHITECTURAL BOUNDARY =====================================================================


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
      serializer << typeid(Circle).hash_code()
                 << circle.radius() << circle.center().x << circle.center().y;
      return serializer.to_string();
   }

   std::string operator()( Square const& square ) const
   {
      fs::Serializer serializer{};
      serializer << typeid(Square).hash_code()
                 << square.side() << square.center().x << square.center().y;
      return serializer.to_string();
   }
};


//---- <JLPrinter.h> ------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <JSONLibrary.h>

class JLPrinter
{
 public:
   void operator()( Circle const& circle ) const
   {
      json_.start_element( "circle" );
      json_.add_key( "radius", circle.radius() );
      json_.end_element();
   }

   void operator()( Square const& square ) const
   {
      json_.start_element( "square" );
      json_.add_key( "side", square.side() );
      json_.end_element();
   }

 private:
   jl::JSONWriter json_;
};


//---- <Main.cpp> ---------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Square.h>
//#include <Shapes.h>
//#include <GLDrawer.h>
//#include <JLPrinter.h>
#include <cstdlib>
#include <iostream>
#include <memory>

int main()
{
   Shapes shapes{};

   using CircleStrategy =
      ConcreteShapeStrategy<Circle,GLDrawer,FSSerializer,JLPrinter>;
   using SquareStrategy =
      ConcreteShapeStrategy<Square,GLDrawer,FSSerializer,JLPrinter>;

   shapes.emplace_back(
      std::make_unique<Circle>( 2.3,
         std::make_unique<CircleStrategy>(
            GLDrawer{ gl::Color::red },
            FSSerializer{},
            JLPrinter{} ) ) );
   shapes.emplace_back(
      std::make_unique<Square>( 1.2,
         std::make_unique<SquareStrategy>(
            GLDrawer{ gl::Color::green },
            FSSerializer{},
            JLPrinter{} ) ) );
   shapes.emplace_back(
      std::make_unique<Circle>( 4.1,
         std::make_unique<CircleStrategy>(
            GLDrawer{ gl::Color::blue },
            FSSerializer{},
            JLPrinter{} ) ) );

   for( auto const& shape : shapes )
   {
      shape->draw();
      std::cout << "Serialized shape: \"" << shape->serialize() << "\"\n";
      shape->print();
   }

   return EXIT_SUCCESS;
}

