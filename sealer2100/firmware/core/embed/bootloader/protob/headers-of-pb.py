#!/usr/bin/env python3
import sys
import os.path

generating_headers = sys.argv[1:]

name = "messages-all.h"
mode = "r+" if os.path.exists(name) else "w+"
with open(name, mode) as f:
    headers = [h for h in f]
    for header in generating_headers:
        include = f'#include "{header}"\n'
        if include not in headers:
            f.write(include)
