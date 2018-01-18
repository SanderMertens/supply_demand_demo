# supply_demand_demo
Toy supplychain generator

This project implements a simple simulation of a factory to demonstrate various features of the corto framework. The demo shows how various features of the corto typesystem, like using enumerations, complex types and using units and ranges.

The project also demonstrates how business logic can be implemented in classes such that these classes can be used in a fully declarative way. This is done by implementing logic in so called "lifecycle hooks", which are special methods defined on classes that are invoked when class instances go through the various stages of its lifecycle.

The project also demonstrates how a web user interface can be added simply by defining two configuration objects in a JSON file, and where to store this JSON file so that the project will always be able to locate it on the filesystem.

A public demo of this project is running here: http://45.55.188.133:9090/?select=*&from=data

## Building and running the project
First ensure that corto is installed, by running:
```
curl https://corto.io/install-dev-src | sh
```

Then clone the project, using:
```
git clone https://github.com/SanderMertens/supply_demand_demo
```

After the project is cloned, run bake on the project:
```
cd supply_demand_demo
bake
```

Now you can run the laptop_factory executable, by doing:
```
laptop_factory/bin/x64-linux-debug/laptop_factory
```

When the application is running you can navigate to http://localhost:9090 to inspect the data of the project in realtime.
