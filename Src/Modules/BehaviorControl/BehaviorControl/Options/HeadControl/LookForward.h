/** Pun intended. Looks straight ahead in a way that the Nao V4's cameras cover the area in front of its feet as well as everything else in front of the robot.*/
option(LookForward, (float)(0.f) pan, (float)(0.34f) tilt)
{
  /** Simply sets the necessary angles */
  initial_state(lookForward)
  {
    action
    {
        /* > Pan is from side to side. As a result of this the +0.90 degres is to the left of
         * the robot, while -0.90 is to the right.
         * > Tilt is from up and down. This time the + is for the down ward movement of the 
         * robots head while the - moves the robot head upwards.
         * > The last parameter is the speed that the robot is asked to do the head movement.
        */
        SetHeadPanTilt(pan, tilt ,150_deg);
    }
  }
}
