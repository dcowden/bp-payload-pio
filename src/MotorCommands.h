#ifndef MotorCommands_h
#define MotorCommands_h

const byte MOTOR_CONTROLLER_ADDRESS = 42;

struct MotorCommand{
  float leftVelocity;
  float rightVelocity;
  boolean enabled;
};

#endif
