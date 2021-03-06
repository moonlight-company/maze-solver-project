/*******************************************************************************
 *
 * RPHandler.cpp - Source File
 *
 * Description	:	Moonlight Company
 *			Maze Solver Project
 *			Route Planner Handler Class
 *
 * Modified	: 12.05.2016 by Utku Norman  ( norman.utku@metu.edu.tr  )
 * Created	: 24.04.2016 by Utku Norman  ( norman.utku@metu.edu.tr  )
 *
 * People	:	Doga Yucalan ( yucalan.doga@metu.edu.tr )
 *			Utku Norman  ( norman.utku@metu.edu.tr  )
 *
 ******************************************************************************/

// Command Line I/O related headers
#include <cstddef>  			// std::nullptr
#include <iostream> 			// std::cout, std::cin, std::endl
#include <iomanip>  			// std::setw

#include "../include/RPHandler.h"

/*******************************************************************************
 *
 * Constructor 
 *
 ******************************************************************************/

RPHandler::RPHandler( void )
{

}

/*******************************************************************************
 *
 * Public Methods 
 *
 ******************************************************************************/

void RPHandler::printState(
	bool printMode 		, 
	bool displayMode 
	)
{
	if( printMode )
	{
		
		std::cout << "<!---------------------------------------------";
		std::cout << std::endl;
		std::cout << "RPHandler.printState() called. Printing state:" ;
		std::cout << std::endl;

	}

	if ( IsNull( state ) )
	{

		std::cerr << "RPHandler: state is Null";	
		std::cerr << std::endl;

		return;

	}
	else
	{

		if( printMode )
		{

			std::cout << "Listing state: "		;
			std::cout << std::endl			;

			std::cout << "desiredBallDirection: "	;
			std::cout << state.desiredBallDirection	;
			std::cout << std::endl;
		}

	}

	if( printMode )
	{
		
		std::cout << "---------------------------------------------->";
		std::cout << std::endl;
		std::cout << std::endl;

	}

}

RPPacket RPHandler::handle(
	LMPacket	labyrinthState	,
	bool		debugMode
	)
{

	if( debugMode )
	{

		std::cout << std::endl						;
		std::cout << "<!---------------------------------------------"	;

		std::cout << std::endl						;

		std::cout << "RPHandler.handle() called. handling..."		;
		std::cout << std::endl						;

	}

	if ( IsNull( labyrinthState ) )
	{

		std::cerr << "RPHandler: labyrinthState of type LMPacket is Null" ;
		std::cerr << std::endl 	;

		return RPPacketNull 	;

	}

	if ( labyrinthState.ballPositionAngle < 0 )
	{

		std::cerr << "RPHandler: labyrinthState.ballPositionAngle < 0" ;
		std::cerr << std::endl 	;

		return RPPacketNull 	;

	}

	if ( IsNull( labyrinthState.ballLocationData ) )
	{

		std::cerr << "RPHandler: labyrinthState.BallLocationData is null" ;
		std::cerr << std::endl 	;

		return RPPacketNull 	;

	}

	// Declare local variables
	int pathNo ;
	
	// Initialize local variables
	pathNo = labyrinthState.ballLocationData.currentPathNo ;
	
	if( debugMode )
	{

		std::cout << "RPHandler: Begin finding desiredBallDirection. "	;

	}

	switch ( pathNo )
	{
		case 0:
		{
			
			state.desiredBallDirection = IN;
			
			break;
		}
		case 2:
		case 4:
		{
			if( labyrinthState.ballLocationData.wayBlocked[ CW ] )
				state.desiredBallDirection = IN;
			
			else
				state.desiredBallDirection = CW;
			
			break;
		}
		case 1:
		case 3:
		case 5:
		{
			if( labyrinthState.ballLocationData.wayBlocked[ CCW ] )
				state.desiredBallDirection = IN;
			
			else
				state.desiredBallDirection = CCW;
			
			break;
		}
		
	if( debugMode )
	{

		std::cout << "RPHandler: Finding desiredBallDirection completed. "	;
		std::cout << std::endl							;

	}
	
	}
	
	if( debugMode )
	{

		std::cout << "PathNo :: desiredBallDirection : "	;

		std::cout << pathNo << " :: "				;
		std::cout << state.desiredBallDirection			;
		std::cout << std::endl					;
		
	}


	if( debugMode )
	{

		std::cout << "State exporting complete." ;
		std::cout << std::endl;

		std::cout << "---------------------------------------------->";
		std::cout << std::endl;
		std::cout << std::endl;

	}


	return state;

}

/*******************************************************************************
 *
 * Private Methods 
 *
 ******************************************************************************/
