
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


depl:loadComponent("URDriverRT_dummy_server", "URDriverRT_dummy_server")
URDriverRT_dummy_server=depl:getPeer("URDriverRT_dummy_server")




URDriverRT_dummy_server:setPeriod(0.85)




if not URDriverRT_dummy_server:configure() then
  print("failed to conf URDriverRT_dummy_server")
end

if not URDriverRT_dummy_server:start() then
  print("failed to start URDriverRT_dummy_server")
end
