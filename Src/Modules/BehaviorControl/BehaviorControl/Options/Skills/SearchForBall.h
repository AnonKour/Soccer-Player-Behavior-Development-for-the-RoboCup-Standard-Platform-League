/** This option lets the robot stand up when it has fallen down. */
option(SearchForBall,(const Vector3f&)(Vector3f::Zero()) startingPosition)
{
	initial_state(startSearch)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball, lookAtBall...");

				goto searchBallFoundBall;
			}

			if (state_time > 4000)
			{
				OUTPUT_TEXT("searchBallTurnClockWise");

				goto searchBallTurnClockWise;
			}
		}
			action
		{
			// Turn around your head around in order to define the position of the ball.  

			HeadControlMode(HeadControl::lookLeftAndRight);

			/* A more sophisticated search must be done. A good idea would be in steps not a full rotation.
			 * So first do a single head swipe then turn 45 angles left do the same head swipe and then turn 90 degrees to do another headswipe.
			 * only 10 degrees are left unchecked. Those 10 degress can be then searched with a full rotation.
			*/

			Stand();

			//WalkAtRelativeSpeed(Pose2f(0.75f, 0.f, 0.f)); // last known position? I dont think thats a smart thing to do.
		}
	}

	state(searchBallTurnClockWise)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball, lookAtBall...");

				goto searchBallFoundBall;
			}

			if ((theRobotPose.inversePose.translation.angle()) <= -90_deg)//((theRobotPose.inversePose.translation.angle()* 180.f / pi) >= -90_deg)
			{
				OUTPUT_TEXT("searchForBallHalt");

				goto searchForBallHalt;
			}
		}
		action
		{
			HeadControlMode(HeadControl::lookForward);

			WalkAtRelativeSpeed(Pose2f(0.75f, 0.f, 0.f)); // last known position? I dont think thats a smart thing to do.
		}
	}

	state(searchForBallHalt)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball, lookAtBall...");

				goto searchBallFoundBall;
			}

			if (state_time > 4000)
			{
				OUTPUT_TEXT("searchBallTurnAntiClockWise");

				goto searchBallTurnAntiClockWise;
			}
		}
			action
		{
			// Turn around your head around in order to define the position of the ball.  

			HeadControlMode(HeadControl::lookLeftAndRight);

			Stand();
		}
	}

	state(searchForSecondBallHalt)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball, lookAtBall...");

				goto searchBallFoundBall;
			}

			if (state_time > 4000)
			{
				OUTPUT_TEXT("searchBallLastStage");

				goto searchBallLastStage;
			}
		}
		action
		{
			// Turn around your head around in order to define the position of the ball.  

			HeadControlMode(HeadControl::lookLeftAndRight);

			Stand();
		}
	}

	state(searchBallTurnAntiClockWise)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball, lookAtBall...");

				goto searchBallFoundBall;
			}

			if ((theRobotPose.inversePose.translation.angle()) >= 90_deg)
			{
				OUTPUT_TEXT("searchBallTurnAntiClockWise");

				goto searchForSecondBallHalt;
			}
		}
		action
		{
			HeadControlMode(HeadControl::lookForward);

			WalkAtRelativeSpeed(Pose2f(-0.75f, 0.f, 0.f)); // last known position? I dont think thats a smart thing to do.
		}
	}

	state(searchBallLastStage)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball, lookAtBall...");

				goto searchBallFoundBall;
			}
		}
		action
		{
			HeadControlMode(HeadControl::lookForward);

			WalkAtRelativeSpeed(Pose2f(-0.75f, 0.f, 0.f)); // last known position? I dont think thats a smart thing to do.
		}
	}

	state(searchBallFoundBall)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			OUTPUT_TEXT("Found the ball, lookAtBall...");

			//goto lookAtBall;
		}
		action
		{

		}
	}
}
