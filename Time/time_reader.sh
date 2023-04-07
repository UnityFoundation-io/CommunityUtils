#!/bin/bash
# Copyright 2023 CommunityUtils Authors

APPLICATION_ID=${1:?APPLICATION_ID is not set}
APPLICATION_PASSWORD=${2:?APPLICATION_PASSWORD is not set}
export TOPIC=${3:?TOPIC is not set}

source common.sh

download-all "${APPLICATION_ID}" "${APPLICATION_PASSWORD}" "time_reader_security_docs"

./TimeReader -DCPSConfigFile rtps.ini -DCPSDebugLevel 1
