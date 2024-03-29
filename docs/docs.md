\tableofcontents

# Introduction

This project provides tools for interdisciplinary analysis of fine-scaled pedestrian movement, especially for science-based decision-making. It is motivated by the fact that local movement and interaction patterns of individuals congregated in public locations, such as entertainment venues and transportation hubs, impacts public health in myriad ways. For instance, infectious disease transmission in crowded areas, such as the 2016 measles outbreak in Disney world that resulted in 125 cases, is affected by the evolution of the pedestrian contact network.

In such contexts, mathematical models can be used to explore different “what if” scenarios for planning public health interventions. For example, pedestrian mobility models could help in the design of public spaces and policies that reduce contacts to mitigate disease spread or encourage walking to improve health outcomes. Understanding the fine-scale movement and interaction patterns of people can help design effective policies and spatial layouts to better engineer suitable movement and interaction patterns for improved public health outcomes in several domains.

Pedestrian dynamics enables such analysis by simulating the trajectories of individual pedestrians in a crowd. Such movement is impacted by behavioral characteristics of humans, policy choices of decisions makers, and design decisions regarding the built environment. Furthermore, the impact of pedestrian dynamics is governed by application-domain models, such as infection spread models. Thus, input from a variety of science domains is required to produce comprehensive understanding for science-based decision-making.

This project includes a modular pedestrian dynamics code to which input from different domains could be easily included. In addition, it includes tools to democratize use by users with limited computing background by including a domain specific language to specify human behavior and policy. We also separately provide a recommender system, as a complementary project, to suggested suitable models to use.

# Usage

VIPRA can either be used as a [Library](#library-build) for use in other projects or built as a [Standalone Simulation](#standalone-build).

There is an [Example Simulation](#example-simulation) below.

## Standalone Build

The `VIPRA` repository comes with a ready `main.cpp` that follows the [Example Simulation](#example-simulation) below.

To build the simulation, in the parent directory run:
```
make
```

The example simulation provided can be run with:
```
./VIPRA
```

## Library

`VIPRA` can be included in a project using [CMake](https://cmake.org/)

```
target_link_libraries(
  **project**
  PUBLIC
  VIPRA
)
```

- Replace `**project**` with the name of your CMake project


## Simulation Construction

Simulations are created by choosing which implementations to use for the several different [Module Types](#module-types).

A utility function for creating simulations is provided that allows the modules to be passed in in any order:
```C++
template <typename... module_ts>
auto VIPRA::simulation(module_ts&&...) -> VIPRA::SimType;
```

### Required Modules

Each simulation requires the following modules:
- `Model`
- `Goals`
- `Pedestrian Set`
- `Map`
  - `Obstacle Set`
  - `Polygon Input`
- `Output Coordinator`
  - `Output` (Can have any number of Output modules)

`Maps` and `Output Coordinators` are special in that they require modules themselves (denoted by the additional indent)

---

## Example Simulation

Below is an example main that uses some of the module implementations that are shipped with VIPRA.

```C++
#include <vipra.hpp>

#include "modules/model/calm_model/calm_model.hpp"

auto main() -> int {
  // Create the simulation, modules can be placed in any order so long as all are provided/valid
  auto sim = VIPRA::simulation(
      CALM::Model{},                          // Pedestrian Dynamics Model
      VIPRA::Goals::AStar{},                  // Goals Module
      VIPRA::Pedestrians::Grid{               // Pedestrian Set Module
          VIPRA::Input::JSON{"maps/pedestrians/a320/a320_144_pedestrians.json"}, // Input Module
      },
      VIPRA::Module::Output{                  // Output Coordinator
        VIPRA::Output::Trajectories::JSON{}   // Output Module
      },
      VIPRA::Module::Map{                     // Map Coordinator
        VIPRA::Input::JSON{"maps/obstacles/a320/a320_polygons.json"}, // Input Module (Polygon Input)
        VIPRA::Obstacles::QuadTree{}          // Obstacle Set Module
      }
  );

  // Run the simulation, passing in parameters
  sim(
    VIPRA::Parameters{
      VIPRA::Input::JSON{"module_params.json"}
    }
  );
}
```

## Example Parameter Sweep

This example uses the parameter sweep engine.

It takes in the total number of simulations to run as a command line argument.

```C++
#include <vipra.hpp>

#include "modules/model/calm_model/calm_model.hpp"

auto main(int argc, char** argv) -> int {
  VIPRA::ParameterSweep::initialize(argc, argv);  // Initialize the parameter sweep engine

  if (argc != 2) {      // Read in the command line arguments for the number of simulations to run
    std::cerr << "Usage: " << argv[0] << " <simCount>" << std::endl;
    return 1;
  }
  size_t simCount = std::stoul(argv[1]);

  auto sim = VIPRA::simulation(     // Create the simulation
      CALM::Model{}, 
      VIPRA::Goals::AStar{},
      VIPRA::Pedestrians::Grid{
          VIPRA::Input::JSON{"maps/pedestrians/a320/a320_144_pedestrians.json"},
      },
      VIPRA::Module::Output{VIPRA::Output::Trajectories::JSON{}},
      VIPRA::Module::Map{
        VIPRA::Input::JSON{"maps/obstacles/a320/a320_polygons.json"},
        VIPRA::Obstacles::QuadTree{}
      });

  VIPRA::ParameterSweep::run(    // Run a parameter sweep
      sim,                       // Simulation to run
      VIPRA::Parameters{VIPRA::Input::JSON{"module_params.json"}}, // Module Parameters
      simCount,                  // Number of simulations
      []() { std::cout << "Simulation complete on: " << VIPRA::ParameterSweep::get_rank() << std::endl; } // Optional: callback for each simulation run
  );
}
```

## Module Parameters

Parameters are loaded into each module before they are initizalized. The exact steps of a simulation run are shown in [`Simluation Steps`](#simulation-steps).

Parameters can use any [`Parameter Input`](#parameter-input-implementation) module, the [`JSON`](#json-input) input module provided works for this.

The format for JSON parameters is:
```JSON
{
  "module type": {
    "module name": {
      "parameter name": "parameter value"
    }
  },
  ...
}
```

Example of a [`Goals`](#goals-implementation) module parameters:
```JSON
{
  ...
  "goals": {                  // Module Type
    "astar": {                // Module Name
      "endGoalType": "exit",  // Parameter values
      "gridSize": 0.1,
      "closestObstacle": 0.25,
      "goalRange": 0.05
    }
  },
  ...
}
```
---

## Included Modules

There are several modules included with VIPRA.

**Model**:
- [`Calm Model`](#calm-model)

**Goals**:
- [`AStar`](#astar-goals)

**Obstacle Set**:
- [`QuadTree`](#quadtree-obstalce-set)

**Pedestrian Set**:
- [`Grid`](#grid-pedestrian-set)

**Input**:
- [`JSON`](#json-input)

**Output**:
- [`JSON`](#json-output)

---

### Calm Model

This `Model` module implements the [Calm Model](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0229690)

#### Use:
```C++
#include "modules/model/calm_model/calm_model.hpp"

VIPRA::simulation(
  ...
  CALM::Model{},
  ...
);
```

#### Parameters:
- `meanMass` : Mean mass for population
- `massStdDev` : Standard Deviation for mass
- `meanReactionTime` : Mean reaction time for population
- `reactionTimeStdDev` : Standard Deviation for reaction time
- `meanMaxSpeed` : Mean max speed for population
- `maxSpeedStdDev` : Standard deviation for max speed
- `meanShoulderLen` : Mean shoulder length for population
- `shoulderLenStdDev` : Standard deviation for shoulder length

### AStar Goals

This `Goals` module uses the [A* algorithm](#astar-algorithm) to find paths for each pedestrian.

#### Use:
```C++
#include "vipra.hpp"

VIPRA::simulation(
  ...
  VIPRA::Goals::AStar{},
  ...
);
```

#### Parameters:
- `endGoalType` : Name of object in map each pedestrian tries to reach (e.g. "exit")
- `goalRange` : Range, in meters, before a goal is counted as "reached" (e.g. 0.05)
- `gridSize` : Length of each side, in meters, of each grid in the pathing graph (e.g. 0.1)
- `closestObstacle` : Closest a grid center can be to an obstacle before being considered non-traversable

### QuadTree Obstacle Set 

This `Obstacle Set` module uses the [Quad Tree](#quadtree-datastructure) to hold the map geometry.

#### Use:
```C++
#include "vipra.hpp"

VIPRA::simulation(
  ...
  VIPRA::Module::Map{
    VIPRA::Input::JSON{"filepath"} // Map requires an Input module
    VIPRA::Obstacles::QuadTree{} // Obstacle sets go inside the Map module
  }
  ...
)
```

#### Parameters:

- `minQuadSize` : Size at which quads stop being subdivided

### Grid Pedestrian Set

This `Pedestrian Set` module holds pedestrians in a grid of cells, allowing for more efficient lookup of nearest neighbors

#### Use:
```C++
#include "vipra.hpp"

VIPRA::simulation(
  ...
  VIPRA::Pedestrians::Grid{},
  ...
);
```

#### Parameters:

- `gridSize`: Size of each grid cell

### JSON Input

This `Input` module loads JSON data, using [nlohmann JSON](https://github.com/nlohmann/json).
- Qualifies as a:
  - Polygon Input
  - Parameter Input

#### Use:

```C++
#include "vipra.hpp"

VIPRA::simulation(
  ...
  VIPRA::Module::Map {
    VIPRA::Input::JSON{"filepath"} // Input modules are usually used in other modules
    ...
  }
);

sim(VIPRA::Parameters{
  VIPRA::Input::JSON{"filepath"} // Or for loading parameters
});
```

#### Parameters:

- NONE

### JSON Output

This `Output` module creates a JSON file containing the positions of each pedestrian for each timestep that matches the output frequency in the simulation module parameters.

#### Use:

```C++
VIPRA::simulation(
  ...
  VIPRA::Module::Output{
    VIPRA::Output::Trajectories::JSON{}
  },
  ...
)
```

#### Parameters:

- `filename`: Name of the output file

---

# Common Errors

<!-- TODO -->

---

# Development

# Overview

<!-- TODO -->

## Simulation

### Simulation Steps

These are the general steps taken when a simulation is run.

#### 1. VIPRA::simulation

First a Simulation is constructed using the `VIPRA::simulation` utility method.

#### 2. Run Sim

The simulation is started using either `operator()` or `run_sim()`.

These both take a [`Parameter Module`](#parameter-module) as their argument.

#### 3. Parameters Loaded

The [`Parameter Module`](#parameter-module) calls `load()` on it's [`Parameter Input`](#parameter-input-implementation) module.

#### 4. Parameters Registered

Each Module has its `register_params()` method called.

This is used to tell the simulation which parameters are required by each module.

#### 5. Modules Configured

Each Module has its `config()` method called.

The loaded parameters are passed in to each module.


#### 6. Modules Initialized

The [`Map`](#map-module), [`Goals`](#goals-implementation), [`Model`](#model-implementation), and [`Behavior Model`](#behaviors) have their `initialize()` methods called.

This allows each module to setup any necessary pre-simulation work.

#### 7. Simulation Run

The following is run until the [`Goals`](#goals-implementation) module simulation goal is met, or until the simulation `max_timestep` is reached.

```
model.timestep();
behaviors.timestep();
pedestrian_set.update();
goals.update();
```

#### 8. Output

Finally, each of the [`Output`](#output-implementation) modules `write()` method is called.

## Module Types

**Model**

[`Model`](#model-implementation) modules are the pedestrian dynamics models that govern how the pedestrians will move during a simulation

**Goals**

[`Goals`](#goals-implementation) modules handle pathfinding for the simulation

**Obstacle Set**

[`Obstacle Set`](#obstacle-set-implementation) modules hold the geometry of the environment and handle map/pedestrian collision detection

**Pedestrian Set**

[`Pedestrian Set`](#pedestrian-set-implementation) modules hold the pedestrian positions/velocities and handle queries to pedestrians (ex. nearest neighbor)

**Input**

[`Input`](#input-implementation) modules handle loading input for the simulation. There are several specialized input types:
- [`Serializable Input`](#serializable-input-implementation) modules are inputs that allow for serialization of their values
- [`Parameter Input`](#parameter-input-implementation) modules are able to load parameters
- [`Polygon Input`](#polygon-input-implementation) modules are able to load polygons for use in obstacle sets

**Output**

[`Output`](#output-implementation) modules handle producing output from the simulation

**Map**

The [`Map`](#map) module is a coordinator, it accepts one [`Obstacle Set`](#obstacle-set-implementation) and one [`Polygon Input`](#polygon-input-implementation).

**Output Coordinator**

The [`Output Coordinator`](#output-coordinator) module handles coordinating any number of [`Output`](#output-implementation) modules.

## Parameter Sweep

<!-- TODO: Add in parameter sweep stuff -->

## Randomization

<!-- TODO: add in about how randomization should be handled -->

## VIPRA Types

<!-- TODO: add in about VIPRA specific types -->

# Module Implementation

> !!! Important Notes:
> - These modules are implemented using templates; this means that any method that accepts a module as a parameter is required to be defined in a header file
> - Every module has helper macros for their required methods, see [Macros](#macros)
> - For these Interfaces, assume `using namespace VIPRA::Concepts`

# Base Module Implementation

Each module is required to implement the following:

### Interface

```C++
void register_params(Params auto&);
void config(Params auto const&);
```

Every Module also requires a name and type tag:

```C++
VIPRA_MODULE_NAME("**name**")
VIPRA_MODULE_TYPE(**type**)
```

- Replace `**name**` with the name for the module; this will be used to pull the correct parameters from input.
- Replace `**type**` with the type of module the class is; available modules are below:

1. `MODEL` - [Model](#model)
2. `GOALS` - [Goals](#goals)
3. `PEDESTRIANS` - [Pedestrian Set](#pedestrian-set)
4. `OBSTACLES` - [Obstacle Set](#obstacle-set)
5. `INPUT` - [Input](#input)
6. `OUTPUT` - [Output](#output)

# Model Implementation

`Model` modules are the pedestrian dynamics models that govern how the pedestrians will move during a simulation

### Interface

Along with the [Base Module](#base-module-implementation) interface, Model modules are required to implement the following methods.

Helper macros can be found in [Model Macros](#model-macros)

```C++
void initialize(PedsetModule auto const&, MapModule auto const&, GoalsModule auto const&, OutputModule auto&);

void timestep(PedsetModule auto const&, MapModule auto const&, GoalsModule auto const&, OutputModule auto&, VIPRA::State&, VIPRA::delta_t, VIPRA::timestep);
```

# Goals Implementation

`Goals` modules handle pathfinding for the simulation.

### Interface

Along with the [Base Module](#base-module-implementation) interface, Goals modules are required to implement the following methods.

Helper macros can be found in [Goals Macros](#goals-macros)

```C++
void initialize(PedSetModule auto const&, MapModule auto const&);
void update(PedSetModule auto const&, MapModule auto const&, VIPRA::delta_t);
void change_end_goal(VIPRA::idx, VIPRA::f3d);

auto current_goals() -> std::vector<VIPRA::f3d> const&;
auto end_goals() -> std::vector<VIPRA::f3d> const&;
auto current_goal(VIPRA::idx) -> VIPRA::f3d const&;
auto end_goal(VIPRA::idx) -> VIPRA::f3d const&;
auto is_goal_met(VIPRA::idx) -> bool;
auto is_sim_goal_met() -> bool;
auto time_since_last_goal(VIPRA::idx) -> VIPRA::f_pnt;
```

# Obstacle Set Implementation

Obstacle Set modules hold the geometry of the environment and handle map/pedestrian collision detection.

### Interface

Along with the [Base Module](#base-module-implementation) interface, Obstacle Set modules are required to implement the following methods.

Helper macros can be found in [Obstacle Set Macros](#obstacle-set-macros)

```C++
void initialize(std::vector<VIPRA::Geometry::Polygon> const&, std::vector<std::string> const&, std::map<std::string, std::vector<VIPRA::f3d>> const&);

auto get_object_types() -> std::vector<std::string> const&;
auto get_objects(std::string const&) -> std::vector<VIPRA::f3d> const&;

auto collision(VIPRA:f3d) -> bool;
auto collision(VIPRA::Geometry::Circle) -> bool;
auto ray_hit(VIPRA::f3d, VIPRA::f3d) -> VIPRA::f_pnt;

auto get_dimensions() -> VIPRA::f3d;
```

# Pedestrian Set Implementation

### Interface

Along with the [Base Module](#base-module-implementation) interface, Obstacle Set modules are required to implement the following methods.

Helper macros can be found in [Pedestrian Set Macros](#pedestrian-set-macros)

```C++
auto num_pedestrians() -> VIPRA::size;

auto ped_coords(VIPRA::idx) -> VIPRA::f3d;
auto all_coords() -> std::vector<VIPRA::f3d> const&;
auto ped_velocity(VIPRA::idx) -> VIPRA::f3d;
auto all_velocities() -> std::vector<VIPRA::f3d> const&;

void update(VIPRA::State const&);
```

# Input Implementation

> !!! Important Note: Inputs should hold off on loading their input until their `load` method is called. This reduces the amount of I/O during parallel runs.

### Interface

Along with the [Base Module](#base-module-implementation) interface, Input modules are required to implement the following methods.

```C++

void load();

template <typename data_t, typename... key_ts>
auto get(key_ts const&...) -> data_t;
```

#### Serializable Input Implementation

Serializable Input modules additionally require:

```C++
auto serialize() -> std::string;
void deserialize(std::string const&);
```

#### Parameter Input Implementation

Parameter Input modules additionally require:

```C++
template <typename data_t, typename... key_ts>
auto get<VIPRA::Parameter<data_t>>(key_ts const&...) -> std::optional<VIPRA::Parameter<data_t>>;
```

<!-- TODO: add in how the values should be handled -->

#### Polygon Input Implementation

Polygon Input modules additionally require:

```C++
template <typename... key_ts>
auto load_polygons(key_ts const&...) -> std::optional<std::vector<VIPRA::Geometry::Polygon>>;
```

# Output Implementation

### Interface

Along with the [Base Module](#base-module-implementation) interface, Output modules are required to implement the following methods.

Helper macros can be found in [Output Macros](#output-macros)

```C++
auto write();

template <typename value_t>
void timestep_value(std::string const&, VIPRA::timestep, value_t);

template <typename value_t>
void ped_timestep_value(VIPRA::idx, VIPRA::timestep, std::string const&, value_t);
```
---

# Model Implementation Example

In this example we will walk through creating a [`Model`](#model-implementation) module.
This Model will simply have pedestrians walking in circles around their initial position.

1. Create Folder and Files
  We will create the following: 
  - `modules/model/example_model`
  - `modules/model/example_model/example_model.hpp`

2. Create the model
```C++

#include <vipra.hpp>

namespace Example {
class Model {
 public:
  // Give the module a name and set its type
  VIPRA_MODULE_NAME("example_model")
  VIPRA_MODULE_TYPE(MODEL)

  // Add in the parameter registration step, this is where we tell VIPRA what parameters the module needs
  VIPRA_REGISTER_STEP { 
    VIPRA_REGISTER_PARAM("radius");
  }

  // Add in the configuration step, this is where the module parameters are loaded in
  VIPRA_CONFIG_STEP { 
    VIPRA_GET_PARAM("radius", _radius); 
  }

  // Add in the initialization step, this is run right before the simulation starts
  VIPRA_MODEL_INIT_STEP { 
    _initialPoints = pedset.all_coords(); 
  }

  // Add in the timestep, this is where the model updates the pedestrians state
  VIPRA_MODEL_TIMESTEP {
    for (size_t i = 0; i < pedset.num_pedestrians(); ++i) {
      // Update the pedestrian's position, to make them move in a circle
      state.positions[i] = VIPRA::f3d{_initialPoints[i][0] + _radius * std::cos(deltaT * timestep),
                                      _initialPoints[i][1] + _radius * std::sin(deltaT * timestep)};
    }
  }

 private:
  VIPRA::f_pnt  _radius;
  VIPRA::f3dVec _initialPoints;
};
}  // namespace Example

```

3. Update `main.cpp` to use our new model

```C++

#include <vipra.hpp>

#include "modules/model/example_model/example_model.hpp"

auto main() -> int {
  auto sim = VIPRA::simulation(
      Example::Model{},       // Here is our model being passed in, the rest don't matter in this example
      VIPRA::Module::Output{
        VIPRA::Output::Trajectories::JSON{}
      },
      VIPRA::Goals::AStar{},
      VIPRA::Pedestrians::Grid{
          VIPRA::Input::JSON{"maps/pedestrians/a320/a320_144_pedestrians.json"},
      },
      VIPRA::Module::Map{
        VIPRA::Input::JSON{"maps/obstacles/a320/a320_polygons.json"},
        VIPRA::Obstacles::QuadTree{}
      }
  );

  sim(VIPRA::Parameters{VIPRA::Input::JSON{"./examples/module_params.json"}});
}
```

4. Update the `module_params.json`

```JSON
{
  "simulation": {
    "main": {
      "max_timestep": 10000,
      "timestep_size": 0.005,
      "output_frequency": 100,
      "random_seed": 12345
    }
  },
  "model": {     // Since our module is a model we put the parameters under the "model" field
    "example_model": {  // We add in a field with our module name
      "radius": 0.2     // Here is our radius parameter
    }
  },
  "goals": {
    "astar": {
      "endGoalType": "exit",
      "gridSize": 0.1,
      "closestObstacle": 0.25,
      "goalRange": 0.05
    }
  },
  "pedset": {
    "grid": {
      "gridSize": 0.5
    }
  },
  "obstacles": {
    "quad_tree": {
      "minQuadSize": 0.05
    }
  },
  "output": {
    "coordinator": {
      "output_dir": "./output"
    },
    "trajectories_json": {
      "filename": "trajectories.json"
    }
  },
  "behavior_model": {
    "main": {
      "behaviors_dir": "./behaviors",
      "behaviors": []
    }
  }
}
```

5. Compile Simulation

The simulation can be compiled by running
```
make
```

6. Run

Run the simulation with
```
./VIPRA
```

The output will be under `./output/trajectories.json`

---

# Macros

For reference on how to use these macros, you can look at `modules/model/calm_model/calm_model.hpp`

## Base Module Macros

#### VIPRA_MODULE_TYPE(type)

`VIPRA_MODULE_TYPE` sets the modules type
- MODEL,
- GOALS,
- INPUT,
- OUTPUT,
- PEDESTRIANS,
- PARAMETERS,
- MAP,
- OBSTACLES,

```C++
VIPRA_MODULE_TYPE(MODEL)
```

#### VIPRA_MODULE_NAME(name)

`VIPRA_MODULE_NAME` sets the modules name, the name is used to grab the correct parameters.

```C++
class ExampleModel {
  VIPRA_MODULE_NAME("example_model")
};
```

#### CHECK_MODULE(type, class)

`CHECK_MODULE` checks that the class conforms to the module interface.

```C++
class ExampleModel {
  ...
};

CHECK_MODULE(MODEL, ExampleModel)
```

#### VIPRA_REGISTER_STEP
#### VIPRA_REGISTER_PARAM

`VIPRA_REGISTER_STEP` provides the proper function signature for the parameter registration step.

`VIPRA_REGISTER_PARAM` registers a parameter in the parameter registration step.

```C++
class ExampleModel {
  public:
    VIPRA_REGISTER_STEP {
      VIPRA_REGISTER_PARAM("example_param")
    }
}
```

#### VIPRA_CONFIG_STEP
#### VIPRA_GET_PARAM
#### VIPRA_GET_ARRAY_PARAM

`VIPRA_CONFIG_STEP` provides the proper function signature for the module configuration step.

`VIPRA_GET_PARAM` loads the parameter into a variable.

`VIPRA_GET_ARRAY_PARAM` loads an array parameter into a vector.

```C++
class ExampleModel {
  public:
    VIPRA_CONFIG_STEP {
      VIPRA_GET_PARAM("meanMass", _meanMass);
      VIPRA_GET_ARRAY_PARAM("array", _arrayExample);
    }

  private:
    VIPRA::f_pnt _meanMass;
    std::vector<std::string> _arrayExample;
}
```

## Model Macros

#### VIPRA_MODEL_INIT_STEP

`VIPRA_MODEL_INIT_STEP` provides the proper function signature for the model initialization step.

It provides the parameters:
- pedset: the pedestrian set module
- map: the map module
- goals: the goals module
- output: the output module
- engine: randomization engine

```C++
class ExampleModel {
  public:
    VIPRA_MODEL_INIT_STEP {
      // initialize model values
    }
}
```

#### VIPRA_MODEL_TIMESTEP

`VIPRA_MODEL_TIMESTEP` provides the proper function signature for the model timestep step.

It provides the parameters:
- pedset: the pedestrian set module
- map: the map module
- goals: the goals module
- output: the output module
- state: the [`State`] object to update
- deltaT: the size of a timestep
- timestep: the current timestep number

```C++
class ExampleModel {
  public:
    VIPRA_MODEL_TIMESTEP {
      // calculate pedestrian movement
      // update state parameter
    }
};
```

## Goals Macros

#### VIPRA_GOALS_INIT_STEP

`VIPRA_GOALS_INIT_STEP` provides the proper function signature for the goals initialization step.

It provides the parameters:
- pedset: the pedestrian set module
- map: the map module

```C++
class ExampleGoals {
  public:
    VIPRA_GOALS_INIT_STEP {
      // initialize pedestrian paths
    }
};
```

# Algorithms

# Data Structures