#!/bin/bash

function download-all {
    local -r username="$1"
    local -r password="$2"
    local -r directory="$3"

    mkdir -p "${directory}"

    local -r json=$(cat <<EOF
{
  "username": "${username}",
  "password": "${password}"
}
EOF
          )

    curl -c cookies.txt -H'Content-Type: application/json' -d"${json}" ${DPM_URL}/api/login
    curl -b cookies.txt ${DPM_URL}/api/token_info
    curl --silent -b cookies.txt ${DPM_URL}/api/applications/identity_ca.pem > "${directory}/identity_ca.pem"
    curl --silent -b cookies.txt ${DPM_URL}/api/applications/permissions_ca.pem > "${directory}/permissions_ca.pem"
    curl --silent -b cookies.txt ${DPM_URL}/api/applications/governance.xml.p7s > "${directory}/governance.xml.p7s"
    curl --silent -b cookies.txt ${DPM_URL}/api/applications/key_pair?nonce=NONCE > "${directory}/key-pair"
    jq -r '.public' "${directory}/key-pair" > "${directory}/identity.pem"
    jq -r '.private' "${directory}/key-pair" > "${directory}/identity_key.pem"
    curl --silent -b cookies.txt ${DPM_URL}/api/applications/permissions.xml.p7s?nonce=NONCE > "${directory}/permissions.xml.p7s"
}

export DYLD_LIBRARY_PATH="${ACE_ROOT}/lib:${DDS_ROOT}/lib:${XERCESCROOT}/bin:${SSL_ROOT}/bin:${COMMUNITY_SECURITY_PLUGIN_ROOT}:./Java"
