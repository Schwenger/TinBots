scribe: Maximilian Schwenger, 11.05.2016, 10-12 session

* Tutor’s recommendation: GPS fails
* Tutor’s recommendation: Radio/Sound instead of IR -> E-Puck sound -> cartography less precise
Microphone already in E-Puck, only sound emitter needed.
Bluetooth communication between e-Pucks

Victim not on e-Puck -> use magnets instead and push them out.

hardware:
=========

2 E-Pucks, Raspberry Pi, charger missing, code transmitter
MicroUSB for Raspberry Pi

[Webcam from Ben]
Camera from chair direct connection to Pi (camera model 1)

Picamera lib for python

SD card missing in Pi! -> ask Hazem -> we’ll get it later.
At least 8 GiB for now, maybe we need less.

Discuss how we want to combine Teddy and IR emitter.  ATmega8 should suffice? -> Agreed on ATtiny2313 + 4 IR emitter
Test whether “our” IR emitter works through various kinds of paper.
Normal paper -> IR goes through (bad)
3 sheets of paper -> still (bad)
Galaxy Tab2 cardboard -> doesn’t go through (good)

Marlene agreed to prepare 2mm Cardboard for walls.

How do we connect IR sensor and E-Puck: We can remove top part of E-Puck and replace by own board. -> We need own board!

Problem: fast deployment for presentation when using E-Pucks from other groups.
Idea: Remove board, screw perfboard (Lochrasterplatine) on top including our sensors and ATmega328p, prepare 3 such boards, so we can deploy within minutes. 
Problem: 10 volts from e-Puck is way too much -> need to reduce. Resistors not sufficient because of sending, thus using Max232x chip.

Connection including battery connection -> look @ wiki
For now: 3 x e-Puck connection cable

Discussion: Using sound instead of IR: Problem with triangulation.

Solution: e-Puck allows direct connection.


==============

scribe: Ben Wiederhake, 11.05.2016, 12-13 mini-session, while looking up prices on the internet

- No 5V batteries found.  Use 4.5V instead. (Should be fine, ATtiny + IR emitter is low-power.)
- RS232 connector is difficult to find and to implement -- avoid altogether?


==============

scribe: Ben Wiederhake, 11.05.2016, 14-16 session

- koehma said "I2C solves everything", apparently including the power supply
- started the specification file (no further notes here as all things are in the spec file)


==============

scribe: Ben Wiederhake, 13.05.2016, 13-16 session

- ability to drive backwards to pick up the victim!
- Magnets
- Proximity sensors to detect whether the teddy is still there

==============

scribe: Marlene Böhmer (first 5%); Maximilian Schwenger (remaining 95%), 18.05.2016 

Communication diagram:

Components: 
- E-Puck + ATmega328p = TinBot
- Pi + Camera = LPS
- DebugServer
- Teddy + ATtiny2313 = Victim

Communication Channels:
Internal: 
- E-Puck <- ATmega328p via i^2c[WAU]
- Pi <- Camera: via USB
External: 
- Teddy -> TinBot IR[SOS]
- TinBot <-> DebugServer via Bluetooth[TBDP]
- LPS <-> TinBot via Bluetooth[LPP]
- TinBot -> Camera via light[CBP]
- TinBot <-> TinBot[T2T]

CBP  = Colored Blob Protocol
LPP  = Local Positioning Protocol
SOS  = Samsung Off Signal
TBDP = Tin Bot Debugging Protocol
WAU  = Where are you?
T2T  = TinBot to TinBot

E-Puck: Path finding, communication, map assembly, victim; considered the hardest.

Divide in groups of two to specify (i) communication protocols (MS, BW), and (ii) prepare an electronic schematic (MK,MB).

Real Time Constraints: 
- Tin Bot + IR Sensors: Sampling in predefined period of time to actually obtain the samples.
- Tin Bot + Tin Bot: Communication response in predefined period of time otherwise considered dead.

TODO:
Purge inconsistencies in spec.
Add Real Time Consistencies to spec.

MS + BW:
Considering communication protocol:
- Bluetooth uses Master-Slave Protocol.
- Can switch Master and Slave, less than 10 TinBots -> Round robin style master for broadcasting
- http://svn.gna.org/viewcvs/e-puck/trunk/program/bluetooth_mirror/
- found std lib for E-Pucks to use Bluetooth. Found reliable source stating that several Bluetooth connections via SPP are possible at the same time.

MK + MB:
- came up with electronic schematics for the victim, and the extension module for the E-Pucks
- additional components required: IRLZ 44N, 10k Ohm Resistor, additional green LED indicating the victim's status, 6-pin ISP header, and on-off switch for victim (in case the yelling wrecks our nerves). Moreover, for the E-Puck extension board: 6-pin ISP header, per TinBot 2 100 Ohm Resistors, connection cable between E-Puck and extension board (? -> ask Hazem!)

Ask Hazem about additional components for the victim.

Finished up specification document.


==============

scribe: Maximilian Köhl (first 50%), Ben Wiederhake (50%) 20.05.2016 12-16 session

- team meeting
- entry cards do not work
- have a look at parts and components
- discussing problems with the connection of our own extension module (I²C is not accessible from above)
- Ben suggested: when we found the victim, say "thank you mario but your princess is in an other castle"
    + Refused (by Ben)
- buy teddy and magnets
- ask Felix about connectors, send e-mail to Hazem with additional components
- requirement "move out" split into two requirements (more details)
- abstract first then details
- 1. July meeting -> 2 PM 29. June

TODO: be reasonably quick, i.e., try to do measurements "on the fly"
    (nonfunctional nice-to-have)
    e-mail to Hazem with additional components

Begin with the virtual prototype
- agreed to NOT use MATLAB for the first iteration of "designing"
- stages:
    + only signal detection (1 receiver)
    + add "IR environment"
    + add concept of direction(s), detect absolute direction
    + rotate in order to find out angle(s)
    + drive towards the signal
        * invariant: don't turn and drive
    + model walls
        * observation: maze is isomorphic to single wall
=> use only the last $k$ measurements
    - uses only finite memory, is easier to simulate
    - uses only finite memory, is actually realtime
=> need to experiment with the actual physical sensitivity

- use OccupancyGrid (Marlene) to model the environment:
    + http://de.mathworks.com/help/robotics/examples/path-planning-in-environments-of-different-complexity.html
    + http://de.mathworks.com/help/robotics/examples/path-following-for-a-differential-drive-robot.html
    + http://de.mathworks.com/help/robotics/examples/update-an-occupancy-grid-from-range-sensor-data.html


==============

scribe: Ben Wiederhake 23.05.2016 16-18 session

feedback regarding simulation:
- simulation NOT of pathfinding algorithm, but of "path-executing"
- rather not swap simulation software tool, but investigation is a good idea

discussing path-executing virtual-model:
- path-execution is isomorphic to single
- separate into three steps:
	* rotate to goal
	* move to goal
	* say when we're done
- assume arbitrary parameters for now, e.g.: 100 steps per 360°, 100 steps per unit length
	* TODO: measure that!
	* rotational velocity according to web: 0.00628 rad/s -- OF THE WHEELS!
- look at what data we actually send to the stepping motors:
	* seems to be the multiple of the basic stepping speed
- "Webots"nlooks like a nice simulation-y thing.  TODO: check whether we can use any of this.
- do NOT drive backwards just because it's quicker to rotate.

discussing right-hand-rule virtual-model:
- robot passes x and y position directly, for simplicity
- use half-plane for simplicity
	"x >= 5 && y <= 3"
- proximity sensors are located at: -150°, -90°, -45°, \approx -20°, \approx 20°, 45°, 90°, 150°


==============

scribe: Ben Wiederhake 25.05.2016 10-12 session

ITEM GET!  IR Emitter
(TODO: Eintragen.)

(Noch keine Hardware von Conrad gekauft, weil wir zu faul sind, und wir 
genügend Zeug eh schon zu hause haben.)

vrep:
- Simulation ist seltsam: "Dreht sich"
- C code schreiben und von Python aus aufrufen
- Simulation: Motoren einzeln ansteuern != Motoren "simultan" ansteuern = echt
- Tutor: wir sollten es in Simulink einbinden
	* Umgebung sollte in Simulink auftauchen :(

vrep integration:
- [1] C <-> Py <-> vrep <-> log <-> Matlab
	* disadvantage: Matlab doesn't *do* anything
- [2] vrep <-> Matlab <-> C
	* vrep does the simulation of environment and sensors; Matlab/C 
	  responds with the commands/actions
	  
Feedback zur Map:
- Map sind Daten aus der Sensorik
- "Darstellung als Matrix" -> Ungern
- Als Viertelebene -> findet er nicht schlecht
- ein "paar" Simulationen in vrep sind okay; hauptsache, nicht *alle*
- und noch eine Simulation zu "GPS Daten bekommen"

Use cases:
- ask LPS
- RHS (1)
- pathfinding
- IR info proc (2)
- Com T2T (3)
	* map updates
	* map vetos
	* victim info

Wir wollen also:
- Simulation per vrep
- Simulation per Viertelebene
- Simulation zu GPS Daten
- Simulation von "IR-Winkel rausfinden"




