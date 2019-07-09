/* This is a behavior of a simple goal keeper that protects its own goal and also gives information in the rest of the team.
 * Given the fact that the game states have the following order: 
 * initial
 * set - standing
 * ready - walks to kick off/in this case walk to the goal?
 * playing
 * getup - When fallen down
 * finished
 * So since set and ready state takes care of the standing we can go straight to locating.
 */

option(SimpleGoalKeeper)
{
	// Does a quick scan of the field in order to determine its position.

	common_transition
	{
		// A good way to print information in the console without including it in every state.

		//OUTPUT_TEXT("theFieldDimensions.xPosOwnGroundline " << theFieldDimensions.xPosOwnGroundline);
		//OUTPUT_TEXT("theRobotPose.translation.x() " << theRobotPose.translation.x());
		//OUTPUT_TEXT("theRobotPose.translation.y() " << theRobotPose.translation.y());
		//OUTPUT_TEXT("theRobotPose.rotation " << theRobotPose.rotation);
	}

	initial_state(localize)
	{
		transition
		{
			// A problem now occurs with the faster scanning. I don't know when its done..

			// Wait a specific amount of time for the localization to be finish.

			// Its also support to run after the ready state. Not straight to playing state.

			if (state_time > 8000) // 4 seconds maybe something with action done? !! How to add searchForBall to actions so action is done can be triggered?
			{
				OUTPUT_TEXT("turnToGoal");

				goto turnToGoal;
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

	/* In this state the goalkeeper needs to find his goalposts. */

	state(turnToGoal)
	{
		transition
		{
			// Check whether the goalkeeper is facing the goal in order to approach it.

			if (std::abs(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(theFieldDimensions.xPosOwnGroundline, 0.f))) < 10_deg)
			{
				OUTPUT_TEXT("walkToGoal");

				goto walkToGoal;
			}
		}
		action
		{
			// Set the head in order to look forward.

			LookForward();
			
			WalkAtRelativeSpeed(Pose2f(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(theFieldDimensions.xPosOwnGroundline, 0.f)), 0.f, 0.f));
		}
	}

	/* Walk to the goal in order to defend it.
	* In order to find the goal we search the posts while for the alignment the penalty mark is used.
	*/

	state(walkToGoal)
	{
		transition
		{
			// Check whether the goalkeeper is facing the goal in order to approach it. Our own side coordinates is always -

			if (theLibCodeRelease.between(theRobotPose.translation.x(), theFieldDimensions.xPosOwnGroundline + 10.f, theFieldDimensions.xPosOwnGroundline + 50.f)) 
			{
				OUTPUT_TEXT("alingInfrontOfGoal");

				goto alingInfrontOfGoal;
			}
		}
		action
		{
			// Set the head in order to look at its own goal. Since we are using global coordinates we use onGround.

			// Since we want the coordinates relative to the robot we can get they by using the code in \BHumanCodeRelease\Src\Modules\BehaviorControl\Libraries\LibCodeReleaseProvider.cpp
			// Or even

			LookAtTarget(Vector3f(0.f, 0.f, (theRobotPose.inversePose * Vector2f(theFieldDimensions.xPosOwnGroundline, 0.f)).angle()));
			
			// Walk to your goal.

			WalkAtRelativeSpeed(Pose2f((theRobotPose.inversePose * Vector2f(theFieldDimensions.xPosOwnGroundline, 0.f)).angle(),0.65f,0.f));
		}
	}

	/* This state is different from the align to goal of a defender or striker. The goalkeeper aligns with the goal he is defending based on the position of the ball.*/

	state(alingInfrontOfGoal)
	{
		transition
		{
			// Check whether you have reached the wanted position.

			if (theLibCodeRelease.between(theRobotPose.translation.y(), -5.f,  5.f) && (theLibCodeRelease.between(theRobotPose.translation.x(), theFieldDimensions.xPosOwnGroundline + 10.f, theFieldDimensions.xPosOwnGroundline + 30.f))) //&& (std::abs((theRobotPose.inversePose * Vector2f(theFieldDimensions.xPosOwnPenaltyMark, 0.f)).angle()) < 2_deg))
			{
				// Check if you already turned towards the center.

				if (std::abs(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f))) < 8_deg)
				{
					// Check if the ball is in your side of the field where it actually starts getting dangerous.

					if (theTeamBallModel.position.x() >= 0)
					{
						OUTPUT_TEXT("scanField");

						goto scanField;
					}

					OUTPUT_TEXT("trackTheBall");

					goto trackTheBall;
				}

				// Robot is not centered. Needs to turn towards the center of the field.

				OUTPUT_TEXT("turnToCenter");

				goto turnToCenter;
			}
		}
		action
		{
			// Do the action for that state. In a way the action is done first? Or parallel with the transition. Moore/Mealy?

			// Set the head in order to left and right.

			LookLeftAndRight();
			
			// Do a verification and if its needed then do some corrections.

			// Check if the rotation of the robot is bigger than the abs 90_deg meaning that is facing its goal.

			if (std::abs(theRobotPose.rotation) > 90_deg)
			{
				// If it is looking at its own goal.

				WalkAtRelativeSpeed(Pose2f(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f)), (theRobotPose.translation.x() - theFieldDimensions.xPosOwnGroundline + 10.f) / 100.f, (theRobotPose.translation.y() - theFieldDimensions.yPosCenterGoal) / 100.f));
			}
			else
			{
				WalkAtRelativeSpeed(Pose2f(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f)), ((theFieldDimensions.xPosOwnGroundline + 10.f) - theRobotPose.translation.x()) / 100.f, (theFieldDimensions.yPosCenterGoal - theRobotPose.translation.y()) / 100.f));
			}
		}
	}

	// In this state the robot turns to face the center of the field.

	state(turnToCenter)
	{
		transition
		{
			// Check whether the goalkeeper is facing the goal in order to approach it.

			if (std::abs(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f))) < 8_deg)
			{
				OUTPUT_TEXT("alingInfrontOfGoal");

				goto alingInfrontOfGoal;
			}
		}
		action
		{
			// Set the head in order to look forward.

			LookLeftAndRight();

			// Walks at relative speed.

			WalkAtRelativeSpeed(Pose2f(Geometry::angleTo(Pose2f(theRobotPose.rotation, theRobotPose.translation.x(), theRobotPose.translation.y()), Vector2f(0.f, 0.f)), 0.f, 0.f));
		}
	}

	/* Turn only the head in order to face the ball. */

	state(trackTheBall)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > theBehaviorParameters.ballNotSeenTimeOut)
			{
				// If not we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is in your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() >= 0.f)
			{
				// If its not in your side of the field.

				OUTPUT_TEXT("alingInfrontOfGoal");

				goto alingInfrontOfGoal;
			}

			// Check if the ball is inside the penalty area. 
			// -1100 <= y <= 1100
			// -3000 <= x <= -2400

			if (theLibCodeRelease.between(theTeamBallModel.position.x(), theFieldDimensions.xPosOwnGroundline, theFieldDimensions.xPosOwnPenaltyArea) &&
				theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightPenaltyArea, theFieldDimensions.yPosLeftPenaltyArea))
				 
			{
				OUTPUT_TEXT("Inside penalty area. AlignForKick.");

				goto alignForKick;
			}

			// Check if the ball is not inside the area the goalkeeper can move. This are is 400 before the left and 400 before the right goal.	

			if (!(theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightGoal + 400, theFieldDimensions.yPosLeftGoal - 400)))
			{
				OUTPUT_TEXT("blockTheOpenCorner");

				goto blockTheOpenCorner;
			}

			// Check if the ball is moving in order to adjust your position in front of the goal.

			if (theBallModel.estimate.velocity != Vector2f::Zero() || (theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightGoal + 400, theFieldDimensions.yPosLeftGoal - 400)))
			{
				OUTPUT_TEXT("active tracking");

				goto activeTracking;
			}
		}
		action
		{
			// Change the head control in order for the robot to look at the ball.

			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			Stand();
		}
	}

	/* In this state the goalkeeper scans the field in order to keep track and give feedback to his teammates. */

	state(scanField)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > theBehaviorParameters.ballNotSeenTimeOut)
			{
				// If not we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is in your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() < 0)
			{
				OUTPUT_TEXT("trackTheBall");

				goto trackTheBall;
			}
		}
		action
		{
			// Keep scanning for feedback.

			LookLeftAndRight();

			Stand();
		}
	}

	/* In this state the robot moves in order to correct its y coordinates according to the ball making sure it covers the majority of the its goal. */

	state(activeTracking)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > theBehaviorParameters.ballNotSeenTimeOut)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}
			
			// Check if the ball is inside the goalkeepers area. 
				// -1100 <= y <= 1100
				// -3000 <= x <= -2400

			if (theLibCodeRelease.between(theTeamBallModel.position.x(), theFieldDimensions.xPosOwnGroundline, theFieldDimensions.xPosOwnPenaltyArea) &&
				theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightPenaltyArea, theFieldDimensions.yPosLeftPenaltyArea))

			{
				OUTPUT_TEXT("Inside penalty area. AlignForKick.");

				goto alignForKick;
			}
			
			// Check if the ball is in your side of the field where it actually starts getting dangerous.

			if (theTeamBallModel.position.x() >= 0)
			{
				OUTPUT_TEXT("alingInfrontOfGoal");

				goto alingInfrontOfGoal;
			}

			// Check if the ball has moved outside the area of action of the goalkeeper.

			if (!(theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightGoal + 400, theFieldDimensions.yPosLeftGoal - 400)))
			{
				OUTPUT_TEXT("blockTheOpenCorner");

				goto blockTheOpenCorner;
			}

		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot. 
			
			LookAtBall(Vector3f(theBallModel.estimate.position.x(), theBallModel.estimate.position.y(), 0.f));

			// Might be a bit confusing. Basically it constrains the movement speed to 0.65f if it exceeds that value.
			// Also in order to determine the sign of the speed I divide the value with its abs value resulting into 1 or -1 and multiplying that with 0.86f.

			WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (std::abs(theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x())/100.f > 0.65f ? 0.65f * ((theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x()) / std::abs(theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x())) : (theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x())/100.f),
				(std::abs(theTeamBallModel.position.y() - theRobotPose.translation.y()) / 100.f > 0.65f ? 0.65f * (theTeamBallModel.position.y() - theRobotPose.translation.y()) / (std::abs(theTeamBallModel.position.y() - theRobotPose.translation.y())) : (theTeamBallModel.position.y() - theRobotPose.translation.y())/100.f)));
		}
	}

	// In this state the goalkeeper walks to the side of the goal that needs to be blocked.

	state(blockTheOpenCorner)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > theBehaviorParameters.ballNotSeenTimeOut)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is inside the penalty area. 
			// -1100 <= y <= 1100
			// -3000 <= x <= -2400

			if (theLibCodeRelease.between(theTeamBallModel.position.x(), theFieldDimensions.xPosOwnGroundline, theFieldDimensions.xPosOwnPenaltyArea) &&
				theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightPenaltyArea, theFieldDimensions.yPosLeftPenaltyArea))

			{
				OUTPUT_TEXT("Inside penalty area. AlignForKick.");

				goto alignForKick;
			}

			// Check if the ball passed the left goal post.	

			if (theTeamBallModel.position.y() >= theFieldDimensions.yPosLeftGoal && theLibCodeRelease.between(theRobotPose.translation.y(), theFieldDimensions.yPosLeftGoal - 410, theFieldDimensions.yPosLeftGoal - 390))
			{
				OUTPUT_TEXT("Ball y passed left goal post. trackTheBallWhileBlocking");

				goto trackTheBallWhileBlockingTheCorner;

			}
			
			// Check if the ball passed the right goal post.

			if (theTeamBallModel.position.y() <= theFieldDimensions.yPosRightGoal && theLibCodeRelease.between(theRobotPose.translation.y(), theFieldDimensions.yPosRightGoal + 390, theFieldDimensions.yPosRightGoal + 410))
			{
				OUTPUT_TEXT("Ball y passed right goal post. trackTheBallWhileBlockingTheCorner");

				goto trackTheBallWhileBlockingTheCorner;
			}

			// Keep checking the position of the ball in order to transition to active tracking.

			if (theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightGoal + 400, theFieldDimensions.yPosLeftGoal - 400))
			{
				OUTPUT_TEXT("activeTracking");

				goto activeTracking;
			}
		}
		action
		{
			// Alternate between the penalty mark and the ball in order to assist the localization.
			
			LookAtTargetAndBall(Vector3f(0.f, 0.f, (theRobotPose.inversePose * Vector2f(theFieldDimensions.xPosOwnPenaltyMark, 0.f)).angle()), Vector3f(theBallModel.estimate.position.x(), theBallModel.estimate.position.y(), 0.f));

			// Check which side to cover. The left goalpost or the right?

			if (theTeamBallModel.position.y() >= theFieldDimensions.yPosLeftGoal - 400)
			{
				WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (std::abs(theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x()) / 100.f > 0.55f ? 0.55f * ((theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x()) / std::abs(theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x())) : (theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x()) / 100.f),
					(std::abs(theFieldDimensions.yPosLeftGoal - 400 - theRobotPose.translation.y())/ 100.f > 0.55f ? 0.55f * ((theFieldDimensions.yPosLeftGoal - 400 - theRobotPose.translation.y()) / (std::abs(theFieldDimensions.yPosLeftGoal - 400 - theRobotPose.translation.y()))) : (theFieldDimensions.yPosLeftGoal - 400 - theRobotPose.translation.y()) / 100.f)));

			}
			else if (theTeamBallModel.position.y() <= theFieldDimensions.yPosRightGoal + 400)
			{
				WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), (std::abs(theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x()) / 100.f > 0.55f ? 0.55f * ((theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x()) / std::abs(theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x())) : (theFieldDimensions.xPosOwnGroundline + 10.f - theRobotPose.translation.x()) / 100.f),
					(std::abs(theFieldDimensions.yPosRightGoal + 400 - theRobotPose.translation.y()) / 100.f > 0.55f ? 0.55f * ((theFieldDimensions.yPosRightGoal + 400 - theRobotPose.translation.y()) / (std::abs(theFieldDimensions.yPosRightGoal + 400 - theRobotPose.translation.y()))) : (theFieldDimensions.yPosRightGoal + 400 - theRobotPose.translation.y()) / 100.f)));
			}
		}
	}
	
	/* This state is responsible for covering one of the goalposts while tracking the ball.*/

	state(trackTheBallWhileBlockingTheCorner)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > theBehaviorParameters.ballNotSeenTimeOut)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is inside the penalty area. 
			// -1100 <= y <= 1100
			// -3000 <= x <= -2400

			if (theLibCodeRelease.between(theTeamBallModel.position.x(), theFieldDimensions.xPosOwnGroundline, theFieldDimensions.xPosOwnPenaltyArea) &&
				theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightPenaltyArea, theFieldDimensions.yPosLeftPenaltyArea))

			{
				OUTPUT_TEXT("Inside penalty area. AlignForKick.");

				goto alignForKick;
			}
		
			// Keep checking the position of the ball in order to transition to active tracking.

			if (theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightGoal + 400, theFieldDimensions.yPosLeftGoal - 400))
			{
				OUTPUT_TEXT("activeTracking");

				goto activeTracking;
			}
		}
		action
		{
			// Look at ball while standing still.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(), theBallModel.estimate.position.y(), 0.f));

			Stand();
		}
	}

	// In this state the defender aligns for a kick.

	state(alignForKick)
	{
		transition
		{
			// Check if the ball has been seen recently.

			if (theLibCodeRelease.timeSinceBallWasSeen > 3000)
			{
				// If not, we need to relocate it.

				OUTPUT_TEXT("searchForBall");

				goto searchForBall;
			}

			// Check if the ball is still inside the goalkeepers area. 
			// -1100 > y  || y > 1100
			// -3000 <= x <= -2400

			if (!(theLibCodeRelease.between(theTeamBallModel.position.x(), theFieldDimensions.xPosOwnGroundline, theFieldDimensions.xPosOwnPenaltyArea) &&
				theLibCodeRelease.between(theTeamBallModel.position.y(), theFieldDimensions.yPosRightPenaltyArea, theFieldDimensions.yPosLeftPenaltyArea)))

			{
				OUTPUT_TEXT("Outside penalty area. activeTracking.");

				goto activeTracking;
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
		}
		action
		{
			// Call the look at ball with the coordinates of the ball relative to the robot.

			LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

			// Since walk at the target doesn't work as intended for angles > 60 at least we need to make a similar to the patrol movement. Walk to target pushes the ball around.

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

	/* In this state the goalkeeper kicks the ball away from its goal. */

	state(kickAwayFromGoal)
	{
		transition
		{
			// Check if the kick has been executed or its been more than 3 seconds in this state.

			if (state_time > 3000 || (action_done))
			{
				OUTPUT_TEXT("alingInfrontOfGoal.");

				goto alingInfrontOfGoal;
			}
		}
		action
		{
			// Look At the target, in this case is the opponent goal. Its 0.f for y because both goals are centered.

			LookAtTarget(Vector3f(0.f, 0.f, theLibCodeRelease.angleToGoal));

			if (theTeamBallModel.position.y() >= 0.f)
			{
				// Different type of kicks? // The parameters must be the same with the initial // TODO must not use this for final kick.
				//theLibCodeRelease.between(theRobotPose.translation.y(), -5.f, 5.f)

				// TODO what angle should be the kicking one?

				InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 100, theBallModel.estimate.position.y() - 50));
			}
			else
			{
				// Different type of kicks? // The parameters must be the same with the initial // TODO must not use this for final kick.

				InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::right), Pose2f(theLibCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 100, theBallModel.estimate.position.y() + 50));
			}
		}
	}

	/* A default state where Nao searches for the ball. */

	state(searchForBall)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball. ActiveTracking.");

				goto activeTracking;
			}
			
			// If the goalkeeper is way to long in this state will search the ball in alternative way. This also resolves the issue where the goalkeeper gets confused with localization.
			
			if (state_time > 20000)
			{
				OUTPUT_TEXT("Its been way too long. SearchForBallWithMovement.");

				goto searchForBallWithMovement;
			}
		}
		action
		{
			// Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

			Stand();
		}
	}

	/* In this state Nao searches for the ball while turning around itself. If it has the wrong alignment it will eventually face the penalty mark and resolve the localization issue.
	   Might not be needed after all. In addition it can cause issues under certain circumstances. */

	state(searchForBallWithMovement)
	{
		transition
		{
			// Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.

			if (theLibCodeRelease.timeSinceBallWasSeen < 300)
			{
				OUTPUT_TEXT("Found the ball. ActiveTracking.");

				goto activeTracking;
			}
		}
		action
		{
			// Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

			WalkAtRelativeSpeed(Pose2f(0.55f,0.f,0.f));
		}
	}

}
