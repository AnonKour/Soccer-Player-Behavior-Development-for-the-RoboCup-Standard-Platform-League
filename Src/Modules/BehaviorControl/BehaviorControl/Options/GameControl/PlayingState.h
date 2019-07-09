option(PlayingState)
{
	initial_state(SimpleGoalKeeper)// Give it a appropriate name in order to understand the state during execution.
	{
		action
		{
			//ScienceDaySoccerBehaviorV2();
			//SimpleStriker();
			//SimpleDefender();
			SimpleGoalKeeper();
			//BallControl(); //Can be called from here as well if we want it to be a part of the game flow.
			//ActiveBallTracking();
			//BhumanExampleStriker();
			//StationaryBallTracking();

			// In order to work with KikeView probably the best it would be to leave this state without a call.
		}
	}
}
