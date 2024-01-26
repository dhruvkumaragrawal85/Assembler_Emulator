ldc 8      ; load first number into accumulator
stl 100      ; store it in memory location 0
ldc 4      ; load second number into accumulator
stl 101      ; store it in memory location 1
ldc 2      ; load third number into accumulator
stl 102      ; store it in memory location 2

; check if first < second
ldl 101
stl 103
ldl 100

sub        ; subtract second from first
brlz check3 ; if result is negative, go to check3
stl 103      ; otherwise, store second number in memory location 3


check3: 
; check if first < third
ldl 103      ; load first number into accumulator
ldl 102      ; load third number into accumulator
sub        ; subtract third from first
brlz done  ; if result is negative, we're done
stl 103      ; otherwise, store third number in memory location 3

done: 
ldl 103      ;load minimum value into accumulator
HALT      


