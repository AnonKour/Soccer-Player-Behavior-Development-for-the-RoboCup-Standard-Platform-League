/* A script made for TUC science day. 
 * It is combination of the stationary ball tracking and the active ball tracking.
 * Change between those two happens with the left bumper.
 * The touch sensor at the top of the nao works as kill switch.
 * This is the version 2 in which Nao also listens for falling and fallen states while on halt in order
 * to try to recover.
 * */
 
option(ScienceDaySoccerBehaviorV2)
{
	/** Initially, all robot joints are off until the chest button is pressed. */

	initial_state(playDead)
	{
		transition
		{
			if(action_done) // After standing up successfully.
			{
				if (SystemCall::getMode() == SystemCall::simulatedRobot)
				{
					// Don't wait for the button in SimRobot

					//goto trackTheBall;

					goto walkToBall;
				}

				OUTPUT_TEXT("Waiting for left bumper.");

				goto waitForBumper;
			}
			
		}
		action
		{
			// Look forward. Set the head control mode. This happens in paraller with the behavior control.

			LookForward();

			// We need to call head control in order for the mode to be used.
			
			Stand();
			
		}
	}
	
	// In this state the nao waits for the press of a bumper.
	
	state(waitForBumper)
	{
		transition
		{
			if(action_done) // left bumper button pressed and released
			{
				OUTPUT_TEXT("Tracking the ball.");
				goto trackTheBall;
			}
		}
		action
		{
			Activity(BehaviorStatus::unknown);
			
			ButtonPressedAndReleased(KeyStates::lFootLeft, 1000, 0);
		}
	}
	
	// This state acts as kill switch and its called in order to stop the robot.
	
	state(halt)
	{
		transition
		{
			if(theFallDownState.state == FallDownState::fallen)
			{
				OUTPUT_TEXT("Going to get up.");
				goto getUp;   
			}
			// Check whether the left bumper is being pressed in order to restart the behavior.
			
			OUTPUT_TEXT(action_done);
			
			if(action_done) // left bumper button pressed and released
			{
				goto trackTheBall;
			}
		}
		action
		{
			// Set the head control mode. 

			LookForward();
			
			Stand();
			
			Activity(BehaviorStatus::unknown);
			
			ButtonPressedAndReleased(KeyStates::lFootLeft, 1000, 0);
		}
	}
	 
	/* Turn in order to face the ball.
	* */
	state(trackTheBall)
	{
		transition
		{
			if(action_done)
			{
				OUTPUT_TEXT("Changing behavior");
				
				goto stand;
			}
			
			// Check if the ball has been seen recently.
        
			if(theLibCodeRelease.timeSinceBallWasSeen > 3000)//theBehaviorParameters.ballNotSeenTimeOut)
			{
				// If not we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBallStill;
			}
		}
		action
		{
			// Look at the ball so you will head correctly there. Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));
			
			ButtonPressedAndReleased(KeyStates::lFootLeft, 1000, 0);
		}
	}
	
	// In this state the robot stands up.
	
	state(stand)
	{
		transition
		{
            if(action_done)
            {
                // If you have reached the ball sit down. Can I track while sitted?
                
                OUTPUT_TEXT("Walking to ball.");
                goto walkToBall;
            }
        }
        action
        {
			// Set the head control mode.

			LookForward();

			Stand();
        }
	}
	
	// In this state the nao heads to the ball.
	
	state(walkToBall)
	{
		transition
		{
			// In order to stop.
			
			if(action_done) // When the head sensor is press/touched.
			{
				OUTPUT_TEXT("Halt");
				goto halt;
			}
			
            // Check if the ball has been seen recently.
            
            if(theLibCodeRelease.timeSinceBallWasSeen > 3000)//theBehaviorParameters.ballNotSeenTimeOut)
            {
                // If not, we need to relocate it.
                
                OUTPUT_TEXT("searchForBall");
                goto searchForBall;
            }
            
			if(theLibCodeRelease.between(theBallModel.estimate.position.x(),160,210) && (theLibCodeRelease.between(theBallModel.estimate.position.angle(),-10_deg,10_deg)))
            {
                OUTPUT_TEXT("In position to kick the ball. Kicking");
                goto kick;
            }
			
			/* Since the code is executed in a serial order if the angle of the robot and the ball isnt the expected
			   it wont go to kick. Instead it should go to align with ball in order to have the ball in front of it.
			*/
			
			if(theLibCodeRelease.between(theBallModel.estimate.position.x(),160,210))
            {
				OUTPUT_TEXT("In need to align with the ball. Aligning with ball.");
				goto alignWithBall;
			}
        }
        action
        {
            // Look at the ball so you will head correctly there. Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));
			
            // Also head towards the ball.

			WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), 0.45f, 0.f));
            
			// And also wait in case nao needs to be stopped.
			
			Activity(BehaviorStatus::unknown);
			
			ButtonPressedAndReleased(KeyStates::headMiddle, 1000, 0);
		}
    }
	
	// In this state the robot aligns in front of the ball.
	//TODO a nice addition would be to add different kicks based on the angle of the robot
	// but that should be done after we make new poses.
	
	state(alignWithBall)
	{
		transition
		{
			// In order to stop.
			
			if(action_done) // When the head sensor is press/touched.
			{
				OUTPUT_TEXT("Halt");
				goto halt;
			}
			
			// We are in this state in order to correct the angle. So if the angle is corrected return to the ball.
			
			if(theLibCodeRelease.between(theBallModel.estimate.position.angle(),-10_deg,10_deg)) 
			{
				OUTPUT_TEXT("Aligned with ball. Walking to ball.");
				
				goto walkToBall;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));
			
            // Also head towards the ball.

            //WalkToTarget(Pose2f(50.f, 50.f, 50.f), theBallModel.estimate.position);
			
			WalkAtRelativeSpeed(Pose2f(((theBallModel.estimate.position.angle())/(std::abs(theBallModel.estimate.position.angle()))) * 0.9f, 0.f, 0.f));
            
			// And also wait in case nao needs to be stopped.
			
			Activity(BehaviorStatus::unknown);
			
			ButtonPressedAndReleased(KeyStates::headMiddle, 1000, 0);
		}
	}
	
	// A default state where Nao searchs for the ball.

	state(searchForBallStill)
	{
		transition
		{
			if(action_done)
			{
				OUTPUT_TEXT("Changing behavior");
				
				goto stand;
			}
			
			// Could this be done otherwise? Like if the ball is in the frame?

			if(theLibCodeRelease.timeSinceBallWasSeen < 300) // This must be small.
			{
				OUTPUT_TEXT("Found the ball, tracking...");
				OUTPUT_TEXT("trackTheBall");

				goto trackTheBall;
			}
		}
		action
		{
			// Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

			ButtonPressedAndReleased(KeyStates::lFootLeft, 1000, 0);
		}
	}
	
	// A default state where Nao searchs for the ball.
    
    state(searchForBall)
    {
        transition
        {
			// In order to stop.
			
			if(action_done) // When the head sensor is press/touched.
			{
				OUTPUT_TEXT("Halt");
				goto halt;
			}
			
            // Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.
            if(theLibCodeRelease.timeSinceBallWasSeen < 300) // 300 is debatable maybe make it shorter.
            {
                OUTPUT_TEXT("Found the ball, heading there...");
                OUTPUT_TEXT("walkToBall");
                goto walkToBall;
            }
        }
        action
        {
			// Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

            WalkAtRelativeSpeed(Pose2f(0.8f, 0.1f, 0.f)); // last known position?
			
			// And also wait in case nao needs to be stopped.
			
			Activity(BehaviorStatus::unknown);
			
			ButtonPressedAndReleased(KeyStates::headMiddle, 1000, 0);
        }
    }
	
	// In this state the Nao kicks the ball with the foot specified.

	state(kick)
	{
		transition
		{
			if(state_time > 3000 || (action_done))//(state_time > 1000 && action_done))
			{
				OUTPUT_TEXT("Walking to ball.");
				goto walkToBall;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

		    LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));
		
			// Different type of kicks?

			InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(theBallModel.estimate.position.angle(), theBallModel.estimate.position.x() - 160.f, theBallModel.estimate.position.y() - 55.f));
		}
	}
	
	/** Get up from the carpet. */
	  
	state(getUp)
	{
		transition
		{
			if(theFallDownState.state != FallDownState::fallen)
			{
				goto halt;
			}
		}
		action
		{
			GetUp();
		}
	}
}
