/** An option intended for scanning purposes in order for the robot to determine 
 * several things regards its surroundings.
 * The time of a full rotation is now unknown.
 * */

option(LookLeftAndRight)
{
    /* Simply sets the necessary angles */

    initial_state(lookForward)
    {
        transition
        {
            if(state_time > 100 && !theHeadMotionEngineOutput.moving)//(state_time > 1000)// If the theHeadMotionRequest.pan == 0.f 
            {
                goto lookLeft;
            }
        }
        action
        {
            /* > Pan is from side to side. As a result of this the +0.90 degres is to the left of
            * the robot, while -0.90 is to the right.
            * > Tilt is from up and down. This time the + is for the down ward movement of the 
            * robots head while the - moves the robot head upwards.
            * > The last parameter is the speed that the robot is asked to do the head movement.
            */
            //SetHeadPanTilt(0.f, 0.38f ,150_deg);
            LookForward();
        }
    }
    state(lookLeft)
    {
        transition
        {
            if (state_time > 100 && !theHeadMotionEngineOutput.moving)//2000)
            {
                goto lookRight;
            }
        }
        action
        {
            /* > Pan is from side to side. As a result of this the +0.90 degres is to the left of
            * the robot, while -0.90 is to the right.
            * > Tilt is from up and down. This time the + is for the down ward movement of the 
            * robots head while the - moves the robot head upwards.
            * > The last parameter is the speed that the robot is asked to do the head movement.
            */
            SetHeadPanTilt(0.85f, 0.38f ,70_deg);
        }
    }
    state(lookRight)
    {
        transition
        {
            if(state_time > 100 && !theHeadMotionEngineOutput.moving)//2000)
            {
                goto lookLeft;
            }
        }
        action
        {
            /* > Pan is from side to side. As a result of this the +0.90 degres is to the left of
            * the robot, while -0.90 is to the right.
            * > Tilt is from up and down. This time the + is for the down ward movement of the 
            * robots head while the - moves the robot head upwards.
            * > The last parameter is the speed that the robot is asked to do the head movement.
            */
            SetHeadPanTilt(-0.85f, 0.38f ,70_deg);
        }
    }
}
