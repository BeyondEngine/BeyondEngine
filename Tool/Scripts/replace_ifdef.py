#!/usr/bin/python
# -*-coding:utf-8-*-
#
# replace_ifdef.py
#   Convert #ifdef guard to #pragma once，reduce compiling file I/O operation.
#   See http://stackoverflow.com/questions/1143936/pragma-once-vs-include-guards
#

import argparse
import sys
import os

cpp_header_ext = ['.h', '.hpp']


# 去掉空行
def trim_empty_line(text_lines):
    index = 0
    while index < len(text_lines):
        line = text_lines[index]
        if len(line.strip()) == 0:
            index += 1
        else:
            text_lines = text_lines[index:]
            break

    if len(text_lines) > 0:
        index = len(text_lines) - 1
        while index > 0:
            line = text_lines[index]
            if len(line.strip()) == 0:
                index -= 1
            else:
                text_lines = text_lines[:index+1]
                break

    return text_lines


# On most compilers, #pragma once will speed up compilation (of one cpp)
# because the compiler need not reopen the file containing this instruction    
def pretty_file(file_name):
    modified = False
    new_text_lines = []
    fp = open(file_name, 'r')
    text_lines = fp.readlines()
    fp.close()
    if len(text_lines) < 3:
        return False

    text_lines = trim_empty_line(text_lines)
    first_line = text_lines[0]
    second_line = text_lines[1]
    if first_line.startswith("#ifndef"):
        guard = first_line[8:].strip()
        if second_line.startswith("#define"):
            if guard == second_line[8:].strip():
                modified = True
                text_lines = text_lines[2:]

    if modified:
        index = len(text_lines) - 1
        while index > 0:
            line = text_lines[index]
            if line.startswith("#endif"):
                text_lines = text_lines[:index]
                break
        # write to file
        text_lines = trim_empty_line(text_lines)
        lines = ['#pragma once\n', '\n']
        lines.extend(text_lines)
        f = open(file_name, 'w')
        f.writelines(lines)
        f.close()

    return modified


def pretty_dir(root_dir):
    process_num = 0
    for root, sub_folders, files in os.walk(root_dir):
        print root.split('/')[-1]
        for file_name in files:
            file_ext = os.path.splitext(file_name)[1]
            if file_ext in cpp_header_ext:
                if pretty_file(root + '/' + file_name):
                    print '\t' + file_name
                    process_num = process_num + 1
                    break
    print process_num, 'file processed.'


def main():
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-d", "--dir", help="project directory", default="./")
    args = parser.parse_args()
    pretty_dir(args.dir)


if __name__ == '__main__':
    main()
