#!/bin/sh

printf "Content-Type: text/html\r\n"
printf "\r\n"

echo "<ul>"
env | sort | while read line; do
    echo "<li>$line</li>"
done
echo "</ul>"
