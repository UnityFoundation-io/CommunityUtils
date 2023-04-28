#!/bin/bash
# Copyright 2023 CommunityUtils Authors

APPLICATION_ID=${1:?APPLICATION_ID is not set}
APPLICATION_PASSWORD=${2:?APPLICATION_PASSWORD is not set}
export TOPIC=${3:?TOPIC is not set}

source common.sh

download-all "${APPLICATION_ID}" "${APPLICATION_PASSWORD}" "time_reader_security_docs"

${JAVA_HOME}/bin/java -ea -cp Java/classes:${COMMUNITY_UTILS_ROOT}/CommunitySecurityPlugin/CommunitySecurityPlugin.jar:${DDS_ROOT}/lib/i2jrt.jar:${DDS_ROOT}/lib/i2jrt_corba.jar:${DDS_ROOT}/lib/OpenDDS_DCPS.jar TimeDemo.TimeReader -DCPSConfigFile rtps.ini -DCPSDebugLevel 1
