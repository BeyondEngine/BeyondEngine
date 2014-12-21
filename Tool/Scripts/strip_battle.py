#!/usr/bin/python
# -*-coding:utf-8-*-

import json
import argparse


def strip_json(input, output):
    fp = open(input, 'r')
    text_lines = fp.readlines()
    fp.close()
    content = []
    preline = ''
    for line in text_lines:
        if line.startswith('{"scene":'):
            assert preline.startswith('request'), preline
            content.append(line)
        preline = line

    f = open(output, 'w')
    f.writelines(content)
    f.close()


def main():
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-i", "--input", help="input file name", default="battleai_stdout")
    parser.add_argument("-o", "--output", help="output file name", default="battlereplay.json")
    args = parser.parse_args()
    strip_json(args.input, args.output)


if __name__ == '__main__':
    main()
