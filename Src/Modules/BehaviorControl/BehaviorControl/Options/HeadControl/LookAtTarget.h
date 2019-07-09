/** An option intended for scanning purposes in order for the robot to determine 
 * several things regards its surroundings.
 * The time of a full rotation is now unknown.
 * */

option(LookAtTarget, (const Vector3f&)(Vector3f::Zero()) target)
{
    /* Simply sets the necessary angles */

    initial_state(lookAtTarget)
    {
        transition
        {
            if(state_time > 100 && !theHeadMotionEngineOutput.moving)//(state_time > 1000)// If the theHeadMotionRequest.pan == 0.f 
            {
                goto lookTarget;
            }
        }
        action
        {
            LookForward();
        }
    }

    state(lookTarget)
    {
        transition
        {
        }
        action
        {
			// Local coordinates.

			SetHeadTarget(target, 100_deg);
        }
    }
}
