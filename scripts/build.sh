#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "$CURRENT_DIR")
DEPENDS_DIR="$PROJECT_DIR/config";

cd "$PROJECT_DIR";
git submodule init;
git submodule update;

build_extfunc_depends()
{
    local params=${@//--force-build/}

    echo ===============
    echo $CFG_TARGET_PLATFORM
    echo ===============

	"$DEPENDS_DIR/scripts/build-apr.sh" $params
	"$DEPENDS_DIR/scripts/build-aprutil.sh" $params

	"$DEPENDS_DIR/scripts/build-minixml.sh" $params

	"$DEPENDS_DIR/scripts/build-openssl.sh" $params;
	"$DEPENDS_DIR/scripts/build-curl.sh" $params

	"$DEPENDS_DIR/scripts/build-alioss.sh" $params;
}

export CFG_PROJECT_NAME="Elastos.SDK.CloudStorage";
export CFG_PROJECT_DIR="$PROJECT_DIR";
export CFG_CMAKELIST_DIR="$PROJECT_DIR/sdk";
source "$DEPENDS_DIR/scripts/build.sh" $@ --force-build;

