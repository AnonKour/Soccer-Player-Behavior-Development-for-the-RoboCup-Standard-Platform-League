/** An option intended for mostly validation of the targets coordinates in order to correct any errors.
 * The time of a full rotation is now unknown.
 * */

option(LookAtTargetOnGroundAndBall, (const Vector3f&)(Vector3f::Zero()) target, (const Vector3f&)(Vector3f::Zero()) ballPosition)
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
			// Set the head facing the target on the ground, meaning global coordinates.

			SetHeadTargetOnGround(target, (HeadMotionRequest::CameraControlMode)(HeadMotionRequest::autoCamera) , 100_deg);
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
