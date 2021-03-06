/*******************************************************************************
 *
 * CUHandler.cpp - Source File
 *
 * Description	:	Moonlight Company
 *			Maze Solver Project
 *			Controller Unit Handler Class
 *
 * Modified	: 12.05.2016 by Utku Norman  ( norman.utku@metu.edu.tr  )
 * Created	: 24.04.2016 by Utku Norman  ( norman.utku@metu.edu.tr  )
 *
 * People	:	Doga Yucalan ( yucalan.doga@metu.edu.tr )
 *			Utku Norman  ( norman.utku@metu.edu.tr  )
 *
 ******************************************************************************/

// Command Line I/O related headers
#include <string>  			// std::nullptr
#include <cstddef>  			// std::nullptr
#include <iostream> 			// std::cout, std::cin, std::endl
#include <iomanip>  			// std::setw

#include "../include/CUHandler.h"

/*******************************************************************************
 *
 * Constructor
 *
 ******************************************************************************/

CUHandler::CUHandler( void )
{

	// Default package details
	commandUp.assign( "W" )		;

	commandDown.assign( "S" )	;

	// Default package details
	senseDataTop.assign( "T" )	;

	senseDataInRange.assign( "N" )	;

	senseDataBottom.assign( "B" )	;

	commandState.data.assign( senseDataInRange ) ;

	state.data.assign( commandUp );

	//state.assign( "" );



/*
	// Default package details
	CommandUp.data.assign( "W" );

	CommandDown.data.assign( "S" );

	// Default package details
	SenseDataTop.data.assign( "T" )		;

	SenseDataInRange.data.assign( "N" )	;

	SenseDataBottom.data.assign( "B" )	;
*/

}

/*******************************************************************************
 *
 * Public Methods
 *
 ******************************************************************************/
 
void CUHandler::printState(
	bool printMode 		, 
	bool displayMode 
	)
{

	if( printMode )
	{
		
		std::cout << "<!---------------------------------------------";
		std::cout << std::endl;
		std::cout << "CUHandler.printState() called. Printing state:" ;
		std::cout << std::endl;

	}

	//if ( IsNull( state ) )
	if( state.data.compare( "" ) == 0 )  // if empty
	{

		std::cerr << "state is Null";	
		std::cerr << std::endl;

		return;

	}
	else
	{

		if( printMode )
		{
			
			std::cout << "state length:" << state.data.length() ;
			std::cout << std::endl			;
			std::cout << "state:" << state.data	;
			std::cout << std::endl			;

		}

	}

	if( printMode )
	{
		
		std::cout << "---------------------------------------------->";
		std::cout << std::endl;
		std::cout << std::endl;

	}

}

CUPacket CUHandler::handle( 
	DDPacket errorInfo	,
	PSMPacket platformState	,
	CHPacket senseDataInfo	,
	bool debugMode
	)
{

	if( debugMode )
	{

		std::cout << std::endl	;
		std::cout << "<!---------------------------------------------"	;
		std::cout << std::endl	;
		std::cout << "CUHandler.handle() called. handling..."	;
		std::cout << std::endl	;

	}
	
	// Declare local variables
	std::string senseData;
	float errorHeight;
	int numberOfIterations;
	std::string commandUnit;

	// Import local variables
	senseData = senseDataInfo.data ;
	errorHeight = errorInfo.nextState;

	// Initialize local variables
	state.data.assign( " " );
	numberOfIterations = errorHeight / 10 + 1;

	if( errorHeight > 0 )
	{

		commandUnit.assign( commandUp ) ;

	}
	else
	{
		commandUnit.assign( commandDown ) ;

	}

	
	for( int i = 0 ; i < numberOfIterations ; i++ )
	{

		state.data += commandUnit ;

	}

	if( debugMode )
	{

		std::cout << "---------------------------------------------->"	;
		std::cout << std::endl	;
		std::cout << std::endl	;

	}

	return state;

	/*

	std::string command;
	std::string state;

	// Initialize local variables
	state.assign( "" );
	command.assign( "" );
	*/

	/*

	// Receive sense data

	if( senseData.compare( "" ) != 0 )  		// if not empty
	{

		commandState.data.assign( senseData ) ;

	}

	// Decide on the next command

	if( commandState.data.compare( senseDataTop )  == 0 )	// if senseDataTop
	{

		state.data.assign( commandDown ) ;
		

	}
	else if( commandState.data.compare( senseDataInRange )  == 0 ) // if senseDataInRange
	{

		// do nothing


	}
	else if( commandState.data.compare( senseDataBottom )  == 0 ) // if senseDataBottom
	{

		state.data.assign( commandUp ) ;

	}
	else
	{

		if( debugMode )
		{

			std::cout << "unknown command" ;
			std::cout << std::endl ;	

		}

	}

	if( debugMode )
	{

		std::cout << "---------------------------------------------->"	;
		std::cout << std::endl	;
		std::cout << std::endl	;

	}

	return state;

	*/

	/*
		if( commandState.data.compare( senseDataTop )  == 0 ) // if senseDataTop
		{

			state.assign( CommandDown );

		}

	// Declare local variables
	CUPacket Command;
	CUPacket state;

	state = CUPacketNull;

	// Algorithm
	if( !IsNull( senseData ) )
	{
		if( senseData.data == SenseDataTop.data )
		{



		}


	}
	*/

}

/*******************************************************************************
 *
 * Private Methods
 *
 ******************************************************************************/
