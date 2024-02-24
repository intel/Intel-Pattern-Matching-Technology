/*
 * This example demonstrates using the pattern matching engine (CuriePME)
 * to classify streams of accelerometer data from CurieIMU.
 *
 * First, the sketch will prompt you to draw some letters in the air (just
 * imagine you are writing on an invisible whiteboard, using your board as the
 * pen), and the IMU data from these motions is used as training data for the
 * PME. Once training is finished, you can keep drawing letters and the PME
 * will try to guess which letter you are drawing.
 *
 * This example requires a button to be connected to digital pin 4
 * https://www.arduino.cc/en/Tutorial/Button
 *
 * NOTE: For best results, draw big letters, at least 1-2 feet tall.
 *
 * Copyright (c) 2016 Intel Corporation.  All rights reserved.
 * See license notice at end of file.
 */

#include "CurieIMU.h"
#include "CuriePME.h"

/*  This controls how many times a letter must be drawn during training.
 *  Any higher than 4, and you may not have enough neurons for all 26 letters
 *  of the alphabet. Lower than 4 means less work for you to train a letter,
 *  but the PME may have a harder time classifying that letter. */
const unsigned int trainingReps = 4;

/* Increase this to 'A-Z' if you like-- it just takes a lot longer to train */
const unsigned char trainingStart = 'A';
const unsigned char trainingEnd = 'F';

/* The input pin used to signal when a letter is being drawn- you'll
 * need to make sure a button is attached to this pin */
const unsigned int buttonPin = 4;

/* Sample rate for accelerometer */
const unsigned int sampleRateHZ = 200;

/* No. of bytes that one neuron can hold */
const unsigned int vectorNumBytes = 128;

/* Number of processed samples (1 sample == accel x, y, z)
 * that can fit inside a neuron */
const unsigned int samplesPerVector = (vectorNumBytes / 3);

/* This value is used to convert ASCII characters A-Z
 * into decimal values 1-26, and back again. */
const unsigned int upperStart = 0x40;

const unsigned int sensorBufSize = 2048;
const int IMULow = -32768;
const int IMUHigh = 32767;

void setup()
{
    Serial.begin(9600);
    while(!Serial);

    pinMode(buttonPin, INPUT);

    /* Start the IMU (Intertial Measurement Unit), enable the accelerometer */
    CurieIMU.begin(ACCEL);

    /* Start the PME (Pattern Matching Engine) */
    CuriePME.begin();

    CurieIMU.setAccelerometerRate(sampleRateHZ);
    CurieIMU.setAccelerometerRange(2);

    trainLetters();
    Serial.println("Training complete. Now, draw some letters (remember to ");
    Serial.println("hold the button) and see if the PME can classify them.");
}

void loop ()
{
    byte vector[vectorNumBytes];
    unsigned int category;
    char letter;

    /* Record IMU data while button is being held, and
     * convert it to a suitable vector */
    readVectorFromIMU(vector);

    /* Use the PME to classify the vector, i.e. return a category
     * from 1-26, representing a letter from A-Z */
    category = CuriePME.classify(vector, vectorNumBytes);

    if (category == CuriePME.noMatch) {
        Serial.println("Don't recognise that one-- try again.");
    } else {
        letter = category + upperStart;
        Serial.println(letter);
    }
}

/* Simple "moving average" filter, removes low noise and other small
 * anomalies, with the effect of smoothing out the data stream. */
byte getAverageSample(byte samples[], unsigned int num, unsigned int pos,
                   unsigned int step)
{
    unsigned int ret;
    unsigned int size = step * 2;

    if (pos < (step * 3) || pos > (num * 3) - (step * 3)) {
        ret = samples[pos];
    } else {
        ret = 0;
        pos -= (step * 3);
        for (unsigned int i = 0; i < size; ++i) {
            ret += samples[pos + (3 * i)];
        }

        ret /= size;
    }

    return (byte)ret;
}

/* We need to compress the stream of raw accelerometer data into 128 bytes, so
 * it will fit into a neuron, while preserving as much of the original pattern
 * as possible. Assuming there will typically be 1-2 seconds worth of
 * accelerometer data at 200Hz, we will need to throw away over 90% of it to
 * meet that goal!
 *
 * This is done in 2 ways:
 *
 * 1. Each sample consists of 3 signed 16-bit values (one each for X, Y and Z).
 *    Map each 16 bit value to a range of 0-255 and pack it into a byte,
 *    cutting sample size in half.
 *
 * 2. Undersample. If we are sampling at 200Hz and the button is held for 1.2
 *    seconds, then we'll have ~240 samples. Since we know now that each
 *    sample, once compressed, will occupy 3 of our neuron's 128 bytes
 *    (see #1), then we know we can only fit 42 of those 240 samples into a
 *    single neuron (128 / 3 = 42.666). So if we take (for example) every 5th
 *    sample until we have 42, then we should cover most of the sample window
 *    and have some semblance of the original pattern. */
void undersample(byte samples[], int numSamples, byte vector[])
{
    unsigned int vi = 0;
    unsigned int si = 0;
    unsigned int step = numSamples / samplesPerVector;
    unsigned int remainder = numSamples - (step * samplesPerVector);

    /* Centre sample window */
    samples += (remainder / 2) * 3;
    for (unsigned int i = 0; i < samplesPerVector; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            vector[vi + j] = getAverageSample(samples, numSamples, si + j, step);
        }

        si += (step * 3);
        vi += 3;
    }
}

void readVectorFromIMU(byte vector[])
{
    byte accel[sensorBufSize];
    int raw[3];

    unsigned int samples = 0;
    unsigned int i = 0;

    /* Wait until button is pressed */
    while (digitalRead(buttonPin) == LOW);

    /* While button is being held... */
    while (digitalRead(buttonPin) == HIGH) {
        if (CurieIMU.dataReady()) {

            CurieIMU.readAccelerometer(raw[0], raw[1], raw[2]);

            /* Map raw values to 0-255 */
            accel[i] = (byte) map(raw[0], IMULow, IMUHigh, 0, 255);
            accel[i + 1] = (byte) map(raw[1], IMULow, IMUHigh, 0, 255);
            accel[i + 2] = (byte) map(raw[2], IMULow, IMUHigh, 0, 255);

            i += 3;
            ++samples;

            /* If there's not enough room left in the buffers
            * for the next read, then we're done */
            if (i + 3 > sensorBufSize) {
                break;
            }
        }
    }

    undersample(accel, samples, vector);
}

void trainLetter(char letter, unsigned int repeat)
{
    unsigned int i = 0;

    while (i < repeat) {
        byte vector[vectorNumBytes];

        if (i) Serial.println("And again...");

        readVectorFromIMU(vector);
        CuriePME.learn(vector, vectorNumBytes, letter - upperStart);

        Serial.println("Got it!");
        delay(1000);
        ++i;
    }
}

void trainLetters()
{
    for (char i = trainingStart; i <= trainingEnd; ++i) {
        Serial.print("Hold down the button and draw the letter '");
        Serial.print(String(i) + "' in the air. Release the button as soon ");
        Serial.println("as you are done.");

        trainLetter(i, trainingReps);
        Serial.println("OK, finished with this letter.");
        delay(2000);
    }
}

/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
