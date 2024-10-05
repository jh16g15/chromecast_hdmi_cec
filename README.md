# HDMI CEC For Chromecast
A project to play/pause a Chromecast over HDMI-CEC, using an Arduino to hijack the CEC pin (as my TV doesn't support CEC)

## Hardware Notes
3.3V Pro Micro

## Chromecast Notes
1K6 Ohm between DDC and +5V Power (pin 18)

## HDMI Pinout
Will need 27K pullups on CEC line 
CEC line is HDMI pin 13, uses 3.3V signalling, normally-high with 27K pullups on each device
CEC Ground is HDMI pin 17



### MEASUREMENTS

voltage between pin 13 (CEC) and pin 17 (CEC-GND) is 3V when plugged into monitor

## circuit for IR receive
lets use pin 7 for the data
the IR sensor also needs power and ground

## Circuit for CEC transmit/receive
pin 2 for CEC tx/rx, connect to HDMI pin 13
connect GND to HDMI pin 17 to share GND
connect pin 2 to 3V3 via a 27K pullup
may need an isolating diode on this pullup (as if device is unpowered but CEC line is held high by another device, don't want to back-power, so we reverse-bias the diode from CEC->3V3

### Resistors I have on hand
green blue brown    560R
brown black orange  10K
red red red         2K2
10K pot


## CEC Protocol Notes
Physical addressing is worked out over DDC, not CEC
Once a device is connected, it transmits physical and logical addresses to all other devices (over CEC or DDC?)
TV's use a physical address of 0.0.0.0 (16 bits total), as it's acting as the Root Device
physical address is not like MAC address, it marks its location in the "tree of devices"


Start Bit -> Header Block -> Data Block 1 (Opcode) -> Data Block 2 (Operand)

Each block is 1 byte plus 2 flags for EOM (end of message) and ACK (Rcvr pulls low to ACK)
Header info - see CEC Table 5
Opcode info - see CEC Table 7-23
Operand info - see CEC Table 26

### CEC Header
Uses 4-bit Logical Addresses - Src, Dst
See CEC 10.2 for more on logical addresses. 0xf is the broadcast address

We can "ping" a logical address by setting EOM in the header.


### Physical Address Discovery
This occurs over DDC as mentioned above. Whenever a new physical address is discovered on the network, the CEC should allocate a Logical Address and broadcast a Report Physical Address to link the physical and logical addresses

Default physical address on startup appears to be 0xffff for all devices. We may need to I2C to the chromecast to assign it a proper physical address before it will behave.

### Logical Addressing
Defines Device Type and unique ID

0x0 TV
0x1 Recording Device 1
0x2 Recording Device 2
0x3 Tuner 1
0x4 Playback Device 1
0x5 Audio
...
0xC Reserved
0xD Reserved
0xE Free Use
0xF Unregstered (src) or Broadcast (dst)

A device automatically attempts to claim the first ID of it's type, so we can expect the chromecast to attempt to claim 0x4. It claims by sending a header-only polling message to itself. If nobody responds, it keeps that address.

### Protocol Notes
Messages can be broadcast-only, or direct-only (or support both)

We need to send messages from 0x0 (TV) to make sure they are not ignored


### Useful Opcodes

The following list describes each heading within the message tables CEC Table 7 to Table 23.
 Opcode – The name used to identify the message.
 Value – The unique identifier for the message.
 Description – A brief description of the message.
 Parameters – The set of parameters used by the message, refer to CEC Table 26 for individual
descriptions.
 Parameter Description – A brief description of the parameters that the message uses.
 Response – Describes how a device should respond on receipt of the message.
 Directly Addressed – Indicates if the message may be directly addressed.
 Broadcast – Indicates if the message may be broadcast.
 Mandatory – Indicates if it is mandatory for a device to react and respond on receipt of the message.
Note that where a message is indicated as being mandatory for ‘All’ devices, this excludes devices which
act only as a CEC Switch.
Within the table some cells are intentionally left blank; this indicates that there are no associated
requirements for the Opcode described.

0x83    Give Physical Address (Direct only, Requests a "Report Physical Address" message in return. Requires response)
0x84    Report Physical Address (Broadcast only, operands are Physical Address and Device Type. Must respond if TV)
0x41    Play (for playback device, not remote control passthrough. Direct only, operand is "play mode")
0x44    User Control Pressed (direct only, operand is UI command code)
0x45    User Control Released (direct only,)

#### Remote Control Passthrough (CEC 13.13)
This is what we want to use for the Chromecast (which I believe only supports Play/Pause, and maybe Stop?)

2 message types, "User Control Pressed" and "User Control Released"

UI Command Code
0x60    Play    (optional play mode as extra operand)
0x61    Pause
0x64    Stop

##### Play Modes
0x24    Play Forward
0x20    Play Reverse
0x25    Play Still  (Pause?)
...
Various others for different slow/fast speeds

#### Device Types
See Logical IDs and yuse "Device 1" (0x2 is reserved) 