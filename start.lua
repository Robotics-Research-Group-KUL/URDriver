
require "rttlib"
require "rttros"


----------------------
-- get the deployer --
tc=rtt.getTC()
if tc:getName() == "lua" then
  depl=tc:getPeer("Deployer")
elseif tc:getName() == "Deployer" then
  depl=tc
end


rtt.setLogLevel("Warning")

--[ get convenience objects ]--
gs = gs or rtt.provides()
--tc = tc or rtt.getTC()
--depl = depl or tc:getPeer("Deployer")

--[ required imports ]--
depl:import("rtt_ros")

ros = gs:provides("ros")


ros:import("URDriver")

--rtt.setLogLevel("Info")

depl:loadComponent("URDriverRT_receiver", "URDriverRT_receiver")
URDriverRT_receiver=depl:getPeer("URDriverRT_receiver")
depl:loadComponent("URDriver_receiver", "URDriver_receiver")
URDriver_receiver=depl:getPeer("URDriver_receiver")
depl:loadComponent("URDriver_program", "URDriver_program")
URDriver_program=depl:getPeer("URDriver_program")

--URDriverRT_receiver:setPeriod(0.008)

--URDriver_receiver:setPeriod(0.1)

URDriver_program:setPeriod(0.008)

--[[
--for real robot
URDriver_program:getProperty("robot_adress"):set("192.168.1.102")
URDriver_program:getProperty("my_adress"):set("192.168.1.101")
URDriverRT_receiver:getProperty("robot_adress"):set("192.168.1.102")
]]--

-- for ur sim on the same robot
URDriver_program:getProperty("robot_address"):set("127.0.0.1")
URDriver_program:getProperty("my_address"):set("127.0.0.1")
URDriverRT_receiver:getProperty("robot_address"):set("127.0.0.1")



if not URDriverRT_receiver:configure() then
  print("failed to conf URDriverRT_receiver")
end


--[[
if not URDriver_receiver:configure() then
  print("failed to conf URDriver_receiver")
end
]]--
if not URDriverRT_receiver:start()then
  print("failed to start")
end

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
--[[
cd URDriver_program
var array q(6)
q[1]=-1.57
send_joint_objective (q,10)


]]--
