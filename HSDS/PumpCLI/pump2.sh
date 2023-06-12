#!/bin/bash

python3 ./pump.py \
        --url http://localhost:8080 \
        -o pump2/organization.csv \
        -l pump2/location.csv \
        -s pump2/service.csv
