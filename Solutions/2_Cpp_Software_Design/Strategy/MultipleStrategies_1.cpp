/**************************************************************************************************
*
* \file MultipleStrategies_1.cpp
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


//---- <DrawStrategy.h> ---------------------------------------------------------------------------

template< typename ShapeT >
class DrawStrategy
{
 public:
   virtual ~DrawStrategy() = default;

   virtual void draw( ShapeT const& shape ) const = 0;
};


//---- <SerializationStrategy.h> ------------------------------------------------------------------

#include <string>

template< typename ShapeT >
class SerializationStrategy
{
 public:
   virtual ~SerializationStrategy() = default;

   virtual std::string serialize( ShapeT const& shape ) const = 0;
};


//---- <PrintStrategy.h> --------------------------------------------------------------------------

template< typename ShapeT >
class PrintStrategy
{
 public:
   virtual ~PrintStrategy() = default;

   virtual void print( ShapeT const& shape ) const = 0;
};


//---- <Circle.h> ---------------------------------------------------------------------------------

//#include <Circle.h>
//#include <Point.h>
//#include <DrawStrategy.h>
//#include <SerializationStrategy.h>
//#include <PrintStrategy.h>
#include <memory>

class Circle : public Shape
{
 public:
   explicit Circle( double radius
                  , std::unique_ptr<DrawStrategy<Circle>>&& drawer
                  , std::unique_ptr<SerializationStrategy<Circle>>&& serializer
                  , std::unique_ptr<PrintStrategy<Circle>>&& printer );

   double radius() const noexcept { return radius_; }
   Point  center() const noexcept { return center_; }

   void draw() const override;
   std::string serialize() const override;
   void print() const override;

 private:
   double radius_;
   Point center_{};
   std::unique_ptr<DrawStrategy<Circle>> drawer_;
   std::unique_ptr<SerializationStrategy<Circle>> serializer_;
   std::unique_ptr<PrintStrategy<Circle>> printer_;
};


//---- <Circle.cpp> -------------------------------------------------------------------------------

//#include <Circle.h>
#include <stdexcept>

Circle::Circle( double radius
              , std::unique_ptr<DrawStrategy<Circle>>&& drawer
              , std::unique_ptr<SerializationStrategy<Circle>>&& serializer
              , std::unique_ptr<PrintStrategy<Circle>>&& printer )
   : radius_{ radius }
   , center_{}
   , drawer_{ std::move(drawer) }
   , serializer_{ std::move(serializer) }
   , printer_{ std::move(printer) }
{
   if( !drawer_ ) {
      throw std::invalid_argument( "Invalid draw strategy: nullptr passed" );
   }
   if( !serializer_ ) {
      throw std::invalid_argument( "Invalid serialization strategy: nullptr passed" );
   }
   if( !printer_ ) {
      throw std::invalid_argument( "Invalid print strategy: nullptr passed" );
   }
}

void Circle::draw() const
{
   drawer_->draw(*this);
}

std::string Circle::serialize() const
{
   return serializer_->serialize(*this);
}

void Circle::print() const
{
   printer_->print(*this);
}


//---- <Square.h> ---------------------------------------------------------------------------------

//#include <Shape.h>
//#include <Point.h>
//#include <DrawStrategy.h>
//#include <SerializationStrategy.h>
//#include <PrintStrategy.h>
#include <memory>

class Square : public Shape
{
 public:
   explicit Square( double side
                  , std::unique_ptr<DrawStrategy<Square>>&& drawer
                  , std::unique_ptr<SerializationStrategy<Square>>&& serializer
                  , std::unique_ptr<PrintStrategy<Square>>&& printer );

   double side  () const noexcept { return side_; }
   Point  center() const noexcept { return center_; }

   void draw() const override;
   std::string serialize() const override;
   void print() const override;

 private:
   double side_;
   Point center_{};
   std::unique_ptr<DrawStrategy<Square>> drawer_;
   std::unique_ptr<SerializationStrategy<Square>> serializer_;
   std::unique_ptr<PrintStrategy<Square>> printer_;
};


//---- <Square.cpp> -------------------------------------------------------------------------------

//#include <Square.h>
#include <stdexcept>

Square::Square( double side
              , std::unique_ptr<DrawStrategy<Square>>&& drawer
              , std::unique_ptr<SerializationStrategy<Square>>&& serializer
              , std::unique_ptr<PrintStrategy<Square>>&& printer )
   : side_{ side }
   , center_{}
   , drawer_{ std::move(drawer) }
   , serializer_{ std::move(serializer) }
   , printer_{ std::move(printer) }
{
   if( !drawer_ ) {
      throw std::invalid_argument( "Invalid draw strategy: nullptr passed" );
   }
   if( !serializer_ ) {
      throw std::invalid_argument( "Invalid serialization strategy: nullptr passed" );
   }
   if( !printer_ ) {
      throw std::invalid_argument( "Invalid print strategy: nullptr passed" );
   }
}

void Square::draw() const
{
   drawer_->draw(*this);
}

std::string Square::serialize() const
{
   return serializer_->serialize(*this);
}

void Square::print() const
{
   printer_->print(*this);
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

class GLDrawer : public DrawStrategy<Circle>
               , public DrawStrategy<Square>
{
 public:
   explicit GLDrawer( gl::Color color ) : color_{color} {}

   void draw( Circle const& circle ) const override
   {
      std::cout << "circle: radius=" << circle.radius()
                << ", color = " << gl::to_string(color_) << '\n';
   }

   void draw( Square const& square ) const override
   {
      std::cout << "square: side=" << square.side()
                << ", color = " << gl::to_string(color_) << '\n';
   }

 private:
   gl::Color color_{};
};


//---- <FSSerializer.h> ---------------------------------------------------------------------------

//#include <SerializationStrategy.h>
//#include <Circle.h>
//#include <Square.h>
//#include <FastSerialization.h>

class FSSerializer : public SerializationStrategy<Circle>
                   , public SerializationStrategy<Square>
{
 public:
   std::string serialize( Circle const& circle ) const override
   {
      fs::Serializer serializer{};
      serializer << typeid(Circle).hash_code() << circle.radius()
                 << circle.center().x << circle.center().y;
      return serializer.to_string();
   }

   std::string serialize( Square const& square ) const override
   {
      fs::Serializer serializer{};
      serializer << typeid(Square).hash_code() << square.side()
                 << square.center().x << square.center().y;
      return serializer.to_string();
   }
};


//---- <JLPrinter.h> ------------------------------------------------------------------------------

//#include <PrintStrategy.h>
//#include <Circle.h>
//#include <Square.h>
//#include <JSONLibrary.h>

class JLPrinter : public PrintStrategy<Circle>
                , public PrintStrategy<Square>
{
 public:
   void print( Circle const& circle ) const override
   {
      json_.start_element( "circle" );
      json_.add_key( "radius", circle.radius() );
      json_.end_element();
   }

   void print( Square const& square ) const override
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

   shapes.emplace_back(
      std::make_unique<Circle>( 2.3,
         std::make_unique<GLDrawer>( gl::Color::red ),
         std::make_unique<FSSerializer>(),
         std::make_unique<JLPrinter>() ) );
   shapes.emplace_back(
      std::make_unique<Square>( 1.2,
         std::make_unique<GLDrawer>( gl::Color::green ),
         std::make_unique<FSSerializer>(),
         std::make_unique<JLPrinter>() ) );
   shapes.emplace_back(
      std::make_unique<Circle>( 4.1,
         std::make_unique<GLDrawer>( gl::Color::blue ),
         std::make_unique<FSSerializer>(),
         std::make_unique<JLPrinter>() ) );

   for( auto const& shape : shapes )
   {
      shape->draw();
      std::cout << "Serialized shape: \"" << shape->serialize() << "\"\n";
      shape->print();
   }

   return EXIT_SUCCESS;
}

