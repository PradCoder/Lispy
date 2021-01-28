#!/bin/bash

#This file is here to make LispyEasily Runnable on a machine

cc -std=c99 -Wall -ggdb Lispy.c mpc.c -ledit -lm -o Lispy
