#!/bin/bash

echo cleaning host $HOSTNAME

rm /dev/shm/buf_response* /dev/shm/file_* /dev/shm/output_file_* /dev/shm/sem.sem_new_msgs  /dev/shm/sem.sem_requests /dev/shm/sem.sem_response* /dev/shm/circular_buffer /dev/shm/index_buf /dev/shm/vader_segment.* /dev/shm/vader_segment.* /dev/shm/caching_info* /dev/shm/sem.sem_write*
rm /dev/shm/sem._mutex* /dev/shm/sem._sem_num_* /dev/shm/_last_* /dev/shm/_first_*
rm -f /dev/shm/*offset*
rm files_location.txt
touch files_location.txt
rm file_*
