#!/usr/bin/fish
fish nc.fish &
while true
    python3 sol.py
    set res $status
    if test $res = 1 || true
        break
    end
end
