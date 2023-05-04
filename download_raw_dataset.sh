#!/bin/bash

#Usage: download_raw_dataset.sh <URL> <output_file_name>

# Download the raw dataset
wget $1 -O /tmp/tmp$$
echo extracting...
gzip -cd /tmp/tmp$$ > raw/$2.txt

