#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status

for script in ${KSQL_START_SCRIPTS//:/ }; do
    echo "Running script \"$script\""
    ksql "${KSQL_URL}" --file "scripts/$script"
done
