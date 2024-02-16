#!/bin/bash

host="$1"
shift
ports=("$@")

if [ -z "$host" ] || [ "${#ports[@]}" -eq 0 ]; then
    echo "Usage: $0 <hostname or IP> <someport> <someport> <someport> ..."
    exit 1
fi

for port in "${ports[@]}"; do
	nc -w 1 -z "$host" "$port"
	echo "Knocked port $port"
	sleep .1
done

if [ "${#ports[@]}" -gt 0 ]; then
    exec nc "$host" "${ports[-1]}"
fi
