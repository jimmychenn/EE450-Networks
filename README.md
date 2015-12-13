# EE450-Networks

The two programming assignments in this repo were exercises in demonstrating some networking knowledge.
The programming objectives are described in the pdf assignments in the folders.

The first programming asignment was written in C while the other was written in Python. Both employ UDP socket connections.

Since prog1 interacts with a server set up by the instructor, it won't work but the code will give insight to how the objective was achieved. To run the test client and server use make, then first run the server program and then run the client program.

Prog3 interacts in a similar way. The intention is for the two programs to be run on different machines on the same LAN network to measure server traffic. As a result, the target IP addresses are meant to hard coded in both scripts. To run the experiment, first run "python server.py" on the intended server machine. Then run "python client.py" on the intended client machine.
