#!/vendor/bin/sh

# data format:
# wlan0=a89cedb9c066
WLAN_MAC_DATA_PATH="/data/vendor/mac_addr/wlan_mac.bin"

# data format:
# Intf0MacAddress=00AA00BB00CC
# Intf1MacAddress=00AA00BB00CD
# END
WLAN_MAC_PERSIST_PATH="/mnt/vendor/persist/wlan_mac.bin"

function wait_for_file() {
    file="${1}"
    max_retries=10
    retries=0

    while [ ! -s "${file}" ]; do
        retries=$((retries + 1))

        if [ "${retries}" -eq "${max_retries}" ]; then
            return 1
        fi

        sleep 1
    done

    return 0
}

if ! wait_for_file "${WLAN_MAC_DATA_PATH}"; then
    exit
fi

if [ ! -f "${WLAN_MAC_PERSIST_PATH}" ]; then
    # Read file contents
    raw_mac_data=$(cat "${WLAN_MAC_DATA_PATH}")

    # Strip wlan0= from the string
    raw_mac="${raw_mac_data#*=}"

    # Convert to decimal
    dec_mac=$(printf "%d" "0x$raw_mac")

    # The MAC of the first interface is the decimal mac,
    # converted to uppercase
    first_mac=$(printf "%012X" "$dec_mac")

    # Increment the decimal mac by one
    dec_mac=$(echo "$dec_mac + 1" | bc)

    # The MAC of the first interface is the decimal mac
    # plus one, converted to uppercase
    second_mac=$(printf "%012X" "$dec_mac")

    # Write the MACs
    echo "Intf0MacAddress=${first_mac}" > "${WLAN_MAC_PERSIST_PATH}"
    echo "Intf1MacAddress=${second_mac}" >> "${WLAN_MAC_PERSIST_PATH}"
    echo "END" >> "${WLAN_MAC_PERSIST_PATH}"
fi

insmod /vendor/lib/modules/wlan.ko
