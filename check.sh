#!/bin/bash
assert(){
        input="$1"

        echo "assemble statement checkment....."
        echo "input sentence => $input "
        echo " "

        ./9cc "$input" 
}

# assert 'a=1; b = 5 * 6-8; a + b/2;'
# assert 'aak=1;'
# assert 'return 5; return 6;'
# assert 'a = 0;if(a)1+1;1+5;'
# assert 'if(0)1+1; else 3;'
# assert  ' a = 1; b = 1; while(a < 3)  1+ 10;'
assert ' a = 1; for(i = 1; i < 5; i = i + 1 ) a = a + 2; a + 0;  '