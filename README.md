# LDMX Event Building Prototype

## src/ldmx/builder/server

Contains the code for emulating data sources from multiple "contributions".

Main classes:

* Configuration.cc:  LDMX builder server emulation configuration
* Parameters.cc: LDMX builder command line parameter extraction for the LDMX event build server emulator
* RssiConnection.cc: RSSI connection for 1 contributor to the LDMX builder
* RssiContributor.cc: RSSI emulation contributor to LDMX event builder
* RssiServer.cc: Class for the collection RSSI LDMX bulder server emulators builder server emulator
* TriggerServer.cc: Composes and sends trigger batches for the builder server emulator
* LdmxBuilderServer.cc:  LDMX builder command line parameter extraction for the HPS (currently) build server emulator


## src/ldmx/builder/client

Contains the code for receiving the emulated data source contributions and building event fragements.

Main classes:

* Builder.cc : Builds and keeps track of the fragments
* Configuration.cc: ldmx event builder configuration
* ConnectionDsc.cc: Contains all the information needed to establish either any TCP/IP or RSSI connection
* Contribution.cc: The base class for all contribution types to an LDMX Event
* Fragements.cc: The aggregation of data contributions (\e e.g. Trigger and SVT) into a coherent class.  The aggregation is called an Fragment.
* Parameters.cc: LDMX builder command line parameter extraction
* RogueBulder.cc: LDMX event/fragment builder
* RssiConnection.cc: RSSI connection for 1 client of LDMX event builder
* RssiContributor.cc: RSSI Contributor to LDMX event builder
* RssReceiver.cc: Receives contributions from RSSI contributors
* TcpConnections.cc: Maintains the list of and active connections
* TriggerReceiver.cc: Reads trigger messages from the builder server emulator
* LdmxBuilder.cc: Ldmx event/fragment builder

# The executables

Upon building the code several executables will appear in the install directory.

* test_list
* rssi_sink - client stored in pts/rssi_sink - receives test packets
* rssi_send - ptd/rssi_send - sends test packets
* LdmxBatchTest
* LdmxBuilderServer:

can be ran as follows:

```
LdmxBuilderServer –rssi –server=localhost –ports=8192,8193 –rate=2 –nsvtEvents=8 –triggerEvents=8 
```

command line arguments are Parameters: daq/src/ldmx/builder/server/Parameters.hh and configured in the corresponding .cc. These parameters ared used to create a configuration. For emulator we use the rssi option. The server calls ```launch_rssi_servers```. For TCP these is not currently an implementation.

If ```sendTriggerBatch``` is set then a ```TriggerServer``` option is configured.

* LdmxBuilder:

this executable receives the events/triggers sent by the server.

# Extending for LDMX:

## Adding a new Contributor

To add a new contributor e.g. a new detector we need to define the server and client side. This involves creating several custom classes, in the following locations:

Server side:
 * daq/include/ldmx/builder/server/Configuration.hh: here we need to define expected number of events from the new contributors
 * daq/include/ldmx/data/server/- : here we would need a few files for the DetBatchHeader, DetBatch,  DetEvent, DetBatchTail
 * daq/include/ldmx/data/common/- : here we would need the same file names as above redefining to be shared between the server and client
 * daq/src/ldmx/builder/server/DetBatch.hh
 * daq/src/ldmx/data/server/*.cc : here we have the src code for the previously described Det classes

plus additions to other parts of the code (look where svt is defined)
