#! /vendor/bin/sh

# Copyright (c) 2012-2013, 2016-2020, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of The Linux Foundation nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
target=`getprop ro.board.platform`

function configure_memory_parameters() {
    # Set Memory parameters.
    #
    # Set per_process_reclaim tuning parameters
    # All targets will use vmpressure range 50-70,
    # All targets will use 512 pages swap size.
    #
    # Set Low memory killer minfree parameters
    # 64 bit will use Google default LMK series.
    #
    # Set ALMK parameters (usually above the highest minfree values)
    # vmpressure_file_min threshold is always set slightly higher
    # than LMK minfree's last bin value for all targets. It is calculated as
    # vmpressure_file_min = (last bin - second last bin ) + last bin
    #
    # Set allocstall_threshold to 0 for all targets.
    #

    # Read adj series and set adj threshold for PPR and ALMK.
    # This is required since adj values change from framework to framework.
    adj_series=`cat /sys/module/lowmemorykiller/parameters/adj`
    adj_1="${adj_series#*,}"
    set_almk_ppr_adj="${adj_1%%,*}"

    # PPR and ALMK should not act on HOME adj and below.
    # Normalized ADJ for HOME is 6. Hence multiply by 6
    # ADJ score represented as INT in LMK params, actual score can be in decimal
    # Hence add 6 considering a worst case of 0.9 conversion to INT (0.9*6).
    # For uLMK + Memcg, this will be set as 6 since adj is zero.
    set_almk_ppr_adj=$(((set_almk_ppr_adj * 6) + 6))
    echo $set_almk_ppr_adj > /sys/module/lowmemorykiller/parameters/adj_max_shift

    # Calculate vmpressure_file_min as below & set for 64 bit:
    # vmpressure_file_min = last_lmk_bin + (last_lmk_bin - last_but_one_lmk_bin)
    minfree_series=`cat /sys/module/lowmemorykiller/parameters/minfree`
    minfree_1="${minfree_series#*,}" ; rem_minfree_1="${minfree_1%%,*}"
    minfree_2="${minfree_1#*,}" ; rem_minfree_2="${minfree_2%%,*}"
    minfree_3="${minfree_2#*,}" ; rem_minfree_3="${minfree_3%%,*}"
    minfree_4="${minfree_3#*,}" ; rem_minfree_4="${minfree_4%%,*}"
    minfree_5="${minfree_4#*,}"

    vmpres_file_min=$((minfree_5 + (minfree_5 - rem_minfree_4)))
    echo $vmpres_file_min > /sys/module/lowmemorykiller/parameters/vmpressure_file_min

    echo "15360,19200,23040,26880,34415,43737" > /sys/module/lowmemorykiller/parameters/minfree
    echo 53059 > /sys/module/lowmemorykiller/parameters/vmpressure_file_min

    # Enable adaptive LMK for all targets &
    # use Google default LMK series for all 64-bit targets >=2GB.
    echo 1 > /sys/module/lowmemorykiller/parameters/enable_adaptive_lmk

    # Enable oom_reaper
    if [ -f /sys/module/lowmemorykiller/parameters/oom_reaper ]; then
        echo 1 > /sys/module/lowmemorykiller/parameters/oom_reaper
    fi

    # Set allocstall_threshold to 0 for all targets.
    # Set swappiness to 100 for all targets
    echo 0 > /sys/module/vmpressure/parameters/allocstall_threshold
    echo 100 > /proc/sys/vm/swappiness

    # Disable wsf for all targets beacause we are using efk.
    # wsf Range : 1..1000 So set to bare minimum value 1.
    echo 1 > /proc/sys/vm/watermark_scale_factor

}

case "$target" in
    "msmnile")
	# Core control parameters for gold
	echo 2 > /sys/devices/system/cpu/cpu4/core_ctl/min_cpus
	echo 60 > /sys/devices/system/cpu/cpu4/core_ctl/busy_up_thres
	echo 30 > /sys/devices/system/cpu/cpu4/core_ctl/busy_down_thres
	echo 100 > /sys/devices/system/cpu/cpu4/core_ctl/offline_delay_ms
	echo 3 > /sys/devices/system/cpu/cpu4/core_ctl/task_thres

	# Core control parameters for gold+
	echo 0 > /sys/devices/system/cpu/cpu7/core_ctl/min_cpus
	echo 60 > /sys/devices/system/cpu/cpu7/core_ctl/busy_up_thres
	echo 30 > /sys/devices/system/cpu/cpu7/core_ctl/busy_down_thres
	echo 100 > /sys/devices/system/cpu/cpu7/core_ctl/offline_delay_ms
	echo 1 > /sys/devices/system/cpu/cpu7/core_ctl/task_thres
	# Controls how many more tasks should be eligible to run on gold CPUs
	# w.r.t number of gold CPUs available to trigger assist (max number of
	# tasks eligible to run on previous cluster minus number of CPUs in
	# the previous cluster).
	#
	# Setting to 1 by default which means there should be at least
	# 4 tasks eligible to run on gold cluster (tasks running on gold cores
	# plus misfit tasks on silver cores) to trigger assitance from gold+.
	echo 1 > /sys/devices/system/cpu/cpu7/core_ctl/nr_prev_assist_thresh

	# Disable Core control on silver
	echo 0 > /sys/devices/system/cpu/cpu0/core_ctl/enable

	# Setting b.L scheduler parameters
	echo 95 95 > /proc/sys/kernel/sched_upmigrate
	echo 85 85 > /proc/sys/kernel/sched_downmigrate
	echo 100 > /proc/sys/kernel/sched_group_upmigrate
	echo 15 > /proc/sys/kernel/sched_group_downmigrate
	echo 1 > /proc/sys/kernel/sched_walt_rotate_big_tasks
	echo 400000000 > /proc/sys/kernel/sched_coloc_downmigrate_ns
	echo 30 > /proc/sys/kernel/sched_min_task_util_for_colocation 

	# cpuset parameters
	echo 0-2 > /dev/cpuset/background/cpus
	echo 0-3 > /dev/cpuset/system-background/cpus
	echo 4-7 > /dev/cpuset/foreground/boost/cpus
	echo 0-2,4-7 > /dev/cpuset/foreground/cpus
	echo 0-7 > /dev/cpuset/top-app/cpus
        echo 0-3 > /dev/cpuset/restricted/cpus

	# Turn off scheduler boost at the end
	echo 0 > /proc/sys/kernel/sched_boost

	# configure governor settings for silver cluster
	echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
	echo 0 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/up_rate_limit_us
        echo 0 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/down_rate_limit_us
	echo 1209600 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq
	echo 576000 > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq
	echo 1 > /sys/devices/system/cpu/cpufreq/policy0/schedutil/pl

	# configure governor settings for gold cluster
	echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy4/scaling_governor
	echo 0 > /sys/devices/system/cpu/cpufreq/policy4/schedutil/up_rate_limit_us
        echo 0 > /sys/devices/system/cpu/cpufreq/policy4/schedutil/down_rate_limit_us
	echo 1612800 > /sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq
	echo 1 > /sys/devices/system/cpu/cpufreq/policy4/schedutil/pl

	# configure governor settings for gold+ cluster
	echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy7/scaling_governor
	echo 0 > /sys/devices/system/cpu/cpufreq/policy7/schedutil/up_rate_limit_us
        echo 0 > /sys/devices/system/cpu/cpufreq/policy7/schedutil/down_rate_limit_us
	echo 1612800 > /sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq
	echo 1 > /sys/devices/system/cpu/cpufreq/policy7/schedutil/pl

	# configure input boost settings
	echo "0:1324800" > /sys/module/cpu_boost/parameters/input_boost_freq
	echo 500 > /sys/module/cpu_boost/parameters/input_boost_ms
        echo "0:0 1:0 2:0 3:0 4:2323200 5:0 6:0 7:2323200" > /sys/module/cpu_boost/parameters/powerkey_input_boost_freq
        echo 400 > /sys/module/cpu_boost/parameters/powerkey_input_boost_ms

	# Disable wsf, beacause we are using efk.
	# wsf Range : 1..1000 So set to bare minimum value 1.
        echo 1 > /proc/sys/vm/watermark_scale_factor

        # Enable oom_reaper
	if [ -f /sys/module/lowmemorykiller/parameters/oom_reaper ]; then
		echo 1 > /sys/module/lowmemorykiller/parameters/oom_reaper
	else
		echo 1 > /proc/sys/vm/reap_mem_on_sigkill
	fi

	# Enable bus-dcvs
	for device in /sys/devices/platform/soc
	do
	    for cpubw in $device/*cpu-cpu-llcc-bw/devfreq/*cpu-cpu-llcc-bw
	    do
		echo "bw_hwmon" > $cpubw/governor
		echo "2288 4577 7110 9155 12298 14236 15258" > $cpubw/bw_hwmon/mbps_zones
		echo 4 > $cpubw/bw_hwmon/sample_ms
		echo 50 > $cpubw/bw_hwmon/io_percent
		echo 20 > $cpubw/bw_hwmon/hist_memory
		echo 10 > $cpubw/bw_hwmon/hyst_length
		echo 30 > $cpubw/bw_hwmon/down_thres
		echo 0 > $cpubw/bw_hwmon/guard_band_mbps
		echo 250 > $cpubw/bw_hwmon/up_scale
		echo 1600 > $cpubw/bw_hwmon/idle_mbps
		echo 14236 > $cpubw/max_freq
                echo 40 > $cpubw/polling_interval
	    done

	    for llccbw in $device/*cpu-llcc-ddr-bw/devfreq/*cpu-llcc-ddr-bw
	    do
		echo "bw_hwmon" > $llccbw/governor
		echo "1720 2929 3879 5931 6881 7980" > $llccbw/bw_hwmon/mbps_zones
		echo 4 > $llccbw/bw_hwmon/sample_ms
		echo 80 > $llccbw/bw_hwmon/io_percent
		echo 20 > $llccbw/bw_hwmon/hist_memory
		echo 10 > $llccbw/bw_hwmon/hyst_length
		echo 30 > $llccbw/bw_hwmon/down_thres
		echo 0 > $llccbw/bw_hwmon/guard_band_mbps
		echo 250 > $llccbw/bw_hwmon/up_scale
		echo 1600 > $llccbw/bw_hwmon/idle_mbps
		echo 6881 > $llccbw/max_freq
                echo 40 > $llccbw/polling_interval
	    done

	    for npubw in $device/*npu-npu-ddr-bw/devfreq/*npu-npu-ddr-bw
	    do
		echo 1 > /sys/devices/virtual/npu/msm_npu/pwr
		echo "bw_hwmon" > $npubw/governor
		echo "1720 2929 3879 5931 6881 7980" > $npubw/bw_hwmon/mbps_zones
		echo 4 > $npubw/bw_hwmon/sample_ms
		echo 80 > $npubw/bw_hwmon/io_percent
		echo 20 > $npubw/bw_hwmon/hist_memory
		echo 6  > $npubw/bw_hwmon/hyst_length
		echo 30 > $npubw/bw_hwmon/down_thres
		echo 0 > $npubw/bw_hwmon/guard_band_mbps
		echo 250 > $npubw/bw_hwmon/up_scale
		echo 0 > $npubw/bw_hwmon/idle_mbps
                echo 40 > $npubw/polling_interval
		echo 0 > /sys/devices/virtual/npu/msm_npu/pwr
	    done
	done

    # memlat specific settings are moved to seperate file under
    # device/target specific folder
    setprop vendor.dcvs.prop 1

    configure_memory_parameters
    ;;
esac

setprop vendor.post_boot.parsed 1
