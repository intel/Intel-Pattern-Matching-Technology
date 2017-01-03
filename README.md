# Intel® Pattern Matching Technology

This repository contains the CuriePME library, which provides access to the
Pattern Matching Engine (PME) within the Intel® Curie™ Compute Module.

Supported Curie™ hardware platforms:

* [Arduino/Genuino 101](https://www.arduino.cc/en/Main/ArduinoBoard101)

* [tinyTILE (Farnell)](https://www.element14.com/community/docs/DOC-82913/l/tinytile-intel-curie-module-based-board)

## Development Environments

This library can be used in the following development environments:

* On Ubuntu 14.04 LTS or 16.04 LTS, using the latest version of the
[Curie Open Developer Kit](https://intel.com/curieodk)

* In the Arduino IDE ("Download as a ZIP", unzip, place inside your libraries
  folder)

## About the Library
the PME is a hardware engine capable of learning and recognizing patterns in
arbitrary sets of data. The CuriePME library provides access to the PME,
making it possible to implement machine-learning pattern matching or
classification algorithms which are accelerated by the pattern-matching
capabilities of the PME.

  + Basic Functions Supported:
     * Learning Patterns
     * Recognizing Patterns
     * Storing Pattern Memory (Knowledge) [Requires [SerialFlash](https://github.com/PaulStoffregen/SerialFlash) Library]
     * Retrieving Pattern Memory (Knowledge) [Requires [SerialFlash](https://github.com/PaulStoffregen/SerialFlash) Library]

## About the Intel® Curie™ Pattern Matching Engine

The Pattern Matching Engine (PME) is a parallel data recognition engine with the following features:
+ 128 parallel Processing Elements (PE) each with"

    - 128 byte input vector
    - 128 byte model memory.
    - 8-Bit Arithmetic Units
    - Two distance evaluation norms with 16-bit resolution:

        * L1 norm (Manhattan Distance)
        * Lsup (Supremum) norm (Chebyshev Distance)

    - Support for up to 32,768 Categories
    - Classification states:

           * ID  - Identified
           * UNC - Uncertain
           * UNK - Unknown

+ Two Classification Functions:

     * k-nearest neighbors (KNN)
     * Radial Basis Function (RBF)

+ Support for up to 127 Contexts

# CuriePME API reference

# *Constants*

* ``CuriePME.noMatch (uint32_t)``: The value returned by classify()
  to indicate a pattern could not be classified
* ``CuriePME.minContext (uint16_t)``: Minimum context value
* ``CuriePME.maxContext (uint16_t)``: Maximum context value
* ``CuriePME.maxVectorSize (int32_t)``: Maximum pattern size (in bytes) that can
  be accepted by learn() and classify()
* ``CuriePME.firstNeuronID (int32_t)``: ID of first neuron in network
* ``CuriePME.lastNeuronID (int32_t)``: ID of last neuron in network
* ``CuriePME.maxNeurons (int32_t)``: Number of neurons in the network

# *Initialization Functions*

### ``CuriePME.begin()``

```
void CuriePME.begin(void)
```

Initialize the PME so it is ready for operation
  
*Parameters*  

none  
  
*Return value*  

none  
  
### ``CuriePME.forget()``

```
void CuriePME.forget(void)
```

Clear any data committed to the network, making the network ready to learn again.  
  
*Parameters*  

none  
    
*Return value*  

none  
  
# *Basic Functions*

### ``CuriePME.learn()``

```
uint16_t CuriePME.learn(uint8_t *pattern_vector, int32_t vector_length, uint8_t category)
```  

Takes a pattern `pattern_vector` of size `vector_length`, and commits it to the
network as training data. The `category` parameter indicates to the PME which
category this training vector belongs to- that is, if a future input has a
sufficiently similar pattern, it will be classified as the same category passed
with this pattern.  
  
*Parameters*  
 
1. `uint8_t *pattern_vector` : Pointer to the training data. Training data must
    be no longer than 128 bytes
2. `int32_t vector_length`   : The size (in bytes) of your training vector
3. `uint8_t category`        : The category that should be assigned to this data  
  
*Return value*  

Total number of committed neurons in the network after the learning
operation is complete  
  
### ``CuriePME.classify()``

```
uint16_t CuriePME.classify(uint8_t *pattern_vector, int32_t vector_length)
```

Takes a pattern `pattern_vector` of size `vector_length`, and uses the
committed neurons in the network to classify the pattern  
  
*Parameters*  

1. `uint8_t *pattern_vector` : Pointer to the data to be classified. Pattern
    data must be no longer than 128 bytes
2. `int32_t vector_length`   : The size (in bytes) of the data to be classified
  
*Return value*  

`CuriePME.noMatch` if the input data did not match any of the trained categories.
Otherwise, the trained category assigned by the network will be returned
  
# *Saving Knowledge*

### ``CuriePME.beginSaveMode()``

```
void CuriePME.beginSaveMode(void)
```

Puts the network into a state that allows the neuron contents to be read
  
*Parameters*  

none
  
*Return value*  

none
  
### ``CuriePME.iterateNeuronsToSave()``

```
uint16_t CuriePME.iterateNeuronsToSave(neuronData& data_array)
```

When in save mode, this method can be called to obtain the data for the next
committed neuron. Each successive call will increment an internal pointer and
return the data for successive committed neurons, until all committed neurons
have been read.
  
*Parameters*  

1. `neuronData& data_array` : a `neuronData` type in which the neuron data will
    be placed
  
*Return value*  

0 when all committed neurons have been read. Otherwise, this method returns
the trained category of the neuron being read
  
### ``CuriePME.endSaveMode()``

```
void CuriePME.endSaveMode(void)
```

Takes the network out of save mode
  
*Parameters*  

none
  
*Return value*  

none
  
# *Restoring Knowledge*

### ``CuriePME.beginRestoreMode()``

```
void CuriePME.beginRestoreMode(void)
```

Puts the network into a state that allows the neuron contents to be restored
from a file
  
*Parameters*  

none
  
*Return value*  

none
  
### ``CuriePME.iterateNeuronsToRestore()``

```
void CuriePME.iterateNeuronsToRestore(neuronData& data_array)
```

When in restore mode, this method can be called to write data to the next
available neuron. Each successive call will increment an internal pointer until
all the neurons in the network have been written.
  
*Parameters*  

1. `neuronData& data_array` : a `neuronData` type containing the neuron data
  
*Return value*  

none
  
### ``CuriePME.endRestoreMode()``

```
void CuriePME.endRestoreMode(void)
```

Takes the network out of restore mode
  
*Parameters*  

none
  
*Return value*  

none
  
# *Configuraton Functions*

### ``CuriePME.setClassifierMode()``

```
void CuriePME.setClassifierMode(PATTERN_MATCHING_CLASSIFICATION_MODE mode)
```

Sets the classifying function to be used by the network
  
*Parameters*  

1. `PATTERN_MATCHING_CLASSIFICATION_MODE mode` The classifying function to use.
    Valid values are:  
    * `RBF_Mode` (default)
    * `KNN_Mode`
  
*Return value*  

none
  
### ``CuriePME.getClassifierMode()``

```
PATTERN_MATCHING_CLASSIFICATION_MODE CuriePME.getClassifierMode(void)
```

Gets the classifying function being used by the network
  
*Parameters*  

none
  
*Return value*  

`PATTERN_MATCHING_CLASSIFICATION_MODE mode` The classifying function being used.
Possible values are: 
 
* `RBF_Mode`
* `KNN_Mode`
  
### ``CuriePME.setDistanceMode()``

```
void CuriePME.setDistanceMode(PATTERN_MATCHING_DISTANCE_MODE mode)
```

Sets the distance function to be used by the network
  
*Parameters*  

1. `PATTERN_MATCHING_DISTANCE_MODE mode` The distance funcion to use.
    Valid values are:  

    * `LSUP_Distance` (default)
    * `L1_Distance`
  
*Return value*  

none
  
### ``CuriePME.getDistanceMode()``

```
PATTERN_MATCHING_DISTANCE_MODE CuriePME.getDistanceMode(void)
```

Gets the distance function being used by the network
  
*Parameters*  

none
  
*Return value*  

`PATTERN_MATCHING_DISTANCE_MODE mode` The distance function being used.
Possible values are: 
 
* `LSUP_Distance`
* `L1_Distance`
  
### ``CuriePME.setGlobalContext()``

```
void CuriePME.setGlobalContext(uint16_t context)
```

Writes a value to the Global Context Register
  
*Parameters*  

1. `uint16_t context` : a value between 0-127 representing the desired context.
   A context value of 0 selects all neurons, regardless of their context value.
  
*Return value*  

none
  
### ``CuriePME.getGlobalContext()``

```
uint16_t CuriePME.getGlobalContext(void)
```

Reads the Global Context Register
  
*Parameters*  

none
  
*Return value*  

`uint16_t`, the contents of the Global Context Register (a value between 0-127)
  
# *Other Functions*

### ``CuriePME.getCommittedCount()``

```
uint16_t CuriePME.getCommittedCount(void)
```

Gets the number of committed neurons in the network (NOTE: this method should
not be used in save/restore mode, because it will give inaccurate results)
  
*Parameters*  

none
  
*Return value*  

`uint16_t`, the number of comitted neurons in the network
  
### ``CuriePME.readNeuron()``

```
void CuriePME.readNeuron(int32_t neuronID, neuronData& data_array)
```

Read a specific neuron by its ID
  
*Parameters*  

1. `int32_t neuronID` : value between 1-128 representing a specific neuron
2. `neuronData& data_array` : neuronData type in which to write the neuron data
  
*Return value*  

none
  
### ``CuriePME.writeVector()`` (KNN_Mode only)

```
uint16_t CuriePME.writeVector(uint8_t *pattern_vector, int32_t vector_length)
```

(Should only be used in `KNN_Mode`) Takes a pattern `pattern_vector` of size
`vector_length`, and uses the committed neurons in the network to classify the
pattern
  
*Parameters*  

1. `uint8_t *pattern_vector` : Pointer to the data to be classified. Pattern
    data must be no longer than 128 bytes
2. `int32_t vector_length`   : The size (in bytes) of the data to be classified
  
*Return value*  

`CuriePME.noMatch` if the input data did not match any of the trained categories.
Otherwise, the trained category assigned by the network will be returned
