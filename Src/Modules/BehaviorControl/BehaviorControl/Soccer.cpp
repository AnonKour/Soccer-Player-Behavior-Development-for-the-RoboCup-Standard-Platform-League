/**
 * @file Soccer.cpp
 *
 * This file implements the root behavior option (playing soccer).
 *
 * @author Arne Hasselbring
 */

#include "Soccer.h"
#include "Tools/BehaviorOptionRegistry.h"

MAKE_BEHAVIOR_OPTION(Soccer);

Soccer::Soccer() :
  Cabsl<Soccer>(BehaviorOptionRegistry::theInstance->theActivationGraph)
{
}

void Soccer::execute()
{
  beginFrame(theFrameInfo.time, false);

  // Default behavior for Soccer.

  Cabsl<Soccer>::execute(OptionInfos::getOption("Root"));

  // For an alternative behaviors that doesn't comply with soccer procedures.

  //Cabsl<Soccer>::execute(OptionInfos::getOption("AlternativeBehavior"));

  //Cabsl<Soccer>::execute(OptionInfos::getOption("BallControlBehavior"));

  endFrame();
}
