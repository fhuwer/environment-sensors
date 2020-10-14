#!/usr/bin/env python
import argparse


def transform_page(input_filename, output_filename):
    with open(input_filename) as f:
        content = f.readlines()

    c_content = []
    for line in content:
        c_content.append(line.rstrip().replace('"', '\\"'))

    header_def = output_filename.replace(".", "_").upper()
    with open(output_filename, "w") as f:
        f.write(f"#ifndef _{header_def}_\n")
        f.write(f"#define _{header_def}_\n\n")
        f.write('const String root_html = "')
        f.write("\\\n".join(c_content))
        f.write('";\n\n')
        f.write("#endif\n")


if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument("input", help="Input filename")
    argparser.add_argument("output", help="Output filename")
    args = argparser.parse_args()
    transform_page(args.input, args.output)
