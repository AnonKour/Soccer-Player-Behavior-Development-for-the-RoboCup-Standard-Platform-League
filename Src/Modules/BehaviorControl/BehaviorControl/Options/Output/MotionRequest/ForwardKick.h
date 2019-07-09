/**
 *  @param mirrored True or false if we want the kick to be mirror. In a way its a choice of the kick foot.
 */
option(ForwardKick, (bool)(false) mirrored)
{
	/** Set the motion request. */

	initial_state(setRequest)
	{
		transition
		{
			// When you are ready.

			if (theMotionInfo.motion == MotionRequest::kick)
			{
				goto requestIsExecuted;
			}
		}
		action
		{
			theMotionRequest.motion = MotionRequest::kick;
			theMotionRequest.kickRequest.kickMotionType = KickRequest::kickForward;
			theMotionRequest.kickRequest.mirror = mirrored;
			theMotionRequest.kickRequest.autoProceed = true;
			theMotionRequest.kickRequest.boost = false;
		}
	}
	
	/** The motion process has started executing the request. */

	target_state(requestIsExecuted)
	{
		transition
		{
			if (theMotionInfo.motion != MotionRequest::kick)
			{
				goto setRequest;
			}	
		}
		action
		{
			theMotionRequest.motion = MotionRequest::kick;
			theMotionRequest.kickRequest.kickMotionType = KickRequest::kickForward;
			theMotionRequest.kickRequest.mirror = mirrored;
			theMotionRequest.kickRequest.autoProceed = true;
			theMotionRequest.kickRequest.boost = false;
		}
	}
}
