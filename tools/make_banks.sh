#!/bin/sh

if [[ $# -eq 0 ]] ; then
    echo 'usage: ./make_banks.sh <org_dir> <wave_dat> <out_dir> [<spu_start_addr>]'
    exit 0
fi

for fn in `ls "$1"`; do
  ./orgconv "$1/$fn" "$2" "$3/${fn%%.*}.sfx" $4
done
