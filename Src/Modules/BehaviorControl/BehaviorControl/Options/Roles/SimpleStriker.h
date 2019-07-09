// This is a behavior of a simple striker that kick the ball towards the enemy goal.

option(SimpleStriker)
{
	// Does a quick scan of the field in order to determine its position.
	
	initial_state(localize)
	{
		transition
		{
			// Wait a specific amount of time for the localization to be finish.

			// Its also supposed to run after the ready state as well. 

			if (state_time > 8000) // 4 seconds maybe and also something with action done? !! How to add searchForBall to actions so action is done can be triggered?
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBall;
			}
		}
			action
		{
			// Do a quick scan around the field. Does it need to move around itself for better accuracy? Not most likely

			LookLeftAndRight();

			Stand();
		}
	}

    // In this state the Nao heads to the ball.
    
    state(walkToBall)
    {
        transition
        {        
            // Check if the ball has been seen recently.
            
            if(theLibCodeRelease.timeSinceBallWasSeen > 5000)// Could use theBehaviorParameters.ballNotSeenTimeOut)
            {
                // If not, we need to relocate it.
                
                OUTPUT_TEXT("searchForBall");

                goto searchForBall;
            }
            
            // If you have reached the ball align with goal. But don't align if its moving. Try to follow the ball instead.
            
            if(theBallModel.estimate.position.x() < 250.f && (theBallModel.estimate.velocity.y() == 0 || theBallModel.estimate.velocity.y() == 0))
            {
				OUTPUT_TEXT("decideKick");

				goto decideKick;
            }	
        }
        action
        {
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

            // Also head towards the ball.

            WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), 0.65f, 0.f));
        }
    }
	
	// In this state the robot align behind the ball towards the goal.
	
	state(alignWithGoal)
	{
		transition
		{
			// If something changed and the ball is not close enough.

			if (theBallModel.estimate.position.x() > 250.f)
			{
				OUTPUT_TEXT("walkToBall");

				goto walkToBall;
			}

			// Check if the ball has been seen recently.
            
            if(theLibCodeRelease.timeSinceBallWasSeen > 5000)
            {
                // If not, we need to relocate it.
                
                OUTPUT_TEXT("searchForBall");

                goto searchForBall;
            }
			
			// If properly aligned kick the ball.
			// The decision about the shooting foot must go a step higher. The foot should be decided higher in the hierarchy and then be used here.

			if (((theLibCodeRelease.between(theBallModel.estimate.position.y(), 40.f, 60.f) && (theTeamBallModel.position.y() >= 0.f)) || 
				(theLibCodeRelease.between(theBallModel.estimate.position.y(), -60.f, -40.f) && (theTeamBallModel.position.y() < 0.f)))
				&&
				(theBallModel.estimate.position.x() <= 210.f) && (std::abs(theLibCodeRelease.angleToGoal) < 5_deg))
			{
				OUTPUT_TEXT("kickToGoal");

				goto kickToGoal;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.
			
			LookAtTargetAndBall(Vector3f(theFieldDimensions.xPosOpponentGoal, 0.f, 0.f),Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Do a first check for the angle with the ball.

			if (std::abs(theLibCodeRelease.angleToGoal) > (pi / 3))
			{
				// Based on the angle the robot has to turn in order to kick the ball we can decide which way to turn.

				if (theLibCodeRelease.angleToGoal > 0.f)
				{
					WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (theBallModel.estimate.position.x() - 210.f) / 100.f, -0.75f));
				}
				else
				{
					WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (theBallModel.estimate.position.x() - 210.f) / 100.f, 0.75f));
				}
			}
			else
			{
				// Align in order to be able to kick the ball towards the target with the appropriate foot.

				if (theTeamBallModel.position.y() >= 0.f)
				{
					// Walk to the given position. Align for left foot kick.

					WalkToTarget(Pose2f(60.f, 70.f, 70.f), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 200.f, theBallModel.estimate.position.y() - 50.f));
				}
				else
				{
					// Walk to the given position. Align for right foot kick.

					WalkToTarget(Pose2f(60.f, 70.f, 70.f), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 200.f, theBallModel.estimate.position.y() + 50.f));
				}
			}
		}
	}

	// In this state the robot align behind the ball in order to dribble left towards the goal.

	state(alignLeftDribble)
	{
		transition
		{
			// If something changed and the ball is not close enough.

			if (theBallModel.estimate.position.x() > 250.f)
			{
				OUTPUT_TEXT("walkToBall");

				goto walkToBall;
			}

			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// If properly aligned kick the ball.

			if (theLibCodeRelease.between(theBallModel.estimate.position.y(), 30.f, 70.f) && (theBallModel.estimate.position.x() <= 200.f) && (std::abs(theLibCodeRelease.angleToGoal) < 15_deg))
			{
				OUTPUT_TEXT("dribbleLeft");

				goto dribbleLeft;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtTargetAndBall(Vector3f(theFieldDimensions.xPosOpponentGoal, 0.f, 0.f),Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Walk to the given position.

			WalkToTarget(Pose2f(65.f, 65.f, 65.f), Pose2f(theLibCodeRelease.angleToGoal + 10_deg, theBallModel.estimate.position.x() - 200, theBallModel.estimate.position.y() - 50));
		}
	}

	// In this state the robot align behind the ball in order to dribble right towards the goal.

	state(alignRightDribble)
	{
		transition
		{
			// If something changed and the ball is not close enough.

			if (theBallModel.estimate.position.x() > 250.f)
			{
				OUTPUT_TEXT("walkToBall");

				goto walkToBall;
			}

			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// If properly aligned kick the ball.
			//!!!!!!(bool(float value, float ------->min<------, float -------> max <------,)) between,

			if (theLibCodeRelease.between(theBallModel.estimate.position.y(), -70.f, -30.f) && (theBallModel.estimate.position.x() <= 200.f) && (std::abs(theLibCodeRelease.angleToGoal) < 15_deg))
			{
				OUTPUT_TEXT("dribbleRight");

				goto dribbleRight;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtTargetAndBall(Vector3f(theFieldDimensions.xPosOpponentGoal, 0.f, 0.f),Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Walk to the given coordinates in order to align for the dribble.

			WalkToTarget(Pose2f(65.f, 65.f, 65.f), Pose2f(theLibCodeRelease.angleToGoal - 10_deg, theBallModel.estimate.position.x() - 200.f, theBallModel.estimate.position.y() + 50.f));
		}
	}

	// In this state the robot decides the type of the kick is going to execute.

	state(decideKick)
	{
		transition
		{
			OUTPUT_TEXT("Number of obstacles seen: " << int(theObstacleModel.obstacles.size()));
			//OUTPUT_TEXT(theObstacleModel.obstacles[0].type);

			// Check if there are any obstacles.

			// It doesn't really see the goal post and even when it does there is a hight possibility that it might recognize it as a some robot. Could be the color?

			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Lets try something else. Work with distance from goal. How far you are from the enemy goal.

			OUTPUT_TEXT("Distance " << float((std::abs(theFieldDimensions.xPosOpponentGoal) - theRobotPose.translation.x())));

			if (int(theObstacleModel.obstacles.size()) == 0) // This does not happen often.
			{
				OUTPUT_TEXT("alignWithGoal");
				
				goto alignWithGoal;
			}

			// Check if there is an opponent blocking your path. That could be more complicated with some movement prediction. What about PathPlanner?

			for (const Obstacle& obstacle : theObstacleModel.obstacles)
			{
				if (obstacle.isOpponent())
				{
					OUTPUT_TEXT("obstacle.isOpponent() " << obstacle.isOpponent());

					if (std::abs(obstacle.center.y()) > 290.f)
					{
						OUTPUT_TEXT("alignWithGoal");
						
						goto alignWithGoal;
					}

					// Check whether a dribble to the left is the better option. Meaning that is <280

					if (obstacle.center.y() <= 0.f)
					{
						OUTPUT_TEXT("alignLeftDribble");

						goto alignLeftDribble;
					}

					// Check whether a dribble to the right is the better option.

					if (obstacle.center.y() > 0.f)
					{
						OUTPUT_TEXT("alignRightDribble");

						goto alignRightDribble;
					}
				}
			}

			// If no opponent was found.

			OUTPUT_TEXT("alignWithGoal");

			goto alignWithGoal;
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));
			
			Stand(); // Probably not
		}
	}

	// In this state the robot kicks the ball towards the goal.
	
	state(kickToGoal)
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
			// Look At the target, in this case is the opponent goal. Its 0.f for y because both goals are centered.

			LookAtTarget(Vector3f(theFieldDimensions.xPosOpponentGoal,0.f,0.f));
			
			if (theTeamBallModel.position.y() >= 0.f)
			{
				// Different type of kicks? // The parameters must be the same with the initial // TODO must not use this for final kick.

				InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 100, theBallModel.estimate.position.y() - 50)); //theBallModel.estimate.position.x() - 160, theBallModel.estimate.position.y() - 55));
			}
			else
			{
				// Different type of kicks? // The parameters must be the same with the initial // TODO must not use this for final kick.

				InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::right), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 100, theBallModel.estimate.position.y() + 50)); //theBallModel.estimate.position.x() - 160, theBallModel.estimate.position.y() - 55));
			}
		}
	}
	
	// In this state the robot dribbles left in order to avoid an opponent.

	state(dribbleLeft)
	{
		transition
		{
			if (state_time > 5000 || (action_done))
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBall;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Use a different in walk kick variant, turnOut.

			InWalkKick(WalkKickVariant(WalkKicks::turnOut, Legs::right), Pose2f(theBallModel.estimate.position.angle(),theBallModel.estimate.position.x() - 150, theBallModel.estimate.position.y() - 50)); //theBallModel.estimate.position.x() - 160, theBallModel.estimate.position.y() - 55));
		}
	}

	// In this state the robot dribbles right in order to avoid an opponent.

	state(dribbleRight)
	{
		transition
		{
			if (state_time > 5000 || (action_done))
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBall;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Use a different in walk kick variant, turnOut.
			
			InWalkKick(WalkKickVariant(WalkKicks::turnOut, Legs::left), Pose2f(theBallModel.estimate.position.angle(),theBallModel.estimate.position.x() - 150, theBallModel.estimate.position.y() + 50)); //theBallModel.estimate.position.x() - 160, theBallModel.estimate.position.y() - 55));
		}
	}

    // A default state where Nao searches for the ball.
    
    state(searchForBall)
    {
        transition
        {
            // Could this be done otherwise?
            
			if(theLibCodeRelease.timeSinceBallWasSeen < 600)
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

			WalkAtRelativeSpeed(Pose2f(0.65f, 0.f, 0.f)); // last known position?
        }
    }
}