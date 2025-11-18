flamegraph_base=../FlameGraph

echo "**********************"
echo "Begin data collection for "$2" Seconds"
echo "Target process" $1
echo "**********************"

tgt_pid=`ps -e | grep $1 | awk '{print $1}' | sort -n | head -n 1`

sudo rm -f on_perf*.data*
sudo rm -f off_perf*.data*
sudo rm -f all_perf*.data*

sudo perf record -F 9999 --call-graph dwarf -p -g -o on_perf.data0 -p $tgt_pid -- sleep $2 &
sudo perf record -e sched:sched_stat_sleep -e sched:sched_switch -e sched:sched_process_exit -e sched:sched_stat_blocked \
  -e sched:sched_stat_iowait -g -o off_perf.data.raw -p $tgt_pid -- sleep $2
# sudo perf record -e sched:sched_stat_sleep -e sched:sched_switch -e sched:sched_process_exit -e sched:sched_stat_blocked \
#   -e sched:sched_stat_iowait -p $tgt_pid -g -o off_perf.data.raw -- sleep $2  

perf_pid=$!

wait $perf_pid

echo "**********************"
echo "Begin of data analysis"
echo "**********************"

# Off-CPU analysis
echo "---"
echo "Generating Off-CPU flamegraph"
echo "---"

sudo perf inject -f -v -s -i off_perf.data.raw -o off_perf.data0
sudo perf script -f -F comm,pid,tid,cpu,time,period,event,ip,sym,dso -i off_perf.data0 > off_perf.data1
sudo awk ' NF > 4 { exec = $1; period_ms = int($5 / 1000000) } NF > 1 && NF <= 4 && period_ms > 0 { print $2 } NF < 2 && period_ms > 0 { printf "%s\n%d\n\n", exec, period_ms } ' off_perf.data1 > off_perf.data2
sync
sudo cat off_perf.data2 | $flamegraph_base/stackcollapse.pl > off_perf.data3
sudo cat off_perf.data3 | $flamegraph_base/flamegraph.pl --countname=ms --title="Off-CPU Time Flame Graph" --colors=io > offcpu_$1.svg

# On-CPU analysis
echo "---"
echo "Generating On-CPU flamegraph"
echo "---"

sudo perf script -f -i on_perf.data0  > on_perf.data1
sudo cat on_perf.data1 | $flamegraph_base/stackcollapse-perf.pl > on_perf.data2
sudo cat on_perf.data2 | $flamegraph_base/flamegraph.pl --title="On-CPU Time Flame Graph" > oncpu_$1.svg

#On/Off CPU Mixed flame graph
echo "---"
echo "Generating Mixed cpu flamegraph"
echo "---"

sudo cat off_perf.data3 > all_perf.data
sudo cat on_perf.data2 >> all_perf.data
sudo cat all_perf.data | $flamegraph_base/flamegraph.pl --countname=ms --title="Mixed-CPU Time Flame Graph" > allcpu_$1.svg

echo "All done!"
