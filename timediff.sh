#/bin/sh
before=`cat time`
after=`date +%s`
elapsed="$(expr $after - $before)"
remainder="$(expr $elapsed % 3600)"
hours="$(expr $(expr $elapsed - $remainder) / 3600)"
seconds="$(expr $remainder % 60)"
minutes="$(expr $(expr $remainder - $seconds) / 60)"

printf "Finished at %s, elapsed time: %02d:%02d:%02d\n" "`date +\"%H:%M:%S\"`" $hours $minutes $seconds
