/*
   Copyright (c) 2016 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "CuriePME.h"

Intel_PMT CuriePME;

// default constructor use a begin() method to initialize the instance
Intel_PMT::Intel_PMT()
{

}

// Default initializer
void Intel_PMT::begin(void)
{
	uint16_t savedNSR = regRead16( NSR );
	forget();

	regWrite16( NSR, (uint16_t) NSR_NET_MODE);

	for( int i = 0; i < maxNeurons; i++)
	{
		regWrite16( TESTCOMP, 0 );
	}

	regWrite16( TESTCAT, 0);
	regWrite16( NSR, savedNSR );

}

// custom initializer for the neural network
void Intel_PMT::begin( 	uint16_t global_context,
			PATTERN_MATCHING_DISTANCE_MODE distance_mode,
			PATTERN_MATCHING_CLASSIFICATION_MODE classification_mode,
			uint16_t minAIF, uint16_t maxAIF )
{
	this->begin();

	configure( global_context,
			distance_mode,
			classification_mode,
			minAIF,
			maxAIF );


}

void Intel_PMT::configure( 	uint16_t global_context,
			PATTERN_MATCHING_DISTANCE_MODE distance_mode,
			PATTERN_MATCHING_CLASSIFICATION_MODE classification_mode,
			uint16_t minAIF, uint16_t maxAIF )
{

	regWrite16( GCR , (global_context | (distance_mode << 7)));
	regWrite16( NSR , regRead16( NSR) | (classification_mode << 5) );
	regWrite16( MINIF, minAIF);
	regWrite16( MAXIF, maxAIF);
}

// clear all commits in the network and make it ready to learn
void Intel_PMT::forget( void )
{
	regWrite16( FORGET_NCOUNT, 0 );
}

// mark --learn and classify--

uint16_t Intel_PMT::learn(uint8_t *pattern_vector, int32_t vector_length, uint16_t category)
{
	if( vector_length > maxVectorSize )
		vector_length = maxVectorSize;

	for( int i = 0; i < vector_length -1; i++ )
	{
	 	regWrite16( COMP , pattern_vector[i] );
	}

	regWrite16( LCOMP, pattern_vector[ vector_length - 1 ] );

    /* Mask off the 15th bit-- valid categories range from 1-32766,
     * and bit 15 is used to indicate if a firing neuron has degenerated */
	regWrite16(CAT, (regRead16(CAT) & ~CAT_CATEGORY) | (category & CAT_CATEGORY));
	return regRead16( FORGET_NCOUNT );

}

uint16_t Intel_PMT::classify(uint8_t *pattern_vector, int32_t vector_length)
{


	uint8_t *current_vector = pattern_vector;
	uint8_t index = 0;

	if (vector_length > maxVectorSize) return -1;

	for (index = 0; index < (vector_length - 1); index++)
	{
		regWrite16(COMP, current_vector[index]);
	}
	regWrite16( LCOMP , current_vector[vector_length - 1] );

	return  ( regRead16(CAT) & CAT_CATEGORY);

}

// write vector is used for kNN recognition and does not alter
// the CAT register, which moves the chain along.
uint16_t Intel_PMT::writeVector(uint8_t *pattern_vector, int32_t vector_length)
{


	uint8_t *current_vector = pattern_vector;
	uint8_t index = 0;

	if (vector_length > maxVectorSize) return -1;

	for (index = 0; index < (vector_length - 1); index++)
	{
		regWrite16(COMP, current_vector[index]);
	}
	regWrite16( LCOMP , current_vector[vector_length - 1] );

	return  0;

}

// retrieve the data of a specific neuron element by ID, between 1 and 128.
uint16_t Intel_PMT::readNeuron( int32_t neuronID, neuronData& data_array)
{
	uint16_t dummy = 0;

	// range check the ID - technically, this should be an error.

	if( neuronID < firstNeuronID )
		neuronID = firstNeuronID;
	if(neuronID > lastNeuronID )
		neuronID = lastNeuronID;

	// use the beginSaveMode method
	beginSaveMode();

	//iterate over n elements in order to reach the one we want.

	for( int i = 0; i < (neuronID -1); i++)
	{

		dummy = regRead16( CAT );
	}

	// retrieve the data using the iterateToSave method

	iterateNeuronsToSave( data_array);

	//restore the network to how we found it.
	endSaveMode();

	return 0;
}

// mark --save and restore network--

// save and restore knowledge
void Intel_PMT::beginSaveMode(void)
{
	nsr_save = regRead16(NSR);

	// set save/restore mode in the NSR
	regWrite16( NSR,  regRead16(NSR) | NSR_NET_MODE);
	// reset the chain to 0th neuron
	regWrite16( RSTCHAIN, 0);
}

// pass the function a structure to save data into
uint16_t Intel_PMT::iterateNeuronsToSave(neuronData& array )
{
	array.context =  regRead16( NCR );
	for( int i=0; i < saveRestoreSize; i++)
	{
		array.vector[i] = regRead16(COMP);
	}

	array.influence = regRead16( AIF );
	array.minInfluence = regRead16( MINIF );
	array.category = regRead16( CAT );

	return array.category;
}

void Intel_PMT::endSaveMode(void)
{
	//restore the network to how we found it.
	regWrite16(NSR, (nsr_save &  ~NSR_NET_MODE));
}


void Intel_PMT::beginRestoreMode(void)
{
	nsr_save = regRead16(NSR);

	forget();
	// set save/restore mode in the NSR
	regWrite16( NSR,  regRead16(NSR) | NSR_NET_MODE);
	// reset the chain to 0th neuron
	regWrite16( RSTCHAIN, 0);
}

uint16_t Intel_PMT::iterateNeuronsToRestore(neuronData& array  )
{
	regWrite16( NCR, array.context  );
	for( int i=0; i < saveRestoreSize; i++)
	{
		regWrite16(COMP, array.vector[i]);
	}

	regWrite16( AIF, array.influence );
	regWrite16( MINIF, array.minInfluence );
	regWrite16( CAT, array.category );

	return 0;
}

void Intel_PMT::endRestoreMode(void)
{
	//restore the network to how we found it.
	regWrite16(NSR, (nsr_save & ~NSR_NET_MODE));
}

// mark -- getter and setters--

Intel_PMT::PATTERN_MATCHING_DISTANCE_MODE // L1 or LSup
Intel_PMT::getDistanceMode(void)
{
	return (GCR_DIST & regRead16(GCR)) ? LSUP_Distance : L1_Distance;
}

void
Intel_PMT::setDistanceMode( Intel_PMT::PATTERN_MATCHING_DISTANCE_MODE mode) // L1 or LSup
{
	uint16_t rd = regRead16(GCR);

	// do a read modify write on the GCR register
	regWrite16(GCR, (mode == LSUP_Distance) ? rd | GCR_DIST : rd & ~GCR_DIST);
}

uint16_t Intel_PMT::getGlobalContext(void)
{
	return (GCR_GLOBAL & regRead16(GCR));
}

// valid range is 1-127
void Intel_PMT::setGlobalContext(uint16_t context)
{
	uint16_t gcrMask = ~GCR_GLOBAL & regRead16(GCR);
	gcrMask |= (context & GCR_GLOBAL);
	regWrite16(GCR, gcrMask);
}

uint16_t Intel_PMT::getNeuronContext(void)
{
	return (NCR_CONTEXT & regRead16(NCR));
}

// valid range is 1-127
void Intel_PMT::setNeuronContext(uint16_t context)
{
	uint16_t ncrMask = ~NCR_CONTEXT & regRead16(NCR);
	ncrMask |= (context & NCR_CONTEXT);
	regWrite16(NCR, ncrMask);
}

// NOTE: getCommittedCount() will give inaccurate value if the network is in Save/Restore mode.
// It should not be called between the beginSaveMode() and endSaveMode() or between
// beginRestoreMode() and endRestoreMode()
uint16_t
Intel_PMT::getCommittedCount( void )
{
	return (getFORGET_NCOUNT() & 0xff );
}

Intel_PMT::PATTERN_MATCHING_CLASSIFICATION_MODE
Intel_PMT::getClassifierMode( void ) // RBF or KNN
{
	if( regRead16( NSR ) & NSR_CLASS_MODE )
		return KNN_Mode;

	return RBF_Mode;
}

void
Intel_PMT::setClassifierMode( Intel_PMT::PATTERN_MATCHING_CLASSIFICATION_MODE mode )
{
	uint16_t mask = regRead16(NSR );
	mask &= ~NSR_CLASS_MODE;

	if( mode == KNN_Mode )
		mask |= NSR_CLASS_MODE;

	regWrite16( NSR, mask);
}

// mark --register access--
//getter and setters
uint16_t Intel_PMT::getNCR( void )
{
	return regRead16(NCR);
}

uint16_t Intel_PMT::getCOMP( void )
{
	return regRead16(COMP);
}

uint16_t Intel_PMT::getLCOMP( void )
{
	return regRead16(LCOMP);
}

uint16_t Intel_PMT::getIDX_DIST( void )
{
	return regRead16(IDX_DIST);
}

uint16_t Intel_PMT::getCAT( void )
{
	return regRead16(CAT);
}

uint16_t Intel_PMT::getAIF( void )
{
	return regRead16(AIF);
}

uint16_t Intel_PMT::getMINIF( void )
{
	return regRead16(MINIF);
}

uint16_t Intel_PMT::getMAXIF( void )
{
	return regRead16(MAXIF);
}

uint16_t Intel_PMT::getNID( void )
{
	return regRead16(NID);
}

uint16_t Intel_PMT::getGCR( void )
{
    return regRead16(GCR);
}

uint16_t Intel_PMT::getRSTCHAIN( void )
{
	return regRead16(RSTCHAIN);
}

uint16_t Intel_PMT::getNSR( void )
{
	return regRead16(NSR);
}

uint16_t Intel_PMT::getFORGET_NCOUNT( void )
{
	return regRead16(FORGET_NCOUNT);
}
