/*******************************************************************************
 *
 * LMHandler.cpp - Source File
 *
 * Description	:	Moonlight Company
 *			Maze Solver Project
 *			Labyrinth Mapper Handler Class
 *
 * Modified	: 12.05.2016 by Doga Yucalan ( yucalan.doga@metu.edu.tr ))
 * Created	: 13.04.2016 by Utku Norman  ( norman.utku@metu.edu.tr  )
 *
 * People	:	Doga Yucalan ( yucalan.doga@metu.edu.tr )
 *			Utku Norman  ( norman.utku@metu.edu.tr  )
 *
 * Associated code snippet sources :
 *
 *  http://opencv-code.com/tutorials/detecting-simple-shapes-in-an-image/
 *  https://github.com/bsdnoobz/opencv-code/blob/master/shape-detect.cpp
 *  http://stackoverflow.com/questions/
 * 	1882047/convention-result-and-code-error-c-int-foo
 *
 ******************************************************************************/

// Command Line I/O related headers
#include <cstddef>  			// std::nullptr
#include <iostream> 			// std::cout, std::cin, std::endl
#include <iomanip>  			// std::setw
#include <math.h>			// std::acos

#include <opencv2/opencv.hpp>		// Image Processing (OpenCV) header

#include "../include/LMHandler.h"

#include "../include/Visualizer.h"

/*******************************************************************************
 *
 * Constructor
 *
 ******************************************************************************/

LMHandler::LMHandler( void )
{

}

/*******************************************************************************
 *
 * Public Methods 
 *
 ******************************************************************************/

void LMHandler::printState(
	bool printMode 		, 
	bool displayMode 
	)
{

	if( printMode )
	{
		
		std::cout << "<!---------------------------------------------";
		std::cout << std::endl;
		std::cout << "LMHandler.printState() called. Printing state:" ;
		std::cout << std::endl;

	}

	if ( IsNull( state ) )
	{

		std::cerr << "LMHandler: state is Null";	
		std::cerr << std::endl;

		return;

	}
	else
	{

		if( printMode )
		{

			std::cout << std::endl			;

			std::cout << "Listing state: "		;
			std::cout << std::endl			;


			if ( IsNull( state.ballLocationData ) )
			{

				std::cerr << "LMHandler: state.ballLocationData is Null";	
				std::cerr << std::endl;

			}
			else
			{
				std::cout << "ballLocationData: "	;
				std::cout << std::endl;

				std::cout << "pathNo: "			;
				std::cout << state.ballLocationData.currentPathNo ;
				std::cout << std::endl;

				std::cout << "Way blocked[]: "		;
				std::cout << " 0: " 			;
				std::cout << state.ballLocationData.wayBlocked[ 0 ] ;
				std::cout << " 1: " 			;
				std::cout << state.ballLocationData.wayBlocked[ 1 ] ;
				std::cout << " 2: " 			;
				std::cout << state.ballLocationData.wayBlocked[ 2 ] ;
				std::cout << " 3: " 			;
				std::cout << state.ballLocationData.wayBlocked[ 3 ] ;
				std::cout << std::endl			;

			}
			
			std::cout << "ballPositionAngle: "	;
			std::cout << state.ballPositionAngle	;
			std::cout << std::endl;

			std::cout << "annotatedImage: " 	;
			std::cout << std::endl			;

		}

		if ( !annotatedImage.data )
		{

			std::cerr <<"LMHandler: No annotatedImage data!" ;
			std::cerr << std::endl ;

		}
		else
		{
			

			if( printMode )
			{

				std::cout << "Showing (LM-annotatedImage): ";
				std::cout << std::endl;

			}

			if( displayMode )
			{

				showImage( "LM-annotatedImage" , annotatedImage );

			}

		}

	}

	if( printMode )
	{
		
		std::cout << "---------------------------------------------->";
		std::cout << std::endl;
		std::cout << std::endl;

	}

}

LMPacket LMHandler::handle( 
	cv::Point2f 	borderPoints[ 3 ] 	,
	MazeData 	mazeData 		, 
	bool debugMode
	)
{

	if( debugMode )
	{

		std::cout << std::endl						;
		std::cout << "<!---------------------------------------------"	;

		std::cout << std::endl						;

		std::cout << "LMHandler.handle() called. handling..."		;
		std::cout << std::endl						;

	}

	if ( IsNull( mazeData ) )
	{

		std::cerr << "LMHandler: mazeData of type MazeData is Null"	;
		std::cerr << std::endl						;

		return LMPacketNull						;

	}

	if ( !mazeData.image.data )
	{

		std::cerr << "LMHandler: No mazeImage data!" 			;
		std::cerr << std::endl 						;

		return LMPacketNull						;

	}

	// Declare local variables
	cv::Mat 	annotatedImage				;
	cv::Mat 	processingImage				;
	cv::Point2f	startLocation				;
	cv::Point2f	endLocation				;
	cv::Point2f	ballLocation				;
	cv::Point2f	centerLocation				;
	
	cv::Point2f	criticalPoints[ 6 ]			;
	float		criticalAngles[ 6 ]			;
	float 		zeroAngle				;
	float		ballAngle				;
	int		sextantIndex				;
	float		scalingArray[ 5 ]			;
	float		normalizedBallAngle			; // our guy!

	std::vector< std::vector< cv::Point > > contours	;
	std::vector< std::vector< cv::Point > > approxContours	;
	std::vector< cv::Vec4i > hierarchy			;

	//std::vector< cv::Point > boundingTriangleVertices 	;
	int boundingTriangleIndex ;

	// Initialize local variables
	annotatedImage	= mazeData.image.clone( )		;
	startLocation	= mazeData.mazePoints[ 0 ] 		;
	endLocation	= mazeData.mazePoints[ 1 ] 		;
	ballLocation	= mazeData.mazePoints[ 2 ] 		;
	

	centerLocation = startLocation ;
	criticalPoints[ 0 ] = borderPoints[ 0 ] ; // one end of upos
	criticalPoints[ 2 ] = borderPoints[ 1 ] ; // one end of vpos
	criticalPoints[ 4 ] = borderPoints[ 2 ] ; // one end of wpos
	
	criticalPoints[ 1 ] = findMidPoint( criticalPoints[ 0 ] , criticalPoints[ 2 ] ); // one end of wneg
	criticalPoints[ 3 ] = findMidPoint( criticalPoints[ 2 ] , criticalPoints[ 4 ] ); // one end of uneg
	criticalPoints[ 5 ] = findMidPoint( criticalPoints[ 0 ] , criticalPoints[ 4 ] ); // one end of vneg

 	for ( int i = 0 ; i < 6 ; i++ )
 	{

 		std::stringstream ss;
		ss << std::to_string( i ) << std::fixed << std::setprecision( 0 ) << "(";
		ss <<  criticalPoints[ i ].x << "," << criticalPoints[ i ].y << ")";
		std::string label = ss.str();

		/*
		std::string label	;

	 	label.assign( std::to_string( i ) );
	 	label += "- ( ";
	 	label += std::to_string( criticalPoints[ i ].x ); 
	 	label += " ,  ";
	 	label += std::to_string( criticalPoints[ i ].y ); 
	 	label += " )";
		*/

 		setLabel( annotatedImage , label , criticalPoints[ i ]	)	; // start


 	}

 	/*
	setLabel( annotatedImage , "0" , criticalPoints[ 0 ]	)	; // start
	setLabel( annotatedImage , "1" , criticalPoints[ 1 ]	)	; // start
	setLabel( annotatedImage , "2" , criticalPoints[ 2 ]	)	; // start
	setLabel( annotatedImage , "3" , criticalPoints[ 3 ]	)	; // start
	setLabel( annotatedImage , "4" , criticalPoints[ 4 ]	)	; // start
	setLabel( annotatedImage , "5" , criticalPoints[ 5 ]	)	; // start
	*/

	// Draw a line in between S & B
	cv::line( annotatedImage , centerLocation , criticalPoints[ 0 ] , CV_RGB ( 0 , 255 , 255 ) ,  1 ) ;
	cv::line( annotatedImage , centerLocation , criticalPoints[ 1 ] , CV_RGB ( 0 , 255 , 255 ) ,  1 ) ;
	cv::line( annotatedImage , centerLocation , criticalPoints[ 2 ] , CV_RGB ( 0 , 255 , 255 ) ,  1 ) ;
	cv::line( annotatedImage , centerLocation , criticalPoints[ 3 ] , CV_RGB ( 0 , 255 , 255 ) ,  1 ) ;
	cv::line( annotatedImage , centerLocation , criticalPoints[ 4 ] , CV_RGB ( 0 , 255 , 255 ) ,  1 ) ;
	cv::line( annotatedImage , centerLocation , criticalPoints[ 5 ] , CV_RGB ( 0 , 255 , 255 ) ,  1 ) ;

	ballAngle = findRelativeAngle( centerLocation, ballLocation);
	
	if( debugMode )
	{

		std::cout << "Critical points importing complete. " ;
		std::cout << std::endl;

		std::cout << "Critical points:";
		std::cout << std::endl;
		for( int i = 0 ; i < 6 ; i++ )
		{
			std::cout << "[ " << i << " ] : ";
			std::cout << criticalPoints[ i ];
			std::cout << std::endl;
		
		}

		std::cout << "Ball angle:";
		std::cout << ballAngle << std::endl;
		
	}
	
	
	if( debugMode )
	{

		std::cout << "Calculating critical angles. " ;
		std::cout << std::endl;
		
	}
	
	criticalAngles[ 0 ] = findRelativeAngle( centerLocation, criticalPoints[ 0 ]); // upos
	criticalAngles[ 1 ] = findRelativeAngle( centerLocation, criticalPoints[ 1 ]); // wneg
	criticalAngles[ 2 ] = findRelativeAngle( centerLocation, criticalPoints[ 2 ]); // vpos
	criticalAngles[ 3 ] = findRelativeAngle( centerLocation, criticalPoints[ 3 ]); // uneg
	criticalAngles[ 4 ] = findRelativeAngle( centerLocation, criticalPoints[ 4 ]); // wpos
	criticalAngles[ 5 ] = findRelativeAngle( centerLocation, criticalPoints[ 5 ]); // vneg
	
	zeroAngle = criticalAngles[ 0 ]; // uneg is our zero
	
	if( debugMode )
	{

		std::cout << "Calculating critical angles complete. " ;
		std::cout << std::endl;
		std::cout << "Critical angles:";
		std::cout << std::endl;
		
		for( int i = 0 ; i < 6 ; i++ )
		{
			std::cout << "[ " << i << " ] : ";
			std::cout << criticalAngles[ i ];
			std::cout << std::endl;
		
		}
		
		std::cout << "Zero angle: ";
		std::cout << zeroAngle;
		std::cout << std::endl;

	}
	
	if( debugMode )
	{

		std::cout << "Finding sextant index. " ;
		std::cout << std::endl;
		
	}
		
	sextantIndex = -1 ;

	for( int i = 0 ; i < 6 ; i++ )
	{

		if( ballAngle >= criticalAngles[ i ] )
		{

			sextantIndex = i ;

		}

	}

	if( sextantIndex == -1 )
	{

		std::cerr << "LMHandler: sextantIndex estimation failed" ;	
		std::cerr << std::endl ;

		return LMPacketNull ;

	}

	
	if( debugMode )
	{

		std::cout << "Finding sextant index complete. " ;
		std::cout << std::endl;
		std::cout << "Sextant index: ";
		std::cout << sextantIndex;
		std::cout << std::endl;
		
	}
	
	if( debugMode )
	{

		std::cout << "Updating critical angles. " ;
		std::cout << std::endl;
		
	}
	
	for( int i = 0 ; i < 6 ; i++)
	{

		criticalAngles[ i ] -= zeroAngle ;

		criticalAngles[ i ] = ( criticalAngles[ i ] >= 0 ? 0 : 360 ) + criticalAngles[ i ];

	}

	ballAngle -= zeroAngle ;

	ballAngle = ( ballAngle >= 0 ? 0 : 360 ) + ballAngle;
		
	if( debugMode )
	{

		std::cout << "Updating critical angles complete. " ;
		std::cout << std::endl;
		std::cout << "Updated critical angles:";
		std::cout << std::endl;
		for( int i = 0 ; i < 6 ; i++ )
		{
			
			std::cout << "[ " << i << " ] : ";
			std::cout << criticalAngles[ i ];
			std::cout << std::endl;
		
		}
	}	
	
	if( debugMode )
	{

		std::cout << "Finding scaling parameters. " ;
		std::cout << std::endl;
		
	}
	
	for( int i = 0; i < 6; i++ )
	{

		scalingArray[ i ] = 60 / ( criticalAngles[ (i + 1) % 6 ] - criticalAngles[ i ] ) ;

	}
	
	if( debugMode )
	{

		std::cout << "Finding scaling parameters complete. " ;
		std::cout << std::endl;
		std::cout << "Scaling parameters:";
		std::cout << std::endl;
		for( int i = 0 ; i < 5 ; i++ )
		{
			std::cout << "[ " << i << " ] : ";
			std::cout << scalingArray[ i ];
			std::cout << std::endl;
		
		}
	}
	
	if( debugMode )
	{

		std::cout << "Finding normalized ball angle. " ;
		std::cout << std::endl;
		
	}
	
	normalizedBallAngle = sextantIndex * 60 + ( ballAngle - criticalAngles[ sextantIndex ] ) * scalingArray[ sextantIndex ];
	
	if( debugMode )
	{

		std::cout << "Finding normalized ball angle complete. " ;
		std::cout << std::endl;
		std::cout << "Ball angle: ";
		std::cout << ballAngle;
		std::cout << std::endl;
		std::cout << "Normalized ball angle: ";
		std::cout << normalizedBallAngle;
		std::cout << std::endl;
		
	}
	
	if( normalizedBallAngle < 0 || normalizedBallAngle > 360 )
	{

		std::cerr << "LMHandler: normalizedBallAngle is out of bounds!" ;	
		std::cerr << std::endl ;

		return LMPacketNull ;

	}

	extractContours( mazeData.image , processingImage , contours, approxContours, hierarchy );
	
	if( debugMode )
	{

		std::cout << "Contour extraction complete. Number of approx. contours found: " ;
		std::cout << approxContours.size() ;
		std::cout << std::endl;

	}

	/*
	boundingTriangleIndex = findBoundingTriangle( approxContours ) ;

	if( boundingTriangleIndex == -1 )
	{

		return LMPacketNull;

	}


	if( debugMode )
	{

		std::cout << "The bounding triangle found at index: " ;
		std::cout << std::to_string( boundingTriangleIndex ) ;
		std::cout << std::endl;

		std::cout << "The related hierarchy record: " ;
		std::cout << hierarchy[ boundingTriangleIndex ] ;
		std::cout << std::endl;

		std::cout << "The child record: " ;
		std::cout << hierarchy[ boundingTriangleIndex ][ 2 ] ;
		std::cout << std::endl;

	}


	
	// Annote all vertices of the approximation curve vector and extract the vertices
	for( std::vector< cv::Point >::size_type i = 0 ; i != 3 ; i++ ) 
	{

		setLabel( annotatedImage , "V" , approxContours[ boundingTriangleIndex ] );

	}

	*/

	/*

	// Annote all vertices of the approximation curve vector and extract the vertices
	// hierarchy[i][2] -> first child
	int mazeBoundIndex = hierarchy[ boundingTriangleIndex ][ 2 ];
	for( std::vector< cv::Point >::size_type i = 0 ; i != 3 ; i++ ) 
	{

		setLabel( annotatedImage , "M" , 
			approxContours[ mazeBoundIndex ] );

	}
	*/

	cv::Mat dummyImage	;

	// Delete everything outside the triangle
	cv::Mat mask = cv::Mat::zeros( processingImage.size( ) , CV_8UC1 );

	//std::vector< std::vector< cv::Point > > dummyContours ;
	//dummyContours.push_back( contours );

	//cv::drawContours( mask , contours , -1 ,  cv::Scalar( 255 ) , CV_FILLED );
	cv::drawContours( mask , contours , -1 ,  cv::Scalar( 255 ) , CV_FILLED );

	if( debugMode )
	{
		std::cout << "Search for contours is complete. Displaying the results:" ;
		std::cout << std::endl;

		showImage( "LM-mask" , mask );

	}

	processingImage.copyTo( dummyImage , mask ) ;
	processingImage = dummyImage.clone( ) ;
	//processingImage.copyTo( processingImage , mask ) ;

	//annotatedImage.copyTo( dummyImage , mask ) ;
	//annotatedImage 	= dummyImage.clone( ) ;

	//setLabel( annotatedImage , "0" , contours[ 0 ] ) ;


	if( debugMode )
	{

		std::cout << "Masking complete. Displaying the results:" ;
		std::cout << std::endl;

		showImage( "LM-maskedProcessingImage" , processingImage );

	}

//ballLocation centerocation


	if( debugMode )
	{

		//std::cout << "Search for contours is complete. Displaying the results:" ;
		//std::cout << std::endl;

		/*
		displayContours(
			"LM-Contours on processingImage" , 
			processingImage.clone() , contours , hierarchy , // approxContours
			false , true ); 
			// true ); // for enumeration , for contour only mode
		*/

		/*
		displayContours(
			"LM-Contours on mazeImage" , 
			mazeData.image.clone() , contours , hierarchy , // approxContours
			false , false ); 
		*/
			
		/*
		displayContours(
			"LM-approxContours on processingImage" , 
			processingImage.clone() , approxContours , hierarchy , // approxContours
			false , true ); 
			// true ); // for enumeration , for contour only mode

		displayContours(
			"LM-approxContours on mazeImage" , 
			mazeData.image.clone() , approxContours , hierarchy , // approxContours
			false , false ); */

	}

	//slope = startLocation.y - ballLocation

	//Point2f a(0.3f, 0.f), b(0.f, 0.4f);
	//Point pt = (a + b)*10.f;

	// Label the annotatedImage
	setLabel( annotatedImage , "S" , centerLocation	)	; // start
	setLabel( annotatedImage , "E" , endLocation	)	; // end
	setLabel( annotatedImage , "B" , ballLocation	)	; // ball

	// Draw a line in between S & B
	cv::line( annotatedImage , centerLocation , ballLocation , CV_RGB ( 0 , 255 , 255 ) ,  1 ) ;

	//double radius = cv::norm( startLocation - ballLocation ) ;//Euclidian distance
	//cv::circle( annotatedImage, startLocation, radius, CV_RGB ( 0 , 255 , 0 ) ,  1 ) ;

	if( debugMode )
	{

		std::cout << "Annotating feature points and drawing line complete." ;
		std::cout << std::endl;

	}


	/*
	CountNonZero(bitwise_and(ShapeAImage,ShapeBImage)).
	*/
	
	////////////////////////////////////////////////////////
	/////////////////////// EXPORTING //////////////////////
	////////////////////////////////////////////////////////

	// Export private variables
	this -> annotatedImage = annotatedImage.clone( )		;

	// Exporting state variables
	state.ballLocationData.wayBlocked[ 0 ]	= false			;
	state.ballLocationData.wayBlocked[ 1 ]	= false			;
	state.ballLocationData.wayBlocked[ 2 ]	= false			;
	state.ballLocationData.wayBlocked[ 3 ]	= false			;

	state.ballLocationData.currentPathNo	= 5			;

	state.ballPositionAngle			= normalizedBallAngle	;
	
	////////////////////////////////////////////////////////
	////////////////// EXPORTING COMPLETE //////////////////
	////////////////////////////////////////////////////////


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
void LMHandler::annotateState( 
	cv::Mat& mat, 
	MazeData state 
	)
{
	
}

/*
int LMHandler::findBoundingTriangle( const std::vector< std::vector< cv::Point > >& contours )
{

	int 	index 		= 0	;
	double 	measure 	= 0 	;
	double 	newMeasure 		;

	// Find the bounding triangle
	for ( std::vector< cv::Point >::size_type i = 0 ; i < contours.size() ; i++ )
	{


		if ( contours[i].size( ) == 3 )
		{

			newMeasure = cv::contourArea( contours[ i ] , true ) ;

			if( newMeasure > measure ) 
			{

				measure = newMeasure ;

				index = i ;

			}

		}

	
	}

	if( measure == 0 )
	{

		std::cerr << "LMHandler: bounding triangle not found" ;	
		std::cerr << std::endl ;

		return -1 ;

	}

	return index ;

}
*/

void LMHandler::extractContours(
	const cv::Mat inputImage,
	cv::Mat& outputImage,
	std::vector< std::vector< cv::Point > >& contours,
	std::vector< std::vector< cv::Point > >& approxContours,
	std::vector< cv::Vec4i >& hierarchy
	)
{

	// Apply grayscaling, BGR -> GRAY
	cv::cvtColor( inputImage , outputImage , CV_BGR2GRAY ) ;

	/*
	int dilation_type = 2;
	if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
	else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
	else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
	*/

	/// Apply the dilation operation
	//cv::dilate( outputImage, outputImage, element );

    cv::blur( outputImage , outputImage , cv::Size( 3 , 3 ) );

	// Apply thresholding, thresholds may be adjusted

	//cv::threshold(outputImage, outputImage, 160, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	//cv::threshold(outputImage, outputImage, 20, 255, CV_THRESH_BINARY_INV );

	cv::Canny( outputImage , outputImage , 110 , 250 ) ;
	//180 , 350 );
	//, 100 , 500 ) ;

	// Set threshold and maxValue
	//double thresh = 120;
	//double maxValue = 255; 
	 
	// Binary Threshold
	//cv::threshold( outputImage , outputImage , thresh, maxValue, 0 ) ;//THRESH_BINARY );

	cv::findContours( outputImage , contours , hierarchy , CV_RETR_TREE , CV_CHAIN_APPROX_NONE ) ;

	std::vector< cv::Point > approxContour;
	double epsilon;

	for ( std::vector< cv::Point >::size_type i = 0 ; i < contours.size() ; i++ )
	{
		// Approximate contour with accuracy proportional to the contour perimeter
		epsilon = cv::arcLength( contours[ i ] , true ) * 0.2 ; //* 0.02 ;

		cv::approxPolyDP( contours[ i ] , approxContour , epsilon , true ) ;

		approxContours.push_back( approxContour );

	}

}

cv::Point2f LMHandler::findMidPoint(
	cv::Point2f vertex1,
	cv::Point2f vertex2 )
{
	cv::Point2f midPoint;
	
	midPoint.x = ( vertex1.x + vertex2.x ) / 2;
	midPoint.y = ( vertex1.y + vertex2.y ) / 2;
	
	return midPoint;
}

float LMHandler::findRelativeAngle(
	cv::Point2f start,
	cv::Point2f end)
{
	float angle;
	
	angle = std::atan2( -1 * ( end.y - start.y ) , end.x - start.x );
	
	// convert from radians to degrees
	angle = angle * 180 / PI;
	
	// mod 360
	

	
	return angle;
}
