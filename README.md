# barelog

**barelog** is a set of C99 modules that can be used to do some logging on many-core systems. The primary targets of barelog are the embedded heterogeneous many-core platforms (such as the [Parallella platform](https://parallella.org)) or any
core that is too small to run any Linux based OS, thus forbidding the use of traditional tools. 

The main use-case would be the logging of some calculus-specific cores that don't
have any kernel but **can still access a shared memory space** to interact with a 
more "traditional" host (that is to say another CPU able to run a Linux kernel).

Please note that due to it's current limitations, barelog is not meant to be used for serious, efficient logging/tracing. For a more sophisticated tool that provides very efficient tracing, please see also [barectf](https://github.com/efficios/barectf).

Note : in the following document, the terms "host" and "target" refer respectively
to a system running a Linux kernel and able to initialize the shared memory and to
the specific core that doesn't run any kernel.

**Key features**:

  * Entirely configurable: you have full control over the functions used by the
    modules to interact with the shared memory as well as the total amount of
    memory used by barelog (inside each core as well as in the shared section).
  * Easy to use: a simple call of the **barelog_log()** function (after proper
    initialization of the modules) allows you to log events without any further
    complications.
  * Provides several "functioning modes": you can enable/disable some parts of 
    the code to suit your needs. For example, to gain some performance, you might 
    want to disable the "DEBUG_MODE" that only offers some internal debugging 
    functions. 
  * Flush events whenever you want: a round-buffer allows you to store the events
    in the local memory of the logged core as long as you want before actually
    flushing them into the shared memory. You have full control over which stored
    event to actually put into the shared space.
  * Format the events data as you want: since the logging module use a modified
    version of "snprintf" you can store any type of data (represented as a string)
    in a event.
 
**Current limitations**:

  * Pretty heavy impact on the performances: since the logging module use a
    modified version of "snprintf", it's quite demanding in terms of clock cycles
    to produce an event.
  * The size of the actual event's data is statically fixed: that means that if 
    the events data are not full, there will be waste of both local memory 
    (of the logged core) and shared memory.
  * The data of an event is represented by a string: which means that you can't
    directly access to all the data logged into that event since they are wrapped
    in a string.
 
## Using

### Compiling the modules

  1. You first need to edit the **common/include/config.h** file to ensure that
     barelog is configured to suit your needs. Note that you can directly include
     a custom configuration header by placing it inside the **platforms**
     directory and then including it. 

  2. Once it's done, you may want need to edit the Makefile to properly set the
     compiler used to compile the target module code. You can also set the
     TARGET_CC flag during the 'make'.
 
  3. Then simply compile the modules using the provided Makefile. You can specify
     whether or not to use a cross-compiling toolchain by setting the
     CROSS_COMPILE flag. You can also decide if you rather want the resulting
     libraries to be static (.a) or shared (.so) by setting the HLIBTYPE and/or
     TLIBTYPE flags (where 'H' stands for Host and 'T' for Target). The default
     behavior is to produce static libraries.

```sh
    make
```
  Or

```sh
    make HLIBTYPE=so TLIBTYP=a
```

If everything went well, two libraries should have been produced in the **libs**
folder :

  * **libbarelog_host**: targets the host program.
  * **libbarelog_logger**: targets the target program.

### Instrumenting and compiling your code

#### Instrumenting your code

Once you have compiled the modules, you just need to instrument your code to get started ! 

To do that, you have to follow those steps :

  1. Initialize the host: you will have to create the **barelog_platform_t** along
     with some memory management functions and to register them to the logger on
     the host by calling the **barelog_host_init()** function. This will allocate
     all the needed chunks of shared memory according to the "config" file and 
     initialize the all host module.
     
  2. Initialize the target: this basically involve the same steps as above but
     with everything specific to the target.
     
  3. Instrument the target code: by using a combination of the **barelog_log()**
     **barelog_flush()** and **barelog_clean()**, you should be able to produce and manage the events inside the logged core.
    
  4. Retrieve the events on the host: the host API offers some functions to
     extract and display the logged events (please see the given example).
     
  5. Finalize the logger: once you're done logging around, use the
     **barelog_finalize() function to ensure every resource is correctly
     deallocated.
     
Please refer to the documentation and/or the given example for more informations.

**WARNING** : if you use barelog, some part of the shared memory (beginning at the
given platform's mem_space) will be used by it. To avoid every hazardous behavior,
consider using the **BARELOG_SHARED_MEM_MAX** macro (which give the size (in 
bytes) of the memory taken by barelog) when allocating new chunks of memory for
your personal needs.

#### Compiling your code

Now that we have everything ready, we just need to compile our programs (one
running on the host and the other on the target).

First of all, make sure that the previously generated barelog's libraries can be
found by the compiler/linker. Assuming that your using gcc, you just need to
specify the -L option :

```sh
    gcc -L path/to/libraries/
 ```

You will then have to build the host program using the **libbarelog_host** library
and the target program with the **libbarelog_logger** library :

```sh
    gcc -L path/to/libraries/ target_main.c -lbarelog_logger
    gcc -L path/to/libraries/ host_main.c -lbarelog_host
```

Of course, this need to be adapted in case you need to use another compiler.

### Create your own configuration file

To create you own configuration file, you can simply follow the "template" given 
by **common/include/config.h**. You don't have to fulfill every fields since there
already is some default values (please refer to config.h).

Once it's done, you just have to put it in the **platforms** directory, thus
guaranteeing that you could later include it in the config header.
     
### Warnings

  * The core numbering on the target must begin at 0.
  * The barelog_device_mem_manager module should be placed in the local memory
    of each logged core.
  * The "SAFE" mode, providing shared memory synchronization mechanism is still
    to be tested, thus implying that no guarantee can be provided.

## Configuring new behaviors/functionalities

You might want to add some functionalities that need some data stored into the
shared memory space of barelog. Since this space is strictly ordered, you will
have to follow those steps to ensure the good global behavior of the modules :

  1. Define the size taken by those data inside the **barelog_internal.h** file:
     you can use the following naming convention : 'BARELOG_FUNCNAME_MEM_SIZE'
     
  2. Edit the BARELOG_SHARED_MEM_DATA_OFFSET macro to take the new data in account
     while computing the offsets of each barelog's data inside the shared memory.

  3. Reserve a new mem_space for your data by adding '1' to the
     BARELOG_HOST_NB_MEM_SPACE macro.
     
  4. Define the new index inside the host's mem_space table of the new data:
     you can use the following naming convention : 'BARELOG_FUNCNAME_I'. Please
     be careful with the index since some may already have been taken and the 
     BARELOG_NB_CORES first refer to the actual events reserved memory spaces. You
     can follow what has been done with BARELOG_DEBUG_MODE and BARELOG_SAFE_MODE
     to get the global picture of how to do it.

  4. Modify the behavior of the "host_mem_manager_init()" and
     "host_mem_manager_finalize()" functions to respectively init and finalize the
     newly reserved mem_space.
