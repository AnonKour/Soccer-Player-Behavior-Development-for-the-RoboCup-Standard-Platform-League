/* This is a behavior of a simple defender that protects its own side of the field and as a result its own goal.
 * Given the fact that the game states have the following order: 
 * initial
 * ready - walks to kick off/in this case just stand?
 * set -?
 * playing
 * getup - When fallen down
 * finished
 * So since ready state takes care of the standing we can go straight to locating.
 */

option(SimpleDefender)
{
	// This is the initial/root state of the FSM.

	// In this state the Nao does a quick scan in order to determine its location before doing anything else. 

	initial_state(localize)
	{
		transition
		{
			// Wait a specific amount of time for the localization to be finish.

			// Its also supposed to run after the ready state as well. 

			if (state_time > 8000) // 4 seconds maybe and also something with action done? !! How to add searchForBall to actions so action is done can be triggered?
			{
				OUTPUT_TEXT("searchForBall.");

				goto searchForBall;
			}
		}
		action
		{
			// Do a quick scan around the field. Does it need to move around itself for better accuracy?

			// Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

			Stand();
		}
	}

	// In this state the Nao stands and observes the ball.

	state(decideNextMove)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is on your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() < 0)
			{
				OUTPUT_TEXT("moveToBallWhileBlocking");

				goto moveToBallWhileBlocking;
			}

			// Check if the ball is not on your side of the field.

			if (theTeamBallModel.position.x() >= 0)
			{
				OUTPUT_TEXT("walkToObservingPosition");

				goto walkToObservingPosition;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			Stand();
		}
	}

	// In this state the defender move in order to kick the ball away fast while trying to body block an opponent.

	state(moveToBallWhileBlocking)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is not on your side of the field any more.

			if (theTeamBallModel.position.x() >= 0)
			{
				OUTPUT_TEXT("walkToObservingPosition");

				goto walkToObservingPosition;
			}

			// Check if you have reached the ball.

			if (theBallModel.estimate.position.x() < 250.f)
			{
				OUTPUT_TEXT("alignForKick");

				goto alignForKick;
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

	// In this state the defender aligns for a kick.

	state(alignForKick)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is not on your side of the field any more.

			if (theTeamBallModel.position.x() >= 0) 
			{
				OUTPUT_TEXT("walkToObservingPosition");

				goto walkToObservingPosition;
			}

			// If properly aligned kick the ball.

			if (((theLibCodeRelease.between(theBallModel.estimate.position.y(), 40.f, 60.f) &&
				(theTeamBallModel.position.y() >= 0.f)) ||
				(theLibCodeRelease.between(theBallModel.estimate.position.y(), -60.f, -40.f) &&
				(theTeamBallModel.position.y() < 0.f)))
				&&
				(theBallModel.estimate.position.x() <= 200.f) &&
				(std::abs(theLibCodeRelease.angleToGoal) < 15_deg))
			{
				OUTPUT_TEXT("kickAwayFromGoal");

				goto kickAwayFromGoal;
			}

			// Check if the ball moved away.
			
			if (theBallModel.estimate.position.x() > 250.f)
			{
				OUTPUT_TEXT("moveToBallWhileBlocking");

				goto moveToBallWhileBlocking;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtTargetAndBall(Vector3f(0.f,0.f,theLibCodeRelease.angleToGoal),Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Since walkAtTarget pushes the ball, doesn't work as we would like to for angles > 90 at least we need to make a similar to the patrol movement.

			if (std::abs(theLibCodeRelease.angleToGoal) > (pi / 3))
			{
				// Based on the angle the robot has to turn in order to kick the ball we can decide which way to turn.

				if (theLibCodeRelease.angleToGoal > 0.f)
				{
					WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle()*5.f, (theBallModel.estimate.position.x() - 210.f) / 100.f, -0.75f));
				}
				else
				{
					WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle()*5.f, (theBallModel.estimate.position.x() - 210.f) / 100.f, 0.75f));
				}

			}
			else
			{
				// Align in order to be able to kick the ball towards the target.
				// Call the look at ball with the coordinates of the ball relative to the robot.

				if (theTeamBallModel.position.y() >= 0.f)
				{
					WalkToTarget(Pose2f(60.f, 70.f, 70.f), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 200.f, theBallModel.estimate.position.y() - 50.f));
				}
				else
				{
					WalkToTarget(Pose2f(60.f, 70.f, 70.f), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 200.f, theBallModel.estimate.position.y() + 50.f));
				}
			}
		}
	}

	// In this state the defender kicks the ball away from his goal

	state(kickAwayFromGoal)
	{
		transition
		{
			if (state_time > 3000 || (action_done))//(state_time > 1000 && action_done))
			{
				OUTPUT_TEXT("decideNextMove.");

				goto decideNextMove;
			}
		}
		action
		{
			// Look At the target, in this case is the opponent goal. Its 0.f for y because both goals are centered.

			LookAtTarget(Vector3f(theFieldDimensions.xPosOpponentGoal,0.f,0.f));

			if (theTeamBallModel.position.y() >= 0.f)
			{
				// Different type of kicks? // The parameters must be the same with the initial // TODO must not use this for final kick. Can not balance...

				// TODO what angle should be the kicking one?

				InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 100, theBallModel.estimate.position.y() - 50)); //theBallModel.estimate.position.x() - 160, theBallModel.estimate.position.y() - 55));
			}
			else
			{
				// Different type of kicks? // The parameters must be the same with the initial // TODO must not use this for final kick.

				InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::right), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 100, theBallModel.estimate.position.y() + 50)); //theBallModel.estimate.position.x() - 160, theBallModel.estimate.position.y() - 55));
			}
		}
	}

	// The robot walks to the predefined observation point. Its position shares the y of the global ball coordinates.

	state(walkToObservingPosition)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is on your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() < 0)
			{
				OUTPUT_TEXT("moveToBallWhileBlocking");

				goto moveToBallWhileBlocking;
			}

			// Check if the robot reached the observing position.
			// TODO maybe I should add the angle with the ball as well.
			if (theLibCodeRelease.between(theRobotPose.translation.x(), -910.f, -890.f) && (theLibCodeRelease.between(theRobotPose.translation.y(), theTeamBallModel.position.y() - 10.f,theTeamBallModel.position.y() + 10.f)))
			{
				OUTPUT_TEXT("standingAndObserving");

				goto standingAndObserving;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Also head towards the observing position.

			// Might be a bit confusing. Basically it constrains the movement speed to 0.85f if it exceeds that value.
			// Also in order to determine the sign of the speed I divide the value with its abs value resulting into 1 or -1 and multiplying that with 0.85f.

			WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (std::abs(-900.f - theRobotPose.translation.x()) > 0.85f ? 0.85f * ((-900.f - theRobotPose.translation.x())/std::abs(-900.f - theRobotPose.translation.x())) : (-900.f - theRobotPose.translation.x())), (std::abs(theTeamBallModel.position.y() - theRobotPose.translation.y()) > 0.85f ? 0.85f * ((theTeamBallModel.position.y() - theRobotPose.translation.y())/(std::abs(theTeamBallModel.position.y() - theRobotPose.translation.y()))) : (theTeamBallModel.position.y() - theRobotPose.translation.y()))));
		
		}
	}

	// In this state the Nao stands and observes the ball.

	state(standingAndObserving)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is on your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() < 0)
			{
				OUTPUT_TEXT("moveToBallWhileBlocking");

				goto moveToBallWhileBlocking;
			}

			// Check the ball's velocity relative to the robot and patrol left.

			if (theBallModel.estimate.velocity.y() > 0)
			{
				OUTPUT_TEXT("patrolLeft");

				goto patrolLeft;
			}

			// Check the ball's velocity relative to the robot and patrol right.

			if (theBallModel.estimate.velocity.y() < 0)
			{
				OUTPUT_TEXT("patrolRight");

				goto patrolRight;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(), theBallModel.estimate.position.y(), 0.f));

			Stand();
		}
	}

	// A state responsible for the left patrol movement of the robot.

	state(patrolLeft)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is on your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() < 0)
			{
				OUTPUT_TEXT("moveToBallWhileBlocking");

				goto moveToBallWhileBlocking;
			}

			// Check if the ball stopped moving and if you reached the required position. It might also just went outside of the camera frame. Works either way.
		
			if (theBallModel.estimate.velocity == Vector2f::Zero())
			{
				OUTPUT_TEXT("The ball stopped moving. walkToObservingPosition.");

				goto walkToObservingPosition;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Walk sideways while trying to keep your rotation to zero, meaning that you face the center.

			WalkAtRelativeSpeed(Pose2f(theRobotPose.rotation*-1, 0.f, 0.75f));
		}
	}

	// A state responsible for the right patrol movement of the robot.

	state(patrolRight)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 5000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is on your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() < 0)
			{
				OUTPUT_TEXT("moveToBallWhileBlocking");

				goto moveToBallWhileBlocking;
			}

			// Check if the ball stopped moving and if you reached the required position. It might also just went outside of the camera frame. Works either way.

			if (theBallModel.estimate.velocity == Vector2f::Zero()) 
			{
				OUTPUT_TEXT("The ball stopped moving. walkToObservingPosition.");

				goto walkToObservingPosition;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(), theBallModel.estimate.position.y(), 0.f));

			// Walk sideways while trying to keep your rotation to zero, meaning that you face the center.

			WalkAtRelativeSpeed(Pose2f(theRobotPose.rotation*-1, 0.f, -0.75f));
		}
	}

	// A default state where Nao searches for the ball.

	state(searchForBall)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball, decideNextMove");

				goto decideNextMove;
			}
		}
		action
		{
			LookLeftAndRight();

			Stand();

			WalkAtRelativeSpeed(Pose2f(0.65f, 0.f, 0.f)); // last known position?
		}
	}

}