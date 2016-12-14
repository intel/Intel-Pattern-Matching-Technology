/*
   Copyright (c) 2016 Intel Corporation.  All rights reserved.
   See license notice at end of file.
*/

// This Example illustrates how to train the Intel(r) Curie(tm) pattern matching engine with
// example data and how to tell how many neurons are committed in the network.

#include "CuriePME.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize Serial communication
  while (!Serial);    // wait for the serial port to open

  // initialize the engine
  CuriePME.begin();

  trainNeuronsWithData();
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

// The pattern matching engine will commit a new neuron for each new category.
// Adding additional data to a category may or may not commit more neurons.
// If the training data is relatively close together and well separated
// from other committed neurons, it usually will not commit an additional
// neuron. Widely separated data sets will usually commit more neurons per category.
void trainNeuronsWithData( void )
{
  Serial.print("Neurons committed before learning = ");
  Serial.print( CuriePME.getCommittedCount());
  Serial.print("\n");

  commitThreeSamples(1, 11, 24, 29);    // Category 1
  commitThreeSamples(2, 18, 75, 38);    // Category 2
  commitThreeSamples(3, 2, 56, 35);     // Category 3
  commitThreeSamples(4, 111, 224, 229); // Category 4
  commitThreeSamples(5, 128, 200, 255); // Category 5
  commitThreeSamples(6, 99, 180, 201);  // Category 6

  Serial.print("Neurons committed after learning = ");
  Serial.print( CuriePME.getCommittedCount());
  Serial.print("\n");

  Serial.print("Now enter 3 numbers, between 0 and 255, separated by a comma. \n");
  Serial.print("Like 11, 24, 29 \n");
}

void commitThreeSamples (int category, uint8_t s1, uint8_t s2, uint8_t s3)
{
  uint8_t vector[3];

  vector[0] = s1;
  vector[1] = s2;
  vector[2] = s3;

  // give the data, the number of elements and the category it belongs to.
  CuriePME.learn(vector, 3, category);

  Serial.print("Category ");
  Serial.print( category );
  Serial.print(" trained with: ");
  printVector(vector);
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
