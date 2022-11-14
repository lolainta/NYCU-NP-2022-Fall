#/usr/bin/fish
function tc
    timeout $argv[1] nc localhost $argv[2] > /dev/null &
#    timeout $argv[1] nc linux2.cs.nctu.edu.tw $argv[2] > /dev/null &
end

for i in (seq $argv[1])
    tc $argv[2] $argv[3]
end
