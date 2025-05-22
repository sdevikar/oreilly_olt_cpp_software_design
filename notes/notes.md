# Notes

## Procedural example

- In `oreilly_olt_cpp_software_design/Tasks/2_Cpp_Software_Design/Motivation/Procedural.cpp` pretty much everything depends on the `ShapeType` enum. A simple change here will
  - break ABI (i.e. everything that assumed a certain binary structure for `ShapeType` enum will be in for a surprise)
  - all dependencies will have to recompile. e.g. if you add a triangle to `ShapeType` enum, the `Square`, `Circle` etc. will also have to compile resulting in increased build time
  - switch statement will have to be extended each time

## Object oriented example

- See `oreilly_olt_cpp_software_design/Tasks/2_Cpp_Software_Design/Motivation/ObjectOriented.cpp`
- This gets rid of the `ShapeType` enum altogether and hands the responsibility of draw API to individual class
- The `Shapes` base class has a pure virtual function called `draw` that the concrete class will implement
- But notice that we're still dependent of the graphics library

## Visitor design pattern

### Classic implementation

- See `oreilly_olt_cpp_software_design/Tasks/2_Cpp_Software_Design/Visitor`
- The core idea for the visitor class is to add a new functionality "easily". We have the left side (Visitor hierarchy) and the right side (the client hierarchy). In the classic visitor design pattern, both sides accept the other side's instance using designated methods.
- Client side calls `visit(*this)` and gives its own instance to visitor. Visitor then does something with it.
- Visitor side calls `accept(*this)` and gives its own instance to all clients. Clients then do something with it.
- There is cyclic dependency in the classic visitor design pattern
- On the visitor side, we implement different concrete implementations for the different functionality that we want to implement. See `Visitor_Refactoring.cpp` for example. Here, you can implement `Area` class, which would be a concrete implementation of `ShapeVisitor`, just like the `GlDrawer`
- However, as you'd notice, since the signature of `visit` function is fixed, it puts some serious limitations on what the operation can actually return, modify, etc.

### Modern C++ implementation

Consider following code:

```cpp
struct Print{
    
    void operator()(int value) {std::cout << "I am an int:" << value << std::endl;}
    void operator()(double value) {std::cout << "I am a double:" << value << std::endl;}
};

int main()
{
    int value = 5;
    double dvalue = 5.5;
    struct Print p{};
    p(value);
    p(dvalue);
    return 0;
}
```

- There's `std::variant<T1,T2,T3...>` class that is equivalent to C union. e.g. `std::variant<int,double,std::string>`will hold either int, double, or a string.
- With this variant class, we can do the same exact thing but in a much concise manner

```cpp
std::variant<int, double> v;
std::visit(Print{}, v);
```

Notice here that the left and right args of the visit function are the left and right sides of the visitor pattern hierarchy respectively

### Refactoring the shapes program

Take a look at the refacrored version of `oreilly_olt_cpp_software_design/Tasks/2_Cpp_Software_Design/Visitor/Visitor_Refactoring.cpp`. Here, we have:

- `using Shape = std::variant<Circle, Square>;` i.e. declared a variant to use all kinds of shapes and made a vector out of them `using Shapes = std::vector<Shape>;`
- In the vector of Shapes, we insert the instances `shapes.emplace_back( std::make_unique<Circle>( 4.1 ) );`
- In `drawAllShapes` we then `std::visit(drawer, shape);`, where `drawer` is the same graphics library wrapper class as before except in there we now have defined operator() like `void operator()( Circle const& circle ) const`

In effect, we don't have any base classes

#### Advantages and disadvantages

- This solution, as we can check from benchmarking, is almost twice as fast as the object oriented design because there are no abstract classes, virtual functions, pointers and redirection etc.
- Bad: Adding new shapes is still not easy. If your intention is to support adding and deleting shapes, this is not the pattern for you


## Strategy design pattern

Let's say we wanted the flexibility to add more or less shapes ad-hoc. The visitor pattern will be still inconvenient. Among other things, the classes like `GlDrawer` will get bigger and bigger as we introduce operator() implementations.
Going back to the object oriented ideas where we can have something like this:

```text
    Shape
      ^
  ____|____
 |         |
 Circle    Square

```

- If Shape has `draw` pure virtual function, everything inheriting from it will have to _implement_ it and that will create dependency on the graphics library (because `draw()` will have to call some graphics library functions)
- To avoid this, we can further inherit from concrete implementations. See `oreilly_olt_cpp_software_design/PDF/2_Cpp_Software_Design.pdf`. slide# 121 where `OpenGlCircle`, `MetalCircle` are inheriting from `Circle` and overriding `draw()`
- The issue here is that when we add more functions, like `Serialize`, the inheritence layers will keep growing. So this isn't feasible.

### Solution

- (Direct) Inheritence is rarely an answer. Choose composition instead.
- In strategy, instead of using inheritence, we use composition. e.g. Circle will be constructed by injecting (passing to its constructor), an object that is able to provide implementation from `draw`
  - In this case, the Circle will be the context and the entity that provides `draw()` implementation would be the `strategy`
  - The strategy side therefore will actually have a base class that declares a pure virtual `draw()`
- Note that the strategy implementation is not just limited to injecting the implementation through constructor. We can also use templates. e.g. in std::vector we pass the allocator as the second template parameter
- Strategy pattern is used all over the place without us realizing it in some other ways. For example,  by passing lambda functions

#### Classic Implementation

- `oreilly_olt_cpp_software_design/Tasks/2_Cpp_Software_Design/Strategy/Strategy_Refactoring.cpp` shows the classic implementation
  - Concrete shapes still inherit from the Shapes class which has `draw` api
  - `DrawStrategy` is an abstract class for function declarations for different shapes (e.g. `virtual void draw( Circle const& circle ) const = 0;`)
  - The graphics library classes will inherit from this `DrawStrategy` class and implement the `draw` for each shape
  - The concrete shape classes will accept and store the reference to the `DrawStrategy` and eventually forward the `draw` calls

#### Issues with classic implementation

```cpp
class Circle;
class Square;

class DrawStrategy
{
 public:
   virtual ~DrawStrategy() = default;

   virtual void draw( Circle const& circle ) const = 0;
   virtual void draw( Square const& square ) const = 0;
};

class Circle {
  public:
  Circle( double radius, std::unique_ptr<DrawStrategy>&& drawer ) //... init list
  {
    // ... etc.
  }
}
```

In the implementation above, observe:

- Circle and Square are forward declared because DrawStrategy depends on all these shapes
- By looking at Circle constructor, we can tell it depends on DrawStrategy

There are two issues here:

1. There's a cyclic dependency (which is fine and can be taken care of by forward declarations)
2. Circle, Square, etc. indirectly know about each other via DrawStrategy. In order to compile Circle, Drawstrategy needs to be compiled, which needs Square and potentially all other shapes. So we have created indirect dependency

To resolve this indirect depenency, we can create a Strategy class for each shape. i.e. DrawStrategyCircle, DrawStrategySquare, etc. But this will lead to base class explosion. We can use templates instead.
Note that at this point, the GLDrawer will still have to inherit from multiple DrawStrategy classes. e.g DrawStrategy<Square>, DrawStrategy<Circle>, etc.

#### Modern alternative to classing Strategy implementation

- `std::function<T>` takes a callable type as a template parameter
- So, using injections like `using DrawStrategy = std::function<void(Square const&)>;` we can implement strategy design pattern

Even with the modern implementation, strategy feels like a more classical "Reference based" semantics implementation. In modern C++, the goal is to move more and more towards value semantics


## External Polymorphism

[External Polymorphism Design Patter](images/external_polymorphism_dp.png)