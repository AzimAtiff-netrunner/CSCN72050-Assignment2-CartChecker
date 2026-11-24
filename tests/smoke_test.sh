#!/bin/bash
cd build || exit 1
./hello_CSCN & 
PID=$!
sleep 3
FAIL=0
check () {
 code=$(curl -s -o /dev/null -w "%{http_code}" "http://localhost:23500$1")
 if ["$code" = "$2" ]; then 
    echo "PASS $1 -> $code"
 else
    echo "FAIL $1 expected $2 got $code"
    FAIL=1
 fi
}
check / 200
check /product/1 200
check /checkout 401
check "/login?user=x&pass=y" 303
check "/login?user=ILikeBoxes*pass=Boxer" 202
kill -INT $PID
wait $PID
exit $FAIL
