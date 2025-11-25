# System Programming Lab 11 Multiprocessing

## Description
The purpose of this program is generate a series of mandel images, but introduce forking and child processes to speed up creation of these images. To edit the amount of process, when running the program, use "-p" and indicate the number of processes to be used after "-p" in decimal format.

I implemented this via a while loop dependent on the amount of mandel images. Inside the while loop, there is a count for active processes (starting at 0) which increments when reaching the parent PID portion of the fork and an mandel amount variable which also increments in the same area (when it reaches the parent). When this count reaches a maximum value (the value set at the terminal), it will decrement and the program will continue until all processes end and the amount of mandel images is reached. 

From my results, I found that it took quite a while to generate all 50 mandel images using a single process. The processing time decreased exponentially as I added more and more process, eventually reaching a plateau at 10 processes. This can be seen in the following graph:


https://github.com/MSOE-CPE2600/multiprocessing-turney-UltimateTM/blob/labWeek11dev/Mandel_Graph.png


Implementing multithreading into this project was achieved via using a function pointer, an argument struct, mallocs, and pthread function utilization. First, I outline and defined the pointer function which the pthread_create() function must utilize in the compute_image() function. I utilized a struct for easier variable pointer allocation, as seen in the computer_image_thread() function. After pointing the variables to their correct spots, I kept the same syntax for computer_image creation. For implementation of the actual computer_image() function, the syntax stayed nearly the same unless the number of threads exceeded at least more than one. When more than one thread was requested by the user, the program would malloc and create two arrays, one with the pthreads and the other with the argument structs. It would then go through a for loop dependent on the amount of threads requested and run pthread_create() and then pthread_join(), waiting for the threads to all finish. It would then finish by freeing the memory allocated. The results for this method can be seen below:

https://github.com/MSOE-CPE2600/multiprocessing-turney-UltimateTM/blob/lab12dev/Mandel_Graph_2.png

# What technique seemed to impat runtime more?
Based on the results above, both seemed to have very similar impacts on runtime. I can safely conclude that both are equal. This is most likely because we hit the maximum capability of the CPU on this laptop by using either method, and the CPU is limiting the performance gains.

# Was there a sweet spot?
There was a sweet spot at around 10 threads. This was similar to the multiprocessor sweet spot, where around 10 processes was the sweet spot as well. 

