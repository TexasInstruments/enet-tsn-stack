#!/bin/bash

PWD=$(cd $(dirname "$0") && pwd)

main()
{
    src_dir=$PWD/..
    dst_dir=$1

    if [[ -z $dst_dir ]]; then
        echo "Please specify the destination folder for copying code to"
        echo "Usage: $0 dst_dir"
        exit 1
    fi

    if [ ! -d $dst_dir ]; then
        echo "Creating destination folder to copy code to"
        mkdir -p $dst_dir
    else
        echo "Deleting the destination folder ..."
        rm -rf $dst_dir/*
    fi

    remove_files=(
        "cb_tmevent.c"
        "cb_ipcsock"
        "cb_xtimer"
        "cb_crc"
        "cb_ipcshmem.c"
        "cb_xtimer"
        "cb_thread.c"
        "cb_ethernet.c"
        "cb_inet"
        "uniconf.c"
        "lmdb_ucbind.c"
        "gptp2d.c"
    )

    pushd $src_dir

    for f in $(find . -type f  -name "*.[ch]" -o -name "*.xml"); do
        if [[ ($f == *"unittest"*) || ($f == *"posix"*) ]]; then
            continue
        fi
        
        file_name=$(basename "$f")
        filename="${file_name%.*}"

        if [[ (" ${remove_files[*]} " =~ " ${filename} ") || (" ${remove_files[*]} " =~ " ${file_name} ") ]]; then
            echo "Skipping file $f"
            continue;
        fi
        
        mkdir -p "$(dirname ${dst_dir}/${f})"
        cat $f > $dst_dir/$f
    done

    cp -f .gitignore $dst_dir/
    cp -f README.md $dst_dir/
    cp -f LICENSE.md $dst_dir/
    cp -f CONTRIBUTING.md $dst_dir/
    cp -rf docs $dst_dir/
    cp -rf docs_src $dst_dir/
    popd

};main $@

