/** A script where the robot tracks the ball while standing still. */

option(StationaryBallTracking)
{
    // This is the initial/root state of the fsm.

    initial_state(start)
    {
        // Decide on the next state of the finite state machine.

        transition
        {
            // The time variable is in ms. So 3000ms == 3s.

            if(state_time > 3000) // Action done?
            {
                OUTPUT_TEXT("Track the ball.");

                goto trackTheBall;
            }
        }
        action
        {
			//HeadControlMode not HeadControl anymore
			// MAYBE? yeah Because its needed in order to be re initialized.
			// Thats why after this initializatioin you can just call HeadControl with the mode you want to change to.
			// Thats why the error about redeclaration occurs.
			// Magic
            /* Was 
            // Set the head control mode. This happens in paraller with the behavior control.

            theHeadControlMode = HeadControl::lookForward;

            // We need to call head control in order for the mode to be used.

            HeadControl();

            Is */

			LookForward();

            //HeadControlMode(HeadControl::lookForward);

            Stand();
        }
    }
    
    /* Turn only the head in order to face the ball. */

    state(trackTheBall)
    {
        transition
        {        
            // Check if the ball has been seen recently.
            
            if(theLibCodeRelease.timeSinceBallWasSeen > theBehaviorParameters.ballNotSeenTimeOut)
            {
                // If not we need to relocate it.

                OUTPUT_TEXT("searchForBall");

                goto searchForBall;
            }
        }
        action
        {
            // Change the head control in order for the robot to look at the ball.
            
            // Call the look at ball with the coordinates of the ball relative to the robot.

            LookAtBall(Vector3f(theBallModel.estimate.position.x(),theBallModel.estimate.position.y(),0.f));
        }
    }
  
    // A default state where Nao searchs for the ball.
    
    state(searchForBall)
    {
        transition
        {
            // Could this be done otherwise? Like if the ball is in the frame?Not really

            if(theLibCodeRelease.timeSinceBallWasSeen < 1000) 
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

            //HeadControlMode(HeadControl::lookLeftAndRight); 
        }
    }
}
