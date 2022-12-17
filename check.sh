#!/bin/bash
assert(){
        input="$1"

        echo "assemble statement checkment....."
        echo "input sentence => $input "
        echo " "

        ./9cc "$input" 
}

assert 'a=1; b = 5 * 6-8; a + b/2;'
assert 'aak=1;'
assert 'return 5; return 6;'