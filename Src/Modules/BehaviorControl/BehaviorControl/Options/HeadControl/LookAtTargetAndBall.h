/** An option intended for mostly validation of the targets coordinates in order to correct any errors.
 * The time of a full rotation is now unknown.
 * */

option(LookAtTargetAndBall, (const Vector3f&)(Vector3f::Zero()) target, (const Vector3f&)(Vector3f::Zero()) ballPosition)
{
    /* Simply sets the necessary angles */

    initial_state(lookAtTargetAndBall)
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
            if (state_time > 1000 && !theHeadMotionEngineOutput.moving)//2000)
            {
                goto lookBall;
            }
        }
        action
        {
			// Local coordinates.

			SetHeadTarget(target, 100_deg);
        }
    }

    state(lookBall)
    {
        transition
        {
            if(state_time > 1000 && !theHeadMotionEngineOutput.moving)//2000)
            {
                goto lookTarget;
            }
        }
        action
        {
			// Local coordinates.

			SetHeadTarget(ballPosition, 100_deg);
        }
    }
}
