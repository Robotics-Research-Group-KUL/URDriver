
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

rtt.setLogLevel("Info")

depl:loadComponent("URDriverRT_receiver", "URDriverRT_receiver")
URDriverRT_receiver=depl:getPeer("URDriverRT_receiver")
--URDriverRT_receiver:setPeriod(0.008)
URDriverRT_receiver:getProperty("prop_adress"):set("127.0.0.1")

if not URDriverRT_receiver:configure() then
  print("failed to conf URDriverRT_receiver")
end
print(URDriverRT_receiver)
if not URDriverRT_receiver:start() then
  print("failed to conf URDriverRT_start")
end
print(URDriverRT_receiver)
