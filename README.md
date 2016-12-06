# Project 3
Simulating a Link State Routing Protocol

## Installation
1. `git clone`
2. `make`

## Usage
`./manager <NETWORKCONFIGFILE>`

After running the manager with a given config file, the program generates logs with names of component.out where component is manager or routerID. 

The manager reads the config file and creates child processes for each router in the simulation. The child processes receives their router ID then make a connection with the manager over TCP. This is their control mechanism for the simulation. They open a UDP port to simulate their connection to the network. The manager gives the whole UDP port mapping for each router to every other router. However, the routers are only able to communicate with those they're connectect to in accordance with the config file.

After the router processes exchange connectivity information with their neighbors, they establish a routing table. When this is complete, the manager begins sending the packet transmission instructions from the config file to the origin routers and the simulated routers fire off their packets.

When all packets have been transmitted, the program exits. All activity is logged.

## Notes
- Timestamps use UTC and have a resolution down to microseconds

## Contributing
N/A (Schoolwork)

## Background
Assigned for CS457 - Networks

## Credits
kbmulligan

## License
MIT License
