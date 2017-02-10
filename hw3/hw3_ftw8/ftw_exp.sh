#!/bin/bash
echo 'original version run 5000 times:'
time for i in {1..5000}; do ./ftw8_ori/ftw8 ~; done>&1 | grep ^real | sed -e s/.*m// | awk '{sum += $1} END {print sum/NR}'

echo 'chdir version run 5000 times:'
time for i in {1..5000}; do ./ftw8/ftw8 ~; done>&1 | grep ^real | sed -e s/.*m// | awk '{sum += $1} END {print sum/NR}'
