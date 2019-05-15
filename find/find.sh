#!/bin/bash
function openfile() {
    for i in `cat $1 | tr -c -s "a-zA-Z" " "`; do
        if [[ ${#i} -gt max ]]; then
            max=${#i}
            max_string=${i}
        fi
    done
    printf "openfile %s\n" $1
    return
}
function opendir() {
    printf "opendir %s\n" $1
    for i in `ls -A $1`; do
        if [[ -d $1/${i} ]]; then
            opendir $1/${i}
        elif [[ -f $1/${i} ]]; then
            openfile $1/${i}
        fi
    done
    return
}

if [[ $# -eq 0 ]]; then
    opendir "."
    printf "the string is %s, len = %d\n" ${max_string:=NULL} $max
else
    for i in $@; do
        max=0
        max_string=0
        if [[ -f ${i} ]];then
            openfile ${i}
        elif [[ -d ${i} ]]; then
            opendir ${i}
        fi
    printf "the string is %s, len = %d\n" ${max_string:=NULL} $max
    done
fi
