# ImageProcessor

## Operative Systems project

This is an University Project based on the concepts that we've seen on class about different
topics of the implementation of an operative system. The topics that we are going to test on
the next project are:

- Memory Management: Here we will be able to manage the memory allocation for our program,
    preventing the allocation of resources that are no needed.
- IPC: Operative systems provide tools for communication through process, so we will need
    to effectively communicate the subprocess that we are going to do
- Threads: Threads are a concept that the Operative Systems provide to "simulate" a parallel
    processing, or execution. Using this we will be able to process a fraction of a given
    image and apply filters to them.
- Interlocking: As we are going to use multiprocessing (threads) we will need to make use
    of tools for interlocking, preventing or making our application to reduce the risk of
    deadlock.

## Objectives

Well, this a simple program proposed as an exercise to _check that you know about operative
systems_, well that is the premise of the university, but I think that there is not a lot of
thinks related to operative system. After all that text the objectives are:

#### Memory Management

Well the most basic need for a computer software is memory management, this is a fundamental
part that will test us about how we manage memory on a right way, and using the tools that the
standard library provide to us.

#### IPC

Inter-Process Communication, this is a tool that we will need to use to allow communication through the root process and the worker childs, this is mostly to "ensure" that the image
don't get corrupted.

#### C

This might be seen as an implicit requirement, but the implication of this is that we know how
to make use of the most low level tools that we, as c developers, have to manage the computer and resolve the problem.

#### Binary Files

One of the off-topic objectives is to manage binary files, the teacher want to us to be able
to know how to read and write binary files, the basic objective here is be able to read BMP
images, process them and write **correctly** preventing artifacts, errors or corruption.


## Personal Objectives

Even if this is a university project I'll try to propose myself certain goals or difficulties
only for fun, and to be a fun way to learn more about C or even the operative system.

The goals are:

- [ ] Parse arguments on the most graceful way.
- [ ] Show an extended help (with context).
- [ ] Give support to read images that are not only BMP.
- [ ] Create a localized version of this executable.
- [ ] Learn how to structure a C project.
- [ ] Try to only use C for everything on the right way.
- [ ] TODO


## Copyright

This code uses source code of a repository for thread pool implementation.
All the rights reserved for [@mbrossard](https://github.com/mbrossard),
and the [Threadpool repository](https://github.com/mbrossard/threadpool)
