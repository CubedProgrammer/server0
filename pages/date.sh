#!/usr/bin/bash
echo -e HTTP/1.1 200 OK\\x0d
echo -e connection: close\\x0d
echo -e content-type: text/plain\\x0d
echo -e \\x0d
date
date +\%s
echo $0 $1
