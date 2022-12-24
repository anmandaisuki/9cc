#!/bin/bash
assert(){
        expected="$1"
        input="$2"

        ./9cc "$input" > tmp.s
        cc -o tmp tmp.s
        ./tmp
        actual="$?"

        if [ "$actual" = "$expected" ]; then
            echo "$input => $actual"
        else
            echo "$input => $actual something wrong"
            exit 1 
        fi
}

# SUCCESS
# assert 0 0
# assert 42 42
# assert 21 5+16+5-5
# assert 41 " 12 + 31 -5 + 3"
# assert 4 ' (3 + 5) / 2'
# assert 15 ' 5*(9-6)'
# assert 20 '+2 *(+5 * 2)'
# assert 0 '2 == 0'
# assert 1 '3 < 5'
# assert 12 'a=1; b = 5 * 6-8; a + b/2;'
# assert 5 'return 5;'
# assert 5 'return 5; return 6;'
# assert 2 'a=1; if(a)1+1;'
# assert 1 'a = 1;{a =1; b=1;}a;'
# assert 4 '{2 + 2;}'
# assert 9 ' a = 1; for(i = 1; i < 5; i = i + 1 ) a = a + 2; a + 0;  '    // GREAT
# assert 2 'a=1; if(a){b = 2; } b ;'
# assert 4 'a = 1;if(a == 4){ 1 + 1; a = 4;}1+3;'
# assert 9 ' a = 1; for(i = 1; i < 5; i = i + 1){a = a + 1; a = a + 1;} a;'
assert 12 'a = 30; for(i = 0; i < 10; i = i + 1){ a = a * 2; a = a * 0 ; } a = a + 12;'

# FAILED

echo OK