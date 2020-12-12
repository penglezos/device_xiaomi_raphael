#!/vendor/bin/sh
usb_config=`getprop vendor.usb.mimode`

if [ "$(getprop persist.vendor.usb.config)" != "$usb_config" -a \
        "$(getprop ro.boot.factorybuild)" != "1" ];then
     setprop persist.vendor.usb.config "$usb_config"
fi
