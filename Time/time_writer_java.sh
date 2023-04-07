#!/bin/bash
# Copyright 2023 CommunityUtils Authors

USERNAME=${1:?USERNAME is not set}
PASSWORD=${2:?PASSWORD is not set}
export TOPIC=${3:?TOPIC is not set}
export DPMGID=${4:?DPMGID is not set}

source common.sh

download-all "${USERNAME}" "${PASSWORD}" "time_writer_security_docs"

${JAVA_HOME}/bin/java -ea -cp Java/classes:${COMMUNITY_SECURITY_PLUGIN_ROOT}/CommunitySecurityPlugin.jar:${DDS_ROOT}/lib/i2jrt.jar:${DDS_ROOT}/lib/i2jrt_corba.jar:${DDS_ROOT}/lib/OpenDDS_DCPS.jar TimeDemo.TimeWriter -DCPSConfigFile rtps.ini -DCPSDebugLevel 1
