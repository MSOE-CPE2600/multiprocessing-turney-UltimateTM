# System Programming Lab 11 Multiprocessing

## Description
The purpose of this program is generate a series of mandel images, but introduce forking and child processes to speed up creation of these images. To edit the amount of process, when running the program, use "-p" and indicate the number of processes to be used after "-p" in decimal format.

I implemented this using the for loop dependent on the amount of mandel images. Inside the for loop, there is a count for active processes (starting at 0) which increments when reaching the parent PID portion of the fork. When this count reaches a maximum value (the value set at the terminal), it will decrement and the program will continue until all processes end. 

From my results, I found that it took quite a while to generate all 50 mandel images using a single process. The processing time decreased exponentially as I added more and more process, eventually reaching a plateau at 10 processes. This can be seen in the following graph:


https://github.com/MSOE-CPE2600/multiprocessing-turney-UltimateTM/blob/labWeek11dev/Mandel_Graph.png
