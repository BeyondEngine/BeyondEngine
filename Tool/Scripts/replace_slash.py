#!/usr/bin/python
# -*-coding:utf-8-*-
#
# replace_slash.py
#   将include中的`\`改为`/`
#
# Usage:
#   python pretty_source.py --dir=./
#

import argparse
import sys
import os
import pdb


def pretty_file(file_name):
    modified = False
    new_text_lines = []
    with open(file_name, 'r') as fp:
        text_lines = fp.readlines()
        for line in text_lines:
            if line.startswith('#include') and line.find('\\') > 0:
                line = line.replace('\\', '/')
                modified = True
            new_text_lines.append(line)

    if modified:
        print 'modified', file_name
        with open(file_name, 'w') as fp:
            fp.writelines(new_text_lines)

    return modified

cpp_ext = ['.h', '.hpp', '.cpp', '.cc', '.cxx']
    
def pretty_dir(root_dir):
    process_num = 0
    for root, sub_folders, files in os.walk(root_dir):
        print root.split('/')[-1]
        for file_name in files:
            file_ext = os.path.splitext(file_name)[1]
            if file_ext in cpp_ext:
                if pretty_file(root + '/' + file_name):
                    # print '\t' + file_name
                    process_num = process_num + 1
    print process_num, 'file processed.'


def main():
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-d", "--dir", help="project directory", default="./")
    args = parser.parse_args()
    pretty_dir(args.dir)


if __name__ == '__main__':
    main()
