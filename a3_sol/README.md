# Scheduling

## Current

In this assignment I had to make some design decisions in regards to the process sceduling. The way I chose which process to go next was really quite simple. It will alway choose process in the highest priority queue first, normal highest priority queue second, and lowest priority queue second. I know what your thinking. This will always lead to starvation of the process in the lowest and normal priority queues if there is a process in the highest priority queue. That's why I implemented conditions for moving between the queues themselves. If I process has run twice it would move down in priority and if a process has run more than 7 times it would move up in priority. I know this method isn't perfect, but it makes it so eventually all processes will be in the lowest priority and slowly make their way back up in priority.

## In in the future

A way I thought to make this better is give a percentage value to the queues for be choosen for each position to be checked. Something like:
    - highest priority: 60%
    - normal priority:  30%
    - lowest priority:  20%

That way we would see even less starvation of process with the lowest priority while still being fair to the processes in the highest priority.

## Other design decisions

Arround the init process:
    - init process cannot send, recieve, reply
    - init porcess can't call P() or V()
