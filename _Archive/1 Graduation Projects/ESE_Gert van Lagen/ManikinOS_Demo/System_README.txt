Basis software for ManikinOS, performing:
- Synchronisation of modules at system start
- Executing time slot task schedule
- Monitoring time slot task execution 
- Executing task for functional safety

, of the modules:
- Main module: 			SmartHub Controller;
- Measurement module: 		Ventilation Controller;
- Measurement module: 		Compression Controller;

Note:

Quality of synchronisation is not sufficiënt. See test results in thesis "Ontwerpen en ontwikkelen van een ManikinOS" for more details.
Bus monitor controller and communication controller are to be developed, but defines are already created in settings.h and TwiController.ino