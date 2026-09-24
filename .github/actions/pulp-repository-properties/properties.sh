#!/usr/bin/env bash
set -euo pipefail

if [[ -z "$MODULE_NAME" || -z "$DISTRIB" || -z "$STABILITY" ]]; then
  echo "::error::some mandatory inputs are empty, please check the logs."
  exit 1
fi

# repository_name selects the standard (open-source), business (paid) or plugins
# repository family. plugins repositories are not versioned (packages carry their
# own date-based versioning), so the version input is only required for the
# other families.
REPOSITORY_TYPE="${REPOSITORY_TYPE:-standard}"
case "$REPOSITORY_TYPE" in
  standard) REPO_BASE="standard" ;;
  business) REPO_BASE="business" ;;
  plugins) REPO_BASE="plugins" ;;
  *)
    echo "::error::Unsupported repository_name: $REPOSITORY_TYPE"
    exit 1
    ;;
esac

if [[ "$REPO_BASE" != "plugins" && -z "$VERSION" ]]; then
  echo "::error::version input is mandatory for the $REPOSITORY_TYPE repository family."
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

# uniform stability segments across every edition (plugins included):
# unstable, testing-release, testing-hotfix, stable
TESTING_SEGMENT="testing"
TESTING_POOL_SEGMENT="testing"
if [[ "$RELEASE_TYPE" == "release" || "$RELEASE_TYPE" == "hotfix" ]]; then
  TESTING_SEGMENT="testing-$RELEASE_TYPE"
  TESTING_POOL_SEGMENT="testing/$RELEASE_TYPE"
fi

STABILITY_SEGMENT="$STABILITY"
POOL_SEGMENT="$STABILITY"
if [[ "$STABILITY" == "testing" ]]; then
  STABILITY_SEGMENT="$TESTING_SEGMENT"
  POOL_SEGMENT="$TESTING_POOL_SEGMENT"
fi

REPOSITORY_PREFIX=""
BASE_PATH_PREFIX=""
TESTING_REPOSITORY_PREFIX=""
TESTING_BASE_PATH_PREFIX=""
STABLE_REPOSITORY_PREFIX=""
STABLE_BASE_PATH_PREFIX=""
REPOSITORY_NAME=""
BASE_PATH=""
SUITE=""
TESTING_REPOSITORY_NAME=""
TESTING_BASE_PATH=""
TESTING_SUITE=""
STABLE_SUITE=""
POOL_PATH=""
TESTING_POOL_PATH=""
STABLE_POOL_PATH=""

if [[ "$REPO_BASE" == "plugins" ]]; then
  # plugins repositories are not versioned: rpm paths carry no version segment
  # and deb suites are the plain distribution codename.
  if [[ "$DISTRIB_FAMILY" == "el" ]]; then
    REPOSITORY_PREFIX="rpm-$DISTRIB-$STABILITY_SEGMENT"
    BASE_PATH_PREFIX="rpm/$DISTRIB/$STABILITY_SEGMENT"
    TESTING_REPOSITORY_PREFIX="rpm-$DISTRIB-$TESTING_SEGMENT"
    TESTING_BASE_PATH_PREFIX="rpm/$DISTRIB/$TESTING_SEGMENT"
    STABLE_REPOSITORY_PREFIX="rpm-$DISTRIB-stable"
    STABLE_BASE_PATH_PREFIX="rpm/$DISTRIB/stable"
  else
    # one deb repository per stability (base path = repository name), suites
    # carry the plain codename only
    REPOSITORY_NAME="${DEB_PREFIX}${STABILITY_SEGMENT}"
    BASE_PATH="$REPOSITORY_NAME"
    SUITE="$DISTRIB"
    TESTING_REPOSITORY_NAME="${DEB_PREFIX}${TESTING_SEGMENT}"
    TESTING_BASE_PATH="$TESTING_REPOSITORY_NAME"
    TESTING_SUITE="$DISTRIB"
    STABLE_REPOSITORY_NAME="${DEB_PREFIX}stable"
    STABLE_BASE_PATH="$STABLE_REPOSITORY_NAME"
    STABLE_SUITE="$DISTRIB"
    POOL_PATH="pool/$POOL_SEGMENT/$MODULE_NAME"
    TESTING_POOL_PATH="pool/$TESTING_POOL_SEGMENT/$MODULE_NAME"
    STABLE_POOL_PATH="pool/stable/$MODULE_NAME"
  fi
else
  if [[ "$DISTRIB_FAMILY" == "el" ]]; then
    REPOSITORY_PREFIX="rpm-$VERSION-$DISTRIB-$STABILITY_SEGMENT"
    BASE_PATH_PREFIX="rpm/$VERSION/$DISTRIB/$STABILITY_SEGMENT"
    TESTING_REPOSITORY_PREFIX="rpm-$VERSION-$DISTRIB-$TESTING_SEGMENT"
    TESTING_BASE_PATH_PREFIX="rpm/$VERSION/$DISTRIB/$TESTING_SEGMENT"
    STABLE_REPOSITORY_PREFIX="rpm-$VERSION-$DISTRIB-stable"
    STABLE_BASE_PATH_PREFIX="rpm/$VERSION/$DISTRIB/stable"
  else
    # one deb repository per stability (base path = repository name), shared by
    # every major version: the version lives in the suite name only
    # (e.g. "trixie-26.09-stable")
    REPOSITORY_NAME="${DEB_PREFIX}${STABILITY_SEGMENT}"
    BASE_PATH="$REPOSITORY_NAME"
    SUITE="$DISTRIB-$VERSION-$STABILITY_SEGMENT"
    TESTING_REPOSITORY_NAME="${DEB_PREFIX}${TESTING_SEGMENT}"
    TESTING_BASE_PATH="$TESTING_REPOSITORY_NAME"
    TESTING_SUITE="$DISTRIB-$VERSION-$TESTING_SEGMENT"
    STABLE_REPOSITORY_NAME="${DEB_PREFIX}stable"
    STABLE_BASE_PATH="$STABLE_REPOSITORY_NAME"
    STABLE_SUITE="$DISTRIB-$VERSION-stable"
    POOL_PATH="pool/$VERSION/$POOL_SEGMENT/$MODULE_NAME"
    TESTING_POOL_PATH="pool/$VERSION/$TESTING_POOL_SEGMENT/$MODULE_NAME"
    STABLE_POOL_PATH="pool/$VERSION/stable/$MODULE_NAME"
  fi
fi

# unless a dedicated stable repository was selected above, stable shares the
# delivery repository (rpm resolves stable through the *_PREFIX variables)
STABLE_REPOSITORY_NAME="${STABLE_REPOSITORY_NAME:-$REPOSITORY_NAME}"
STABLE_BASE_PATH="${STABLE_BASE_PATH:-$BASE_PATH}"

# one Pulp Domain per edition; stable and non-stable repositories share it.
# The stable/non-stable write boundary is the repository name, enforced
# server-side by name-scoped grants. stable_domain is kept as a distinct output
# for the scripts that address the stable tier, even though it now always
# equals domain.
DOMAIN="$REPO_BASE"
STABLE_DOMAIN="$DOMAIN"

echo "[DEBUG] - repository_type: $REPOSITORY_TYPE"
echo "[DEBUG] - repository_prefix: $REPOSITORY_PREFIX"
echo "[DEBUG] - base_path_prefix: $BASE_PATH_PREFIX"
echo "[DEBUG] - testing_repository_prefix: $TESTING_REPOSITORY_PREFIX"
echo "[DEBUG] - testing_base_path_prefix: $TESTING_BASE_PATH_PREFIX"
echo "[DEBUG] - stable_repository_prefix: $STABLE_REPOSITORY_PREFIX"
echo "[DEBUG] - stable_base_path_prefix: $STABLE_BASE_PATH_PREFIX"
echo "[DEBUG] - repository_name: $REPOSITORY_NAME"
echo "[DEBUG] - base_path: $BASE_PATH"
echo "[DEBUG] - suite: $SUITE"
echo "[DEBUG] - testing_repository_name: $TESTING_REPOSITORY_NAME"
echo "[DEBUG] - testing_base_path: $TESTING_BASE_PATH"
echo "[DEBUG] - testing_suite: $TESTING_SUITE"
echo "[DEBUG] - stable_suite: $STABLE_SUITE"
echo "[DEBUG] - pool_path: $POOL_PATH"
echo "[DEBUG] - testing_pool_path: $TESTING_POOL_PATH"
echo "[DEBUG] - stable_pool_path: $STABLE_POOL_PATH"
echo "[DEBUG] - domain: $DOMAIN"
echo "[DEBUG] - stable_domain: $STABLE_DOMAIN"

{
  echo "repository_prefix=$REPOSITORY_PREFIX"
  echo "base_path_prefix=$BASE_PATH_PREFIX"
  echo "testing_repository_prefix=$TESTING_REPOSITORY_PREFIX"
  echo "testing_base_path_prefix=$TESTING_BASE_PATH_PREFIX"
  echo "stable_repository_prefix=$STABLE_REPOSITORY_PREFIX"
  echo "stable_base_path_prefix=$STABLE_BASE_PATH_PREFIX"
  echo "repository_name=$REPOSITORY_NAME"
  echo "base_path=$BASE_PATH"
  echo "suite=$SUITE"
  echo "testing_repository_name=$TESTING_REPOSITORY_NAME"
  echo "testing_base_path=$TESTING_BASE_PATH"
  echo "testing_suite=$TESTING_SUITE"
  echo "stable_suite=$STABLE_SUITE"
  echo "stable_repository_name=$STABLE_REPOSITORY_NAME"
  echo "stable_base_path=$STABLE_BASE_PATH"
  echo "pool_path=$POOL_PATH"
  echo "testing_pool_path=$TESTING_POOL_PATH"
  echo "stable_pool_path=$STABLE_POOL_PATH"
  echo "domain=$DOMAIN"
  echo "stable_domain=$STABLE_DOMAIN"
} >> "$GITHUB_OUTPUT"
