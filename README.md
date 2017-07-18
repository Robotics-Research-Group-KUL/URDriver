# URDriver
orocos components for interfacing with UR robots

## usage

set the pc address in the same subnet of the robot and set such IPs in properties
```lua
    ros:import("URDriver")
    timefreq=0.008
    
    depl:loadComponent("URDriverRT_receiver", "URDriverRT_receiver")
    URDriverRT_receiver=depl:getPeer("URDriverRT_receiver")
    
    depl:loadComponent("URDriver_program", "URDriver_program")
    URDriver_program=depl:getPeer("URDriver_program")
    
    URDriver_program:setPeriod(timefreq)			 
    URDriver_program:getProperty("robot_address"):set("192.168.1.102")--ip robot
    URDriver_program:getProperty("my_address"):set("192.168.1.101")
    URDriverRT_receiver:getProperty("robot_address"):set("192.168.1.102")
    URDriver_program:getProperty("timeOut"):set(timefreq)
```
     
For receiving the data from the robot, it suffices to:
```lua
    if not URDriverRT_receiver:configure() then
        print("failed to conf URDriverRT_receiver")
    end
    if not URDriverRT_receiver:start()then
        print("failed to start")
    end
```
For the component that sends the velocities:
```lua
    if not URDriver_program:configure() then
      print("failed to conf URDriver_program")
    end
      print(">> send-program")
    if not URDriver_program:send_program()then
      print("failed to send-program")
    end
      print(">> open-server")
    if not URDriver_program:open_server()then
      print("failed to open-server")
    end
    if not URDriver_program:start()then
      print("failed to start URDriver_program")
    end
 ```   
Then, when the controller is up, and the velocities are ready in ```qdes_inport``` to be commanded, run
```lua
    URDriver_program:start_send_velocity()
```
The command 
```lua
    URDriver_program:stop_send_velocity()
```   
Stops the robot.

to test velocity control, write some value in the control
```lua
    qdd=rtt.Variable("array")
    qdd:fromtab({0.1,0,0,0,0,0})
    qqdport = rttlib.port_clone_conn(URDriver_program:getPort("qdes_inport"))

    qqdport:write(qdd)

```

it is possible to direcly command joint position
```lua
    pi=3.14
    time=10.0
    qdes=rtt.Variable("array")
    qdes:fromtab{pi,pi/2,pi,pi,pi,pi/2}
    URDriver_program:send_joint_objective(qdes,time)
```   
## structure 
There are 3 components:

- **URDriver_program**: send the program, and at the moment commands velocity and joint positions. Functionalies can be expanded should be run at 8ms
- **URDriverRT_receiver**: recieve the data every 8ms _it uses the file descriptor activity, so it does it should be aperiodic, as it is triggered when new data arrives_
- **URDriver_receiver**: (component not compiled because largely not finished, not useful for control) recieve the data every 100ms _it also uses the file descriptor_.

The data structures depends by the firmware (V3.0-3.1 and Pre-3.0 implemented), and affects the receivers, I think that the program should work for several versions (tested on 3.1 and 1.8).

*Note*:  for V1.8, there is a property in the URDriverRT_receiver that must be set:
```lua
    URDriverRT_receiver:getProperty("version_interface"):set("Pre-3.0")
```

The classes for reading the data from socket are all c++ (e.g.[RTDeserealize.hpp](include/URDriver/RTDeserialize.hpp) )

There is a virtual class (e.g. ```RTdata```) that is implemented depending by the firmware (```RTdataV31``` or ```RTdataV18```).
There are several getter functions, that are implemented in the base class, they return ```-1``` if not reimplemented in the child class.
The value is returned by reference.
