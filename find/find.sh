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
    ls -A $1 1>/dev/null 2>/dev/null
    if [[ $? -eq  0 ]];then 
        for i in `ls -A $1`; do
            if [[ -d $1/${i} ]]; then
                ls -A $1/${i} 1>/dev/null 2>/dev/null
                if [[ $? -eq 0 ]]; then
                    opendir $1/${i}
                else
                    printf "\033[31m%s : Permission denied !\033[0m\n" $1/${i}
                fi
            elif [[ -f $1/${i} ]]; then
                check=`Fliter $1/${i}`
                if [[ ${check} -eq 0 ]]; then
                    openfile $1/${i}
                fi
            fi
        done
    fi
    return
}

printf "\t\t\t\033[1;5;37;42m\t\t开始查找\t\t\033[0m\n\n\n\n"
if [[ $# -eq 0 ]]; then
    opendir "."
    printf "\033[32mthe string is %s, len = %d\033[0m\n" ${max_string:=NULL} $max
else
    for i in $@; do
        if [[ -d ${i} ]]; then
            continue
        elif [[ -f ${i} ]]; then
            check=`Fliter ${i}`
            if [[ check -eq 0 ]]; then
                continue
            else
                printf "\033[31mError : %s is not a text file\033[0m\n" ${i}
                exit 1
            fi
        else
            printf "\033[31mError : no such file or directory: %s\033[0m\n" ${i}
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
            ls -A ${i} 1>/dev/null 2>/dev/null
            if [[ $? -eq 0 ]]; then
                opendir ${i}
            else
                printf "\033[31m%s : Permission denied !\033[0m\n" ${i}
            fi
        fi
    printf "\033[32mthe string is %s, len = %d\033[0m\n" ${max_string:=NULL} $max
    done
fi
