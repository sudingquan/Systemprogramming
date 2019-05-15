#!/bin/bash
nowpath=`pwd`
function openfile() {
    local max=0
    for i in `cat $1 | tr -c -s "a-zA-Z" " "`; do
        if [[ ${#i} -gt max ]]; then
            max=${#i}
            local max_string=${i}
        fi
    done
    printf "the string is %s, len = %d\n" ${max_string:=NULL} $max
    return
}
function opendir() {
    for i in `ls -A $1`; do
        if [[ -d ${i} ]]; then
            opendir $1/${i}
        elif [[ -f ${i} ]]; then
            openfile $1/${i}
        fi
    done
    return
}
if [[ -f ${1} ]]; then
    openfile $nowpath/${i}
elif [[ -d ${1} ]]; then
    opendir $nowpath/${i}
fi

cd $nowpath
