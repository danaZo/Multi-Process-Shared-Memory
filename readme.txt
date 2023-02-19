To run the project use make all
this will create executeables with the names : server and client
run the sever and then the client

IMPORTANT:if you stop the server without closing gracefully, the shared memory won't get deallocated.
to gracefully close the server run the client and send the command EXIT

IMPORTANT: SYSV vs POSIX
in this assignment we used the SYSV API for handling shared memory
and not POSIX, this means we used mutexes instead fnctl as well
We got Netzer's approval to do so

BONUS:
BonusServer and BonusClient are implemetations of unix domain sockets

Dana zorohov 207817529
noam vanunu 318995156