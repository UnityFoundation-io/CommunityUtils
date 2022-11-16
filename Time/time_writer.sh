#!/bin/bash

USERNAME=${1:?USERNAME is not set}
PASSWORD=${2:?PASSWORD is not set}
export TOPIC=${3:?TOPIC is not set}
export DPMGID=${4:?DPMGID is not set}

source common.sh

download-all "${USERNAME}" "${PASSWORD}" "time_writer_security_docs"

./TimeWriter -DCPSConfigFile rtps.ini -DCPSDebugLevel 1
