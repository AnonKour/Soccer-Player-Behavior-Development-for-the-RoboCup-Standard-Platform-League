/** All option files that belong to the current behavior have to be included by this file. */

#include "Options/Soccer.h"

#include "Options/GameControl/HandleGameState.h"
#include "Options/GameControl/HandlePenaltyState.h"
#include "Options/GameControl/PlayingState.h"
#include "Options/GameControl/ReadyState.h"

// The headcontrol modes.

#include "Options/HeadControl/LookForward.h"
#include "Options/HeadControl/LookAtBall.h"
#include "Options/HeadControl/LookLeftAndRight.h"
#include "Options/HeadControl/LookAtTargetAndBall.h"
#include "Options/HeadControl/LookAtTarget.h"
#include "Options/HeadControl/LookAtTargetOnGroundAndBall.h"
#include "Options/HeadControl/LookAtTargetOnGround.h"

#include "Options/Output/Activity.h"
#include "Options/Output/Annotation.h"
#include "Options/Output/HeadControlMode.h"
#include "Options/Output/HeadMotionRequest/SetHeadPanTilt.h"
#include "Options/Output/HeadMotionRequest/SetHeadTarget.h"
#include "Options/Output/HeadMotionRequest/SetHeadTargetOnGround.h"
#include "Options/Output/MotionRequest/InWalkKick.h"
#include "Options/Output/MotionRequest/SpecialAction.h"
#include "Options/Output/MotionRequest/Stand.h"
#include "Options/Output/MotionRequest/WalkAtAbsoluteSpeed.h"
#include "Options/Output/MotionRequest/WalkAtRelativeSpeed.h"
#include "Options/Output/MotionRequest/WalkToTarget.h"
#include "Options/Output/MotionRequest/GetUpEngine.h"
#include "Options/Output/MotionRequest/ForwardKick.h"
#include "Options/Output/PlaySound.h"

// Implemented roles

#include "Options/Roles/StationaryBallTracking.h"
#include "Options/Roles/ActiveBallTracking.h"
#include "Options/Roles/Example.h"
#include "Options/Roles/BallControl.h"
#include "Options/Roles/SimpleStriker.h"
#include "Options/Roles/SimpleDefender.h"
#include "Options/Roles/SimpleGoalKeeper.h"
#include "Options/Roles/ScienceDaySoccerBehaviorV2.h"

#include "Options/Roles/BhumanExampleStriker.h"

#include "Options/Skills/GetUp.h"
#include "Options/Skills/SearchForBall.h"

#include "Options/Tools/ButtonPressedAndReleased.h"
#include "Options/Tools/USBCheck.h"
