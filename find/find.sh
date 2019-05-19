#!/bin/bash
function Fliter() {
    file $1 | grep -q -w "text"
    if [[ $? -eq 0 ]];then
        echo 0
    else 
        echo 1
    fi
}
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
            check=`Fliter $1/${i}`
            if [[ check -eq 0 ]]; then
                openfile $1/${i}
            fi
        fi
    done
    return
}

if [[ $# -eq 0 ]]; then
    opendir "."
    printf "the string is %s, len = %d\n" ${max_string:=NULL} $max
else
    for i in $@; do
        if [[ -d ${i} ]]; then
            continue
        elif [[ -f ${i} ]]; then
            check=`Fliter ${i}`
            if [[ check -eq 0 ]]; then
                continue
            else
                printf "Error : %s is not a text file\n" ${i}
                exit 1
            fi
        else
            printf "Error : no such file or directory: %s\n" ${i}
            exit 1
        fi
    done
    for i in $@; do
        max=0
        max_string=0
        if [[ -f ${i} ]]; then
            check=`Fliter ${i}`
            if [[ check -eq 0 ]]; then
                openfile ${i}
            fi
        elif [[ -d ${i} ]]; then
            opendir ${i}
        fi
    printf "the string is %s, len = %d\n" ${max_string:=NULL} $max
    done
fi
