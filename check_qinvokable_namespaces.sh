#!/bin/bash

ERROR_LINES=$(git grep Q_INVOKABLE | grep "*" | grep -v "KulloDesktop::")
ERROR_COUNT=$(echo "$ERROR_LINES" | wc -w | tr -d ' ')

if [[ "$ERROR_COUNT" != "0" ]]; then
    >&2 echo "Errors found in lines:"
    >&2 echo "$ERROR_LINES"
    exit 1
else
    exit 0
fi
