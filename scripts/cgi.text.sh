#!/bin/sh

printf "Content-Type: text/plain\r\n"
printf "\r\n"

env | sort
