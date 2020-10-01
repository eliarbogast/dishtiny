#!/bin/bash

TARGETS=$(find . -type f ! -path "./third-party/*" ! -path "./.git/*" ! -path "*.tar.gz" ! -path "*.jpg" ! -path "*.png" ! -path "*/assets/*")

for filename in $TARGETS
do
  sed -i 's/\s*$//g' $filename
done
