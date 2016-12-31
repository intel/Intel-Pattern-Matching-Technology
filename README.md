# CuriePME API reference

## Initialization Functions

### ``CuriePME.begin()``

```
void CuriePME.begin(void)
```

Initialise the PME so it is ready for operation  
  
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
  
## Basic Functions

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

`0x7FFF` if the input data did not match any of the trained categories.
Otherwise, the trained category assigned by the network will be returned
  
## Saving Knowledge

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
  
## Restoring Knowledge

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
  
## Configuraton Functions

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
Valid values are: 
 
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
Valid values are: 
 
* `LSUP_Distance`
* `L1_Distance`
  
### ``CuriePME.setGlobalContext()``

```
void CuriePME.setGlobalContext(uint16_t context)
```

Writes a value to the Global Context Register. Valid context values range
between 1-127. A context value of 0 enables all neurons, with no regard to
their context
  
*Parameters*  

1. `uint16_t context` : a value between 1-127 representing the desired context
  
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

`uint16_t`, the contents of the Global Context Register (a value between 1-127)
  
## Other Functions

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

`0x7FFF` if the input data did not match any of the trained categories.
Otherwise, the trained category assigned by the network will be returned
