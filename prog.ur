def driverProg():
  HOSTNAME = "192.168.1.101"
  PORT = 50001
  MSG_OUT = 1
  MSG_QUIT = 2
  MSG_JOINT_STATES = 3
  MSG_MOVEJ = 4
  MSG_WAYPOINT_FINISHED = 5
  MSG_STOPJ = 6
  MSG_SERVOJ = 7
  MSG_SET_PAYLOAD = 8
  MSG_WRENCH = 9
  MSG_SET_DIGITAL_OUT = 10
  MSG_GET_IO = 11
  MSG_SET_FLAG = 12
  MSG_SET_TOOL_VOLTAGE = 13
  MSG_SET_ANALOG_OUT = 14
  MULT_wrench = 10000.0
  MULT_payload = 1000.0
  MULT_jointstate = 10000.0
  MULT_time = 1000000.0
  MULT_blend = 1000.0
  MULT_analog = 1000000.0
  pi = 3.14159265359
  
  MSG_VELJ=20
  
  def send_out(msg):
    enter_critical
    socket_send_int(MSG_OUT)
    socket_send_string(msg)
    socket_send_string("~")
    exit_critical
  end
  
  def send_waypoint_finished(waypoint_id):
    enter_critical
    socket_send_int(MSG_WAYPOINT_FINISHED)
    socket_send_int(waypoint_id)
    exit_critical
  end

  
  SERVO_IDLE = 0
  SERVO_RUNNING = 1
  cmd_servo_state = SERVO_IDLE
  cmd_servo_id = 0  # 0 = idle, -1 = stop
  cmd_servo_q = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
  cmd_servo_dt = 0.0
  def set_servo_setpoint(id, q, dt):
    enter_critical
    cmd_servo_state = SERVO_RUNNING
    cmd_servo_id = id
    cmd_servo_q = q
    cmd_servo_dt = dt
    exit_critical
  end
  thread servoThread():
    state = SERVO_IDLE
    while True:
      # Latches the new command
      enter_critical
      q = cmd_servo_q
      dt = cmd_servo_dt
      id = cmd_servo_id
      do_brake = False
      if (state == SERVO_RUNNING) and (cmd_servo_state == SERVO_IDLE):
        # No command pending
        do_brake = True
      end
      state = cmd_servo_state
      cmd_servo_state = SERVO_IDLE
      exit_critical
      
      #do_brake = False
      
      # Executes the command
      if do_brake:
        #stopj(1.0)  # TODO
        send_out("Braking")
        sync()
      elif state == SERVO_RUNNING:
        servoj(q, 0, 0, dt)
        #send_out("Servoed")
      else:
        #send_out("Idle")
        sync()
      end
    end
  end
  ##new
  Speed_IDLE = 0
  Speed_RUNNING = 1
  cmd_Speed_state = Speed_IDLE
  cmd_Speed_a = 0  # 0 = idle, -1 = stop
  cmd_Speed_q = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
  cmd_Speed_dt = 0.0
  def set_Speed_setpoint(q,a, dt):
    
    enter_critical
    cmd_Speed_state = Speed_RUNNING
    cmd_Speed_a = a
    cmd_Speed_q = q
    cmd_Speed_dt = dt
    exit_critical
  end
  thread SpeedThread():
    state = Speed_IDLE
    while True:
      # Latches the new command
      enter_critical
      qd = cmd_Speed_q
      dt = cmd_Speed_dt
      a = cmd_Speed_a
      
      
      state = cmd_Speed_state
      cmd_Speed_state = Speed_IDLE
      exit_critical
      
      #do_brake = False
      #textmsg("here")
      # Executes the command
      if state == Speed_RUNNING:
        
        speedj(qd, a, dt)
        #send_out("Servoed")
      else:
        #send_out("Idle")
        sync()
      end
    end
  end
  ##end new
  
  
  while not socket_open(HOSTNAME, PORT):
    textmsg("waiting on socket open")
    sleep(5)
  end
      
  
  thread_servo = run servoThread()
  thread_speed = run SpeedThread()
  
  # Servoes in a circle
  #movej([1.5,-0.4,-1.57,0,0,0], 3, 0.75, 1.0)
  #t = 0
  #while True:
  #  q = [1.5,0,-1.57,0,0,0]
  #  q[0] = q[0] + 0.2 * sin(0.25 * t*(2*pi))
  #  q[1] = q[1] - 0.2 + 0.2 * cos(0.25 * t*(2*pi))
  #  #servoj(q, 3, 1, 0.08)
  #  #send_out("servoed")
  #  set_servo_setpoint(t, q, 0.08)
  #  t = t + 0.08
  #  sleep(0.08)
  #end
  
  while True:
    #send_out("Listening")
    ll = socket_read_binary_integer(1)
    
    if ll[0] == 0:
      #send_out("Received nothing")
    elif ll[0] > 1:
      send_out("Received too many things")
    else:
      mtype = ll[1]
      if mtype == MSG_QUIT:
        send_out("Received QUIT")
        break
      elif mtype == MSG_MOVEJ:
        send_out("Received movej")
        params_movej = socket_read_binary_integer(1+6+4)
        if params_movej[0] == 0:
          send_out("Received no parameters for movej message")
        end
        
        # Unpacks the parameters
        waypoint_id = params_movej[1]
        q = [params_movej[2] / MULT_jointstate,
                   params_movej[3] / MULT_jointstate,
                   params_movej[4] / MULT_jointstate,
                   params_movej[5] / MULT_jointstate,
                   params_movej[6] / MULT_jointstate,
                   params_movej[7] / MULT_jointstate]
        a = params_movej[8] / MULT_jointstate
        v = params_movej[9] / MULT_jointstate
        t = params_movej[10] / MULT_time
        r = params_movej[11] / MULT_blend
        
        # Sends the command
        send_out("movej started")
        movej(q, a, v, t, r)
        send_waypoint_finished(waypoint_id)
        send_out("movej finished")
      elif mtype == MSG_VELJ:
        params_velj = socket_read_binary_integer(8)
        if params_velj[0] != 8:
          textmsg("Received" ,params_velj[0])
        else:
          qd = [params_velj[1] / MULT_jointstate,
                    params_velj[2] / MULT_jointstate,
                    params_velj[3] / MULT_jointstate,
                    params_velj[4] / MULT_jointstate,
                    params_velj[5] / MULT_jointstate,
                    params_velj[6] / MULT_jointstate]
          a = params_velj[7] / MULT_jointstate
          t = params_velj[8] / MULT_time
          set_Speed_setpoint(qd,a, t)
        end

      elif mtype == MSG_SERVOJ:
        # Reads the parameters
        params_servoj = socket_read_binary_integer(1+6+1)
        if params_servoj[0] == 0:
          send_out("Received no parameters for movej message")
        end
        
        # Unpacks the parameters
        waypoint_id = params_servoj[1]
        q = [params_servoj[2] / MULT_jointstate,
                   params_servoj[3] / MULT_jointstate,
                   params_servoj[4] / MULT_jointstate,
                   params_servoj[5] / MULT_jointstate,
                   params_servoj[6] / MULT_jointstate,
                   params_servoj[7] / MULT_jointstate]
        t = params_servoj[8] / MULT_time
        
        # Servos
        #servoj(q, 3, 0.1, t)
        #send_waypoint_finished(waypoint_id)
        set_servo_setpoint(waypoint_id, q, t)
      elif mtype == MSG_SET_PAYLOAD:
        params = socket_read_binary_integer(1)
        if params[0] == 0:
          send_out("Received no parameters for setPayload message")
        end
        
        payload = params[1] / MULT_payload
        #send_out(payload)
        send_out("Received new payload")
        set_payload(payload)
      elif mtype == MSG_STOPJ:
        send_out("Received stopj")
        stopj(1.0)
      elif mtype == MSG_SET_DIGITAL_OUT:
        #send_out("Received Digital Out Signal")
        # Reads the parameters
        params_sdo = socket_read_binary_integer(2)
        if params_sdo[0] == 0:
          send_out("Received no parameters for set_digital_out message")
        end
        if params_sdo[2] > 0:
           set_digital_out(params_sdo[1], True)
        elif params_sdo[2] == 0:
          set_digital_out(params_sdo[1], False)
        end
      elif mtype == MSG_SET_FLAG:
        #send_out("Received Set Flag Signal")
        # Reads the parameters
        params_sf = socket_read_binary_integer(2)
        if params_sf[0] == 0:
          send_out("Received no parameters for set_flag message")
        end
        if params_sf[2] != 0:
          set_flag(params_sf[1], True)
        elif params_sf[2] == 0:
          set_flag(params_sf[1], False)
        end
      elif mtype == MSG_SET_ANALOG_OUT:
        #send_out("Received Analog Out Signal")
        # Reads the parameters
        params_sao = socket_read_binary_integer(2)
        if params_sao[0] == 0:
          send_out("Received no parameters for set_analog_out message")
        end
        set_analog_out(params_sao[1], (params_sao[2] / MULT_analog))
      elif mtype == MSG_SET_TOOL_VOLTAGE:
        #send_out("Received Tool Voltage Signal")
        # Reads the parameters (also reads second dummy '0' integer)
        params_stv = socket_read_binary_integer(2)
        if params_stv[0] == 0:
          send_out("Received no parameters for set_tool_voltage message")
        end
        set_tool_voltage(params_stv[1])
      else:
        send_out("Received unknown message type")
      end
    end
  end
  
  socket_send_int(MSG_QUIT)
end
