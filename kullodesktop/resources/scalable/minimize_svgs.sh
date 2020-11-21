#!/bin/bash
set -u
set -e
command -v shellcheck > /dev/null && shellcheck "$0"

function md5() {
    md5sum "$1" | cut -d" " -f 1
}

function filesize() {
    stat --format=%s "$1"
}

for f in ./*.svg; do
    echo "$f"
    bu="$f~"
    cp "$f" "$bu"
    svgo --precision=4 --input="$bu" --output="$f"

    # Restore if optimization did reduze filesize by less than 2 %
    size_f=$(filesize "$f")
    size_bu=$(filesize "$bu")
    if (( size_f > size_bu * 98 / 100 )); then
        cp "$bu" "$f"
    fi
done

