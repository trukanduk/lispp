#/usr/bin/env python

from __future__ import print_function
import argparse

def main(args):
    template_text = args.template_file.read();
    module_text = ""
    for line in args.module_file:
        module_text += '    "{}"\n'.format(line.rstrip().replace('"', '\\"'))

    result_text = template_text.format(module_name=args.module_name,
                                       module_text=module_text.strip())
    print(result_text, file=args.output_file)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Tool for generation lispp files")
    parser.add_argument("template_file", type=argparse.FileType('r'))
    parser.add_argument("module_name")
    parser.add_argument("module_file", type=argparse.FileType('r'))
    parser.add_argument("output_file", type=argparse.FileType('w'))

    args = parser.parse_args()
    main(args)
