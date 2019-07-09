/** A script where the robot tracks and follows the ball around while staying inside a specific part of the field.*/

option(ActiveBallTracking)
{   
    // In this state the nao does a quick scan in order to determine its location before doing anything else. 
    // When more than one nao are in the team I believe that this step can be skipped since they are all working for the localazation? Maybe.
    
    initial_state(localize)
    {
        transition
        {
            if(state_time > 9000) // 9 seconds maybe something with action done?
            {
                OUTPUT_TEXT("Walking to ball");

                goto walkToBall;
            }
        }
        action
        {
            // Do a quick scan around the field. Does it need to move around itself for better accuracy?
            
            // Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();
        }
    }

    // In this state the nao heads to the ball.
    
    state(walkToBall)
    {
        transition
        {        
            // Check if the ball has been seen recently.
            
            if(theLibCodeRelease.timeSinceBallWasSeen > 300)//4000)//theBehaviorParameters.ballNotSeenTimeOut)
            {
                // If not, we need to relocate it.
                
                OUTPUT_TEXT("searchForBall");

                goto searchForBall;
            }
            
            // If you have reached the ball stand still.
            
            if(theBallModel.estimate.position.x() < 250.f)
            {
                OUTPUT_TEXT("stand");

                goto stand;
            }
        }
        action
        {
            // Call the look at ball with the coordinates of the ball relative to the robot.

            LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));

            WalkAtRelativeSpeed(Pose2f(theBallModel.estimate.position.angle(), 0.6f, 0.f));
        }
    }

    /** The robot stands up and starts to play when stand was executed. */
    
    state(stand)
    {
        transition
        {
            // Check if the ball is relative close to the robot.

			if(theBallModel.estimate.position.x() <= 250.f)//state_time > 4000) //action_done && 
            {
                // If you have reached the ball sit down. Can I track while sitted?
                
                OUTPUT_TEXT("Figuring out the position on the field.");
                goto localize;
            }
        }
        action
        {
            // Set the head control mode to lookforward.

			LookForward();
          
            Stand();
        }
    }
    
    // A default state where Nao searchs for the ball.
    
    state(searchForBall)
    {
        transition
        {
            // Could this be done otherwise? Like if the ball is in the frame? This must be based with the walk to ball state in order to avoid needless loops.
            
			if(theLibCodeRelease.timeSinceBallWasSeen < 300)
            {
                OUTPUT_TEXT("Found the ball, heading there...");
                OUTPUT_TEXT("walkToBall");

                goto walkToBall;
            }

            // We need to be checking based on the ground line not the field border.
            
            /* Make sure you are staying inside the field.*/
            
			// There is something very similar at Src/Representations/Configuration/FieldDimention.h

			if(!((theRobotPose.translation.x() <= theFieldDimensions.xPosOpponentGroundline - 1000.f)
            && 
            (theRobotPose.translation.x() >= theFieldDimensions.xPosOwnGroundline + 1000.f)
                
            && 
                
            (theRobotPose.translation.y() <= theFieldDimensions.yPosLeftSideline - 1000.f)
            &&
            (theRobotPose.translation.y() >= theFieldDimensions.yPosRightSideline + 1000.f)))
			{
                OUTPUT_TEXT("realigning");

                goto realign;
            }
        }
        action
        {
            // Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();
            
            WalkAtRelativeSpeed(Pose2f(0.f, 0.7f, 0.f)); // last known position? I do have an idea about the last known position since I am only checking the time the ball was last seen.
        }
    }
    
    // A default state the compliments the searchForBall state in order to keep the robot inside the field bounds. NEED TO FIND A BETTER NAME!!
    
    state(realign)
    {
        transition
        {
			/* From the autonomous agent project I have found that in order to determine the angle with something you do
			/  libCodeRelease.angleToGoal = (theRobotPose.inversePose * Vector2f(theFieldDimensions.xPosOpponentGroundline, 0.f)).angle();
			/  The above code is take from LibCodeReleaseProvider.cpp located at Src\Modules\BehaviorControl\Libraries but I thought its not 
			/  working... Is it? I am confuzed. Well its rads...
			*/
			
            // Check whether you are facing the inside of the field in order to continue the search for the ball inside the field.
            
            if((theRobotPose.translation.x() <= theFieldDimensions.xPosOpponentGroundline - 1000.f)
            && 
            (theRobotPose.translation.x() >= theFieldDimensions.xPosOwnGroundline + 1000.f)
            && 
            (theRobotPose.translation.y() <= theFieldDimensions.yPosLeftSideline - 1000.f)
            &&
            (theRobotPose.translation.y() >= theFieldDimensions.yPosRightSideline + 1000.f)
			&&
			(std::abs((theRobotPose.inversePose * Vector2f(0.f, 0.f)).angle()) < 1)) // its not exactly degrees its Rads!
			//theLibCodeRelease.between(theRobotPose.rotation, 190_deg, 260_deg))) // I want it between 190 and 260 ish
            {
                OUTPUT_TEXT("Resuming the search.");

                goto searchForBall;
            }
        }
        action
        {
            // Turn around your head around in order to define the position of the ball.  

			LookLeftAndRight();

			if((theRobotPose.inversePose * Vector2f(0.f, 0.f)).angle() > 0)//(theRobotPose.rotation > 0_deg)
            {
                WalkAtRelativeSpeed(Pose2f(0.8f, 0.4f, 0.f));
            }
            else
            {
                WalkAtRelativeSpeed(Pose2f(-0.8f, 0.4f, 0.f));
            }
        }
    }
}
