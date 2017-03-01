/*
   Copyright (c) 2016 Intel Corporation.  All rights reserved.
   See license notice at end of file.

   This example restores the neural network from a file saved in the storage memory
   on the Arduino/Genuino 101 board that was saved by the previous example sketch.
   When the network is restored, it is able to use the training learned from prior
   examples without retraining it. Thus the prior knowledge is restored.
*/

#include "CuriePME.h"

#include <SerialFlash.h>
#include <SPI.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize Serial communication
  while (!Serial);    // wait for the serial port to open

  // initialize the engine
  CuriePME.begin();

  // Init. SPI Flash chip
  if (!SerialFlash.begin(ONBOARD_FLASH_SPI_PORT, ONBOARD_FLASH_CS_PIN)) {
    Serial.println("Unable to access SPI Flash chip");
  }

  restoreNetworkKnowledge();

  Serial.print("\n Now enter 3 numbers, between 0 and 255, separated by a comma. \n");
  Serial.print("Like 11, 24, 29 \n");
}

void loop() {

  uint8_t vector[3];
  // now we'll classify some unknown data and let the
  // engine decide which pattern of 3 numbers most closely match
  // what it has been taught, or if they don't match anything it knows about.

  int x, y, z;
  while (Serial.available() > 0) {

    x = Serial.parseInt();
    y = Serial.parseInt();
    z = Serial.parseInt();

    // Wait until we see newline or carriage return
    while (!isLineEnding(Serial.read()));

    // Then wait until we stop seeing then
    while (isLineEnding(Serial.peek())) {
      Serial.read();
    }

    vector[0] = constrain(x, 0, 255);
    vector[1] = constrain(y, 0, 255);
    vector[2] = constrain(z, 0, 255);

    int answer = CuriePME.classify(vector, 3 );

    Serial.print("You entered: ");
    printVector(vector);

    if( answer == CuriePME.noMatch ) {
      Serial.print("Which didn't match any of the trained categories.\n");
    } else {
      Serial.print("The closest match to the trained data \n");
      Serial.print("is category: ");
      Serial.print( answer );
      Serial.print("\n");
    }
  }
}

void printVector (uint8_t vector[])
{
    Serial.print(vector[0]);
    Serial.print(",");
    Serial.print(vector[1]);
    Serial.print(",");
    Serial.println(vector[2]);
}

bool isLineEnding (char c)
{
  return (c == '\r' || c == '\n') ? true : false;
}

// This function reads the file saved by the previous example
// The file contains all the data that was learned, then saved before.
// Once the network is restored, it is able to classify patterns again without
// having to be retrained.

void restoreNetworkKnowledge ( void )
{
  const char *filename = "NeurData.dat";
  SerialFlashFile file;
  int32_t fileNeuronCount = 0;

  Intel_PMT::neuronData neuronData;

	// Open the file and write test data
  file = SerialFlash.open(filename);

  CuriePME.beginRestoreMode();
  if (file) {
    // iterate over the network and save the data.
    while(1) {
      Serial.print("Reading Neuron: ");

      uint16_t neuronFields[4];
      file.read( (void*) neuronFields, 8);
      file.read( (void*) neuronData.vector, 128 );

      neuronData.context = neuronFields[0] ;
      neuronData.influence = neuronFields[1] ;
      neuronData.minInfluence = neuronFields[2] ;
      neuronData.category = neuronFields[3];

      if (neuronFields[0] == 0 || neuronFields[0] > 127)
        break;

      fileNeuronCount++;

      // this part just prints each neuron as it is restored,
      // so you can see what is happening.
      Serial.print(fileNeuronCount);
      Serial.print("\n");

      Serial.print( neuronFields[0] );
      Serial.print( "\t");
      Serial.print( neuronFields[1] );
      Serial.print( "\t");
      Serial.print( neuronFields[2] );
      Serial.print( "\t");
      Serial.print( neuronFields[3] );
      Serial.print( "\t");

      Serial.print( neuronData.vector[0] );
      Serial.print( "\t");
      Serial.print( neuronData.vector[1] );
      Serial.print( "\t");
      Serial.print( neuronData.vector[2] );

      Serial.print( "\n");
      CuriePME.iterateNeuronsToRestore( neuronData );
    }
  }

  CuriePME.endRestoreMode();
  Serial.print("Knowledge Set Restored. \n");
}


/*
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
