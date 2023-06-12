#!/bin/bash

python3 ./pump.py \
        --url http://localhost:9080 \
        -o pump3/organization.csv \
        -l pump3/location.csv \
        -s pump3/service.csv
