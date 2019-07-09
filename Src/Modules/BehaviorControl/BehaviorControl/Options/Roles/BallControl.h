// In this script the Nao aims to keep control of the ball around the center mark.
/* Kick the ball with controllable force?*/

option(BallControl)
{
	common_transition
	{
		OUTPUT_TEXT("theLibCodeRelease.timeSinceBallWasSeen " << theLibCodeRelease.timeSinceBallWasSeen);

		// Check if the ball has been seen recently. Should Not Be used as it is cause it can skip the localize state entirely.

		if (theLibCodeRelease.timeSinceBallWasSeen > 5000 && theLibCodeRelease.timeSinceBallWasSeen < 100657) // It enters so many times in here until it finds the ball. && (state_time < 1000)
		{ 
			// If not, we need to relocate it.

			OUTPUT_TEXT("searchForBall");

			goto searchForBall;
		}
	}

	// This is the initial/root state of the FSM.

	// In this state the Nao does a quick scan in order to determine its location before doing anything else. 
	
	initial_state(localize)
	{
		transition
		{
			// Wait a specific amount of time for the localization to be finish.

			// Its also supposed to run after the ready state as well. 

			if (state_time > 8000) // 4 seconds maybe something with action done? !! How to add searchForBall to actions so action is done can be triggered?
			{
				OUTPUT_TEXT("walkToBallDirectly.");

				goto walkToBallDirectly;
			}
		}
		action
		{
			// Do a quick scan around the field. Does it need to move around itself for better accuracy?

			// Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

			 // A full head rotation takes around 4000 + 4000 for one more scan.
		
			Stand();
		}
	}

	// In this state the Nao heads to the ball.

	state(walkToBallDirectly)
	{
		transition
		{
			// Check if the ball is located at the target.
			
			// In this behavior the target is the center circle.
			
			//centerCircleRadius = 600; so r^2 = 600^2

			if(((theTeamBallModel.position.x()*theTeamBallModel.position.x()) + (theTeamBallModel.position.y()*theTeamBallModel.position.y())) <= (theFieldDimensions.centerCircleRadius*theFieldDimensions.centerCircleRadius))
			{
				// If yes we stand still and track it making sure it stays on target.

				OUTPUT_TEXT("Move to the observing location.");

				goto walkToObservingPosition;

				// or walk around the circle? Like patrol? Its not practical but it
				// can be a good practice of movement parallel while observing something. Like in defense.
			}

			// If you have reached the kick it to the target.

			if (theBallModel.estimate.position.norm() < 500.f)//(theBallModel.estimate.position.x() < 250.f) Probably less when the walk in an arc is resolved.
			{
				OUTPUT_TEXT("alignToTarget");

				goto alignWithTarget;
			}

			/*	If the angle with the target is too big, like in the back of the robot the robot must do a bigger maneuver.
				It cant be done with one state due to the nature of the angle? Yes, when the angles
				is above 90 degrees an extra stage is needed otherwise the maneuvering the robots needs to do with just the walkToTarget its time consuming.
			*/
		
			// Check if there is no angle with the ball and if the angle with the target is above 90 degs. 
			// IMPORTANT! In order to properly calculate the angel with something we need to use the following format:
			// (Pose2f(angle of our object ,x of our object,y of our object) * Vector2f(x of the target, y of the target)).angle() and that angle is in rads. 
			// Pose2f and Vector2f MUST NOT BE OMMITED!!!

			if ((std::abs(theBallModel.estimate.position.angle()) < 10_deg) && (std::abs(Geometry::angleTo(Pose2f(theRobotPose.rotation, theTeamBallModel.position.x(), theTeamBallModel.position.y()), Vector2f(0.f, 0.f))) > (pi/2)))
			{
				OUTPUT_TEXT("walkToBallWithAnArc");
				
				// We need to work with the alignment in a different way.

				goto walkToBallWithAnArc;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Also head towards the target. The 5.f was added with the intention to make the smaller angles turn faster. 

			// Maybe 5 is too much. Needs testing.

			WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle()*2.f, 0.8f, 0.f));//*5.f
		}
	}

	// In this state the robot align behind the ball towards the target.

	state(alignWithTarget)
	{
		transition
		{
			// If properly aligned kick the ball.

			if (theLibCodeRelease.between(theBallModel.estimate.position.y(), 20.f, 50.f) && (theBallModel.estimate.position.x() <= 210.f) && (std::abs((theRobotPose.inversePose * Vector2f(0.f, 0.f)).angle()) < 3_deg))
			{
				OUTPUT_TEXT("Kicking the ball.");

				goto kick;
			}

			// If you see the ball to go further away for some reason. !! 500 needs to change, feels kinda sloppy.

			if (theBallModel.estimate.position.norm() >= 500.f)//(theBallModel.estimate.position.x() > 250.f) 
			{
				OUTPUT_TEXT("Walking to ball directly.");

				goto walkToBallDirectly;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtTargetAndBall(Vector3f(0.f, 0.f, 0.f),Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));
			
			// Since walk at the target doesn't work as intended for angles > 90 at least we need to make a similar to the patrol movement.

			if(std::abs(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f))) > pi / 3)
			{
				// Based on the angle the robot has to turn in order to kick the ball we can decide which way to turn.

				if (Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f)) > 0)
				{
					WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (theBallModel.estimate.position.x() - 215.f)/100.f, -0.75f));
				}
				else
				{
					WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (theBallModel.estimate.position.x() - 215.f)/100.f, 0.75f));
				}
				
			}
			else
			{
				// Align in order to be able to kick the ball towards the target.

				// Also decide the kicking leg?

				WalkToTarget(Pose2f(80.f, 80.f, 80.f), Pose2f((theRobotPose.inversePose * Vector2f(0.f, 0.f)).angle(), theBallModel.estimate.position.x() - 200, theBallModel.estimate.position.y() - 50));
			}
		}
	}

	// In this state the Nao while taking into consideration the final target takes a different root and aligns with target.

	state(walkToBallWithAnArc)
	{
		transition
		{
			// Check if the ball is located at the target.

			// In this behavior the target is the center circle.

			//centerCircleRadius = 750; so r^2 = 750^2 or 600 if we are on our lab.

			if (((theTeamBallModel.position.x()*theTeamBallModel.position.x()) + (theTeamBallModel.position.y()*theTeamBallModel.position.y())) <= (theFieldDimensions.centerCircleRadius*theFieldDimensions.centerCircleRadius))
			{
				// If yes we stand still and track it making sure it stays on target.

				OUTPUT_TEXT("Move to the observing location.");

				goto walkToObservingPosition;
			}

			// If you have reached the kick it to the target.
			
			if ((theBallModel.estimate.position.norm() < 350.f)) //&& (std::abs((theRobotPose.inversePose * Vector2f(0.f, 0.f)).angle()) < pi/2))//(theBallModel.estimate.position.x() < 250.f)
			{
				OUTPUT_TEXT("alignToTarget");

				goto alignWithTarget;
			}

			// Check also if you don't need to walk in an arc anymore.

			if (std::abs(Geometry::angleTo(Pose2f(theRobotPose.rotation, theTeamBallModel.position.x(), theTeamBallModel.position.y()), Vector2f(0.f, 0.f))) < (pi / 2))
			{
				OUTPUT_TEXT("walkToBallDirectly");

				// Walk to ball directly cause it was moved.

				goto walkToBallDirectly;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Also head towards the target. - 0.5f is in order to add an arc in trajectory of the robot 

			// The closer I get to the ball the faster I want to turn in degrees but the slower I want to move in forward.

			if (Geometry::angleTo(Pose2f(theRobotPose.rotation, theTeamBallModel.position.x(), theTeamBallModel.position.y()), Vector2f(0.f, 0.f)) > 0)//(theRobotPose.rotation > 0_deg)
			{
				WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), 0.85f * theBallModel.estimate.position.norm()* 0.0006f + 0.3f, (0.85f * theBallModel.estimate.position.norm()* 0.0003f + 0.3f)*(-1)));
			}
			else
			{
				WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), 0.85f * theBallModel.estimate.position.norm()* 0.0006f + 0.3f, (0.85f * theBallModel.estimate.position.norm()* 0.0003f + 0.3f)));
			}
		}
	}

	// In this state the robot kicks the ball towards the goal.

	state(kick)
	{
		transition
		{
			if ((state_time > 3000 && action_done))//state_time > 3000 ||
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBallDirectly;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Different type of kicks? // The parameters must be the same with the initial 

			//ForwardKick(true);

			InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(0.f, theBallModel.estimate.position.x() - 100, theBallModel.estimate.position.y() - 50)); //theBallModel.estimate.position.x() - 160, theBallModel.estimate.position.y() - 55));
		}
	}

	// The robot walks to the predefined observation point. In this case the observation point is an outer circle
	// of the fields center circle. Do we want to be aligned with some field land mark? Its own goal post or penalty mark?s

	state(walkToObservingPosition)
	{	
		transition
		{
			// Check if the robot reached the observing position.

			if (theLibCodeRelease.between(((theRobotPose.translation.x()*theRobotPose.translation.x()) + (theRobotPose.translation.y()*theRobotPose.translation.y())),((theFieldDimensions.centerCircleRadius + 155.f)*(theFieldDimensions.centerCircleRadius + 155.f)),((theFieldDimensions.centerCircleRadius + 160.f)*(theFieldDimensions.centerCircleRadius + 160.f))))
			{
				OUTPUT_TEXT("standingAndObserving");

				goto standingAndObserving;
			}

			// Keep checking the position of the ball to make sure its on target.

			if (((theTeamBallModel.position.x()*theTeamBallModel.position.x()) + (theTeamBallModel.position.y()*theTeamBallModel.position.y())) > (theFieldDimensions.centerCircleRadius*theFieldDimensions.centerCircleRadius))
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBallDirectly;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Also head towards the ball.

			WalkAtRelativeSpeed(Pose2f((theRobotPose.inversePose * Vector2f(0.f, 0.f)).angle(), (((theRobotPose.translation.x()*theRobotPose.translation.x()) + (theRobotPose.translation.y()*theRobotPose.translation.y())) - ((theFieldDimensions.centerCircleRadius + 160.f)*(theFieldDimensions.centerCircleRadius + 160.f))) / 100000, 0.f));
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
				OUTPUT_TEXT("Found the ball, heading there...");

				OUTPUT_TEXT("walkToBallDirectly");

				goto walkToBallDirectly;
			}
		}
		action
		{
			// Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

			WalkAtRelativeSpeed(Pose2f(0.55f, 0.f, 0.f)); // last known position? I don't think thats a smart thing to do.
		}
	}

	// In this state the Nao stands and observes the ball.

    state(standingAndObserving)
    {
        transition
        {
			// Patrol left.

			if (theBallModel.estimate.velocity.y() > 0)
			{
				OUTPUT_TEXT("PatrolLeft");

				goto patrolLeft;
			}

			// Patrol right.

			if (theBallModel.estimate.velocity.y() < 0)
			{
				OUTPUT_TEXT("PatrolRight");

				goto patrolRight;
			}

			// Keep checking the position of the ball to make sure its on target.

			if (((theTeamBallModel.position.x()*theTeamBallModel.position.x()) + (theTeamBallModel.position.y()*theTeamBallModel.position.y())) > (theFieldDimensions.centerCircleRadius*theFieldDimensions.centerCircleRadius))
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBallDirectly;
			}
        }
        action
        {
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

            Stand();
        }
    }

	// A state responsible for the left patrol movement of the robot.

	state(patrolLeft)
	{
		transition
		{
			// Keep checking the position of the ball to make sure its on target.

			if (((theTeamBallModel.position.x()*theTeamBallModel.position.x()) + (theTeamBallModel.position.y()*theTeamBallModel.position.y())) > (theFieldDimensions.centerCircleRadius*theFieldDimensions.centerCircleRadius))
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBallDirectly;
			}

			// Check if the ball stopped moving and if you reached the required position. The shortest distance should be when he angle with the ball is zero.

			if ((theBallModel.estimate.velocity == Vector2f::Zero()) && (theLibCodeRelease.between(theBallModel.estimate.position.angle(), -1_deg, 1_deg)))
			{
				OUTPUT_TEXT("standingAndObserving");

				goto standingAndObserving;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			WalkAtRelativeSpeed(Pose2f(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f)), (((theRobotPose.translation.x()*theRobotPose.translation.x()) + (theRobotPose.translation.y()*theRobotPose.translation.y())) - ((theFieldDimensions.centerCircleRadius + 160.f)*(theFieldDimensions.centerCircleRadius + 160.f))) / 100000, 0.85f));
		}
	}

	// A state responsible for the right patrol movement of the robot.

	state(patrolRight)
	{
		transition
		{
			// Keep checking the position of the ball to make sure its on target.

			if (((theTeamBallModel.position.x()*theTeamBallModel.position.x()) + (theTeamBallModel.position.y()*theTeamBallModel.position.y())) > (theFieldDimensions.centerCircleRadius*theFieldDimensions.centerCircleRadius))
			{
				OUTPUT_TEXT("Walking to ball.");

				goto walkToBallDirectly;
			}
		
			// Check if the ball stopped moving and if you reached the required position.

			if ((theBallModel.estimate.velocity == Vector2f::Zero()) && (theLibCodeRelease.between(theBallModel.estimate.position.angle(), -1_deg, 1_deg)))// Dont forget that
			{
				OUTPUT_TEXT("standingAndObserving");

				goto standingAndObserving;
			}
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			WalkAtRelativeSpeed(Pose2f(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f)), (((theRobotPose.translation.x()*theRobotPose.translation.x()) + (theRobotPose.translation.y()*theRobotPose.translation.y())) - ((theFieldDimensions.centerCircleRadius + 160.f)*(theFieldDimensions.centerCircleRadius + 160.f))) / 100000, -0.85f));
		}
	}

}