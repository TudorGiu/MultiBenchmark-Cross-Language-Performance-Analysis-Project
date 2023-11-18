import random
import threading
import time

SIZE_OF_ARRAY = 100000
TRIES = 1000

def dynamic_memory_allocation_time():
    sum = 0
    for _ in range(TRIES):
        start_time = time.time_ns()

        array = [0] * SIZE_OF_ARRAY

        end_time = time.time_ns()
        sum += end_time - start_time

    return int(sum / TRIES)

def dynamic_memory_access_time():
    
    sum = 0
    array = [random.randint(0, 9999) for _ in range(SIZE_OF_ARRAY)]

    a = 0

    for _ in range(TRIES):
    
        start_time = time.time_ns()

        for i in range(SIZE_OF_ARRAY):
            a = array[i]

        end_time = time.time_ns()
        sum += end_time - start_time

    return int(sum / TRIES)

def thread_function():
    pass

def thread_creation_time():
    sum = 0
    for _ in range(TRIES):
        start_time = time.time_ns()
        thread = threading.Thread(target=thread_function)
        thread.start()
        end_time = time.time_ns()
        sum += end_time - start_time

    return int(sum / TRIES)

if __name__ == "__main__":
    print("PYTHON MEASUREMENTS")
    print("Dynamic memory allocation: " + str(dynamic_memory_allocation_time()))
    print("Dynamic memory access: " + str(dynamic_memory_access_time()))
    print("Thread creation: " + str(thread_creation_time()))
    print("_______________________________________________________\n")

