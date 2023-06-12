#!/bin/bash

export DPM_APPLICATION_ID="63"
export DPM_PASSWORD="jV8CKPCqIS7Sv6Ta"
export DPM_NONCE="writer9080"
export HTTP_PORT=9080

export ACCESS_CONTROL_ALLOW_ORIGIN="http://localhost:3000"

#export CREATE_WRITERS=0

./Writer/Writer -DCPSPendingTimeout 5
