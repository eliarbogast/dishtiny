#!/bin/bash

################################################################################
echo
echo "running evolvekickoff.sh"
echo "------------------------"
################################################################################

# fail on error
set -e

if (( "$#" < 4 )); then
  echo "USAGE: [container_tag] [repo_sha] [stint] [series...]"
  exit 1
fi

CONTAINER_TAG="${1}"
echo "CONTAINER_TAG ${CONTAINER_TAG}"
shift

REPO_SHA="${1}"
echo "REPO_SHA ${REPO_SHA}"
shift

STINT="${1}"
echo "STINT ${STINT}"
shift

SERIES="${@}"
echo "SERIES ${SERIES}"

# set up and jump into temporary work directory
cd "$(mktemp -d)"

# curl repro_runner.sh script into to a temporary file
REPRO_RUNNER="$(mktemp)"
curl -o "${REPRO_RUNNER}" "https://raw.githubusercontent.com/mmore500/dishtiny/${REPO_SHA}/script/repro_runner.sh"
chmod +x "${REPRO_RUNNER}"

################################################################################
echo
echo "Run Job with repro_runner.sh"
echo "--------------------------------"
################################################################################

"${REPRO_RUNNER}" \
  -p dnh2v -u mmore500 -s dishtiny \
  --repo_sha "${REPO_SHA}" --container_tag "${CONTAINER_TAG}" \
  << REPRO_RUNNER_HEREDOC_EOF

# fail on error
set -e

################################################################################
echo
echo "running evolvekickoff.sh"
echo "------------------------"
################################################################################

echo "CONTAINER_TAG ${CONTAINER_TAG}"
echo "REPO_SHA ${REPO_SHA}"
echo "STINT ${STINT}"
echo "SERIES ${SERIES}"

source ~/.secrets.sh || :

for just_one_series in ${SERIES}; do

  JOB_SCRIPT="\$(mktemp)"

  echo "series \${just_one_series}"
  echo "JOB_SCRIPT \${JOB_SCRIPT}"

  j2 --format=yaml -o "\${JOB_SCRIPT}" "dishtiny/slurm/evolve/evolvejob.slurm.sh.jinja" << J2_HEREDOC_EOF
container_tag: "${CONTAINER_TAG}"
repo_sha: "${REPO_SHA}"
series: "\${just_one_series}"
stint: "${STINT}"
J2_HEREDOC_EOF

  for retry in {1..10}; do
    sshpass -p "\${HOST_PASSWORD}" ssh "\${HOST_USERNAME}@\$(hostname)" sbatch "\${JOB_SCRIPT}" && echo "  job submit success" && break \
      || (echo "retrying job submit (\${retry})" && sleep \$((RANDOM % 10)))
    if ((\${retry}==10)); then echo "job submit fail" && exit 1; fi
  done &

done

wait

################################################################################
echo
echo "Done! (SUCCESS)"
echo "---------------"
################################################################################

REPRO_RUNNER_HEREDOC_EOF
