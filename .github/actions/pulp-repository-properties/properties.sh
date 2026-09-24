#!/usr/bin/env bash
set -euo pipefail

if [[ -z "$MODULE_NAME" || -z "$DISTRIB" || -z "$VERSION" || -z "$STABILITY" ]]; then
  echo "::error::some mandatory inputs are empty, please check the logs."
  exit 1
fi

# parse-distrib emits the el family either generically ("el") or per version
# ("el7"/"el8"/"el9"/"el10" on centreon-collect); normalize it to "el" so the
# family checks below are portable across both conventions.
DEB_PREFIX=""
case "$DISTRIB_FAMILY" in
  el | el[0-9]*)
    DISTRIB_FAMILY="el"
    ;;
  debian) DEB_PREFIX="apt-" ;;
  ubuntu) DEB_PREFIX="ubuntu-" ;;
  *)
    echo "::error::Unsupported distribution family: $DISTRIB_FAMILY"
    exit 1
    ;;
esac

REPOSITORY_PREFIX=""
BASE_PATH_PREFIX=""
STABLE_REPOSITORY_PREFIX=""
REPOSITORY_NAME=""
BASE_PATH=""
SUITE=""
STABLE_BASE_PATH=""
STABLE_SUITE=""
POOL_PATH=""

if [[ "$DISTRIB_FAMILY" == "el" ]]; then
  REPOSITORY_PREFIX="rpm-$VERSION-$DISTRIB-$STABILITY"
  BASE_PATH_PREFIX="rpm/$VERSION/$DISTRIB/$STABILITY"
  STABLE_REPOSITORY_PREFIX="rpm-$VERSION-$DISTRIB-stable"
else
  # one deb repository per stability (base path = repository name), shared by
  # every major version: the version lives in the suite name only
  # (e.g. "trixie-26.09-stable")
  REPOSITORY_NAME="${DEB_PREFIX}${STABILITY}"
  BASE_PATH="$REPOSITORY_NAME"
  SUITE="$DISTRIB-$VERSION-$STABILITY"
  STABLE_BASE_PATH="${DEB_PREFIX}stable"
  STABLE_SUITE="$DISTRIB-$VERSION-stable"
  POOL_PATH="pool/$VERSION/$STABILITY/$MODULE_NAME"
fi

# stable and non-stable repositories share the Domain; the write boundary
# between them is the repository name, enforced server-side by name-scoped
# grants.
DOMAIN="standard"

echo "[DEBUG] - repository_prefix: $REPOSITORY_PREFIX"
echo "[DEBUG] - base_path_prefix: $BASE_PATH_PREFIX"
echo "[DEBUG] - stable_repository_prefix: $STABLE_REPOSITORY_PREFIX"
echo "[DEBUG] - repository_name: $REPOSITORY_NAME"
echo "[DEBUG] - base_path: $BASE_PATH"
echo "[DEBUG] - suite: $SUITE"
echo "[DEBUG] - stable_base_path: $STABLE_BASE_PATH"
echo "[DEBUG] - stable_suite: $STABLE_SUITE"
echo "[DEBUG] - pool_path: $POOL_PATH"
echo "[DEBUG] - domain: $DOMAIN"

{
  echo "repository_prefix=$REPOSITORY_PREFIX"
  echo "base_path_prefix=$BASE_PATH_PREFIX"
  echo "stable_repository_prefix=$STABLE_REPOSITORY_PREFIX"
  echo "repository_name=$REPOSITORY_NAME"
  echo "base_path=$BASE_PATH"
  echo "suite=$SUITE"
  echo "stable_base_path=$STABLE_BASE_PATH"
  echo "stable_suite=$STABLE_SUITE"
  echo "pool_path=$POOL_PATH"
  echo "domain=$DOMAIN"
} >> "$GITHUB_OUTPUT"
