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

assert 0 0
assert 42 42
assert 21 5+16+5-5
assert 41 " 12 + 31 -5 + 3"
assert 4 ' (3 + 5) / 2'
assert 15 ' 5*(9-6)'
assert 20 '+2 *(+5 * 2)'

echo OK