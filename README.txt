Team Members:
Amy Lin
Emily Perez-Rodriguez

CONTRIBUTIONS:
In Milestone 1, Emily implemented the Message and MessageSerialization functionalities.
Amy implemented the Table and ValueStack functionalities. For the client programs,
Amy implemented get_value, and Emily implemented set_value. We worked together to implement
and debug incr_value. 


Synchronization Report

There were several data structures that needed to be synchronized for this assignment. The m_tables map needed to be synchronized
because multiple client threads may use this collection and concurrent access could lead to inconsistencies. We also needed to synchronize the Table objects themselves
because these objects are accessed and modified concurrently by multiple threads. The modified_tables set needed to be synchronized in order to ensure that multiple threads 
could add and remove Table objects from the set without issue. We used mutexes to ensure only one thread at a time could access critical 
sections of our code, particularly when handling GET and SET commands so that multiple threads couldn't modify or access Table objects at the same time, preventing data corruption and 
undefined behavior.

We are confident that our server is free of race conditions and deadlocks. We made sure use mutexes when handling GET and SET commands in 
order to ensure the data in the table was protected from concurrent access. We also made sure to use trylock instead of lock during transactions to ensure
that if a lock can't be acquired, transactions fail without waiting for another thread to release a lock.