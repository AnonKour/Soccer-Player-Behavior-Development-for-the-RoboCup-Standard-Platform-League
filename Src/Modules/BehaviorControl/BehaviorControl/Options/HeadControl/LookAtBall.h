/** An option intended for scanning purposes in order for the robot to determine 
 * several things regards its surroundings.
 * What if there is no target? Maybe it should look forward then?
 */
// (const Vector3f&) (Vector3f)
option(LookAtBall, (const Vector3f&)(Vector3f::Zero()) target, (float)(pi) speed)
{
    /* Simply sets the necessary anglVector3f::Zero()es 
    initial_state(lookForward)
    {
        transition
        {
            if(state_time > 200)
            {
                goto lookBall;
            }
        }

    }
    */
    initial_state(lookAtBall)
    {
        transition
        {
            if(target == Vector3f::Zero())
            {
                // We should do a look forward instead.
                OUTPUT_TEXT("(target == Vector3f::Zero() so look forward");

                LookForward();
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
            
            // Do we want to make the speed relative to the distance? There was some oscilation occuring during suddent movements. Also we need to add an threshold.
            
            // Need to get the current angle and the new one. 
            
            // SetHeadPanTilt(theBallModel.estimate.position.angle(), 0.34f ,135_deg);
            /*We can specify the absolute angels of the head joints (option SetHeadPanTilt), a position on the field (option SetHeadTargetOnGround),
            or a position relative to the robot (option SetHeadTarget). */

            // Changed the order of the functions variables.

            SetHeadTarget(target, 150_deg);
            //SetHeadTarget((target.x(),target.y(),0.f), 150_deg);
        }
    }
    
}