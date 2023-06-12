#!/bin/bash

python3 ./pump.py \
        --url http://34.135.232.161 \
        -o pump1/organization.csv \
        -l pump1/location.csv \
        -s pump1/service.csv
