#! /usr/bin/env bash 

echo "|Thread<br>Count|Wall Clock<br>Time|User Time|System Time|Speedup|"
echo "|:--:|--:|--:|--:|:--:|"

cmd=./hash-04
baseThreadCount=1

format="%e %U %S"
runTime=( $( /usr/bin/time -f"$format" $cmd $baseThreadCount 2>&1; ) )
baseTime=${runTime[0]}

printf "|%d|%5.2f|%5.2f|%5.2f|1.00|\n" $baseThreadCount ${runTime[@]}

threadCounts=(2 3 4 5 6 7 8 16 24 32 40 48 56 64 72 80)
for numThreads in ${threadCounts[@]} ; do
    runTime=( $( /usr/bin/time -f"$format" $cmd $numThreads 2>&1; ) )
    speedup=$(echo "scale=4; $baseTime / ${runTime[0]}" | bc)
    printf "|%d|%5.2f|%5.2f|%5.2f|%5.2f|\n" $numThreads ${runTime[@]} $speedup
done
