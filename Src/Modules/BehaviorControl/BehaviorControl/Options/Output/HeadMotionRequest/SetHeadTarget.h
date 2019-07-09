/** Sets all members of the HeadMotionRequest representation for positioning the robot's head pointing towards a given point relative to the robot's hip. */
//option(SetHeadTarget, (const Vector3f&) target, (HeadMotionRequest::CameraControlMode)(HeadMotionRequest::autoCamera) camera, (float)(pi) speed)
option(SetHeadTarget, (const Vector3f&) target, (float)(pi) speed, (HeadMotionRequest::CameraControlMode)(HeadMotionRequest::autoCamera) camera)
{
  /** Set the head motion request. */
  initial_state(setRequest)
  {
    action
    {
      theHeadMotionRequest.mode = HeadMotionRequest::targetMode;
      theHeadMotionRequest.cameraControlMode = camera;
      theHeadMotionRequest.target = target;
      theHeadMotionRequest.speed = speed;
    }
  }
}
