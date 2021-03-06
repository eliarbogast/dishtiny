#!/bin/bash

set -e

usage="$(basename "$0"): run reproducible, self-documenting computations

./$(basename "$0")
  to enter an interactive shell session, then <ctrl-d> to complete

cat script.sh | ./$(basename "$0")
  to run a bash script as a session, automatically completes after script executes

for each session:

  1. a session ID (\"REPRO_ID\") is generated

  2. a work directory is created within the current working directory

  3. a folder called output is created within the work directory

  4. a shallow, recursive clone of the project's GitHub repository is performed in the work directory
    * the repository's sha is recorded for reproducibility

  5. the project's Docker container is fetched
    * Docker performs caching to save bandwidth for repeated accesses
    * the container's sha is recorded for reproducibility

  6. a shell session is launched on the Docker container using Singularity

  7. during the shell session, all commands are recorded

  8. shell session completes
    * if any files from the session should be saved, be sure to put them in the output folder during the shell session
    * bonus points if you use the REPRO_ID environment variable in output filenames so that the files can be traced back to this session

  9. the following artifacts are added to the OSF project's storage
    * \"a=repro+year={REPRO_YEAR}/month={REPRO_MONTH}/day={REPRO_DAY}/hour={REPRO_HOUR}/id={REPRO_ID}/a=runner+repro={REPRO_ID}+ext=.sh\"
      * this script
    * \"a=repro+year={REPRO_YEAR}/month={REPRO_MONTH}/day={REPRO_DAY}/hour={REPRO_HOUR}/id={REPRO_ID}/a=stdin+repro={REPRO_ID}+ext=.txt\"
      * commands recorded during the shell session
    * \"a=repro+year={REPRO_YEAR}/month={REPRO_MONTH}/day={REPRO_DAY}/hour={REPRO_HOUR}/id={REPRO_ID}/a=output+repro={REPRO_ID}+ext=.tar.gz\"
      * tar.gz archive of the output folder
    * \"a=repro+year={REPRO_YEAR}/month={REPRO_MONTH}/day={REPRO_DAY}/hour={REPRO_HOUR}/id={REPRO_ID}/a=manifest+repro={REPRO_ID}+ext=.tar.gz\"
      * hierarchical listing of output folder contents
    * \"a=repro+year={REPRO_YEAR}/month={REPRO_MONTH}/day={REPRO_DAY}/hour={REPRO_HOUR}/id={REPRO_ID}/a=rerun+repro={REPRO_ID}+ext=.sh\"
      * script to rerun computations from the current session
      * using sha's to ensure the same exact same source and container
    * \"a=repro+year={REPRO_YEAR}/month={REPRO_MONTH}/day={REPRO_DAY}/hour={REPRO_HOUR}/id={REPRO_ID}/a=log+repro={REPRO_ID}+ext=.txt\"
      * output from the current session

  10. if the shell session exited an error code, a pushover notification is dispatched
    * contains \$SLURM_JOB_NAME
    * contains hyperlink to the job log file on OSF

required environment variables:
  OSF_USERNAME
  OSF_PASSWORD
  PUSHOVER_USER_TOKEN
  PUSHOVER_APP_TOKEN

required arguments:
  -p / --project  osf project ID
  -s / --slug  slug used to identify project on dockerhub and github
  -u / --username  username used for dockerhub and github

optional arguments:
  -b / --branch  source branch to check out (default: master)
  -c / --container_tag tag of docker container to run in, can include container sha (default: latest)
  -h / --help  show this message and exit
  -r / --repo_sha repo sha to check out (default: none)

exported environment variables:
  REPRO_ID a randomly generated 16-character string identifying this session
  REPRO_YEAR
  REPRO_MONTH
  REPRO_DAY
  REPRO_HOUR

"

################################################################################
echo
echo "Setup Temporary Files"
echo "--------------------------------------"
################################################################################

stdin=$(mktemp)
log=$(mktemp)
context=$(mktemp)
rerun=$(mktemp)
output=$(mktemp)
manifest=$(mktemp)
runner=$(readlink -f $0) # get path to this script

################################################################################
echo
echo "Redirect stdout and stderr to Log File"
echo "--------------------------------------"
################################################################################
exec > >(tee "${log}") 2>&1

################################################################################
echo
echo "running repro_runner.sh"
echo "-----------------------"
################################################################################

echo "${0} ${@}"

################################################################################
echo
echo "Parse Arguments"
echo "--------------------------------------"
################################################################################

# adapted from https://stackoverflow.com/a/33826763
while [[ "$#" -gt 0 ]]; do
  case $1 in
    -b|--branch) arg_branch="$2"; shift ;; # source branch to check out
    -c|--container_tag) container_tag="$2"; shift ;; # container to run in
    -h|--help) echo "$usage"; exit;; # print help message
    -p|--project) arg_project="$2"; shift ;; # osf project id
    -r|--repo_sha) repo_sha="$2"; shift ;; # source sha to check out
    -s|--slug) arg_slug="$2"; shift ;; # github/dockerhub project slug
    -u|--username) arg_username="$2"; shift ;; # github/dockerhub username
    *) echo "Unknown argument passed: $1"; exit 1 ;;
  esac
  shift
done

# setup default arguments
test ${arg_branch} || arg_branch=master
test ${container_tag} || container_tag=latest

echo "arg_branch ${arg_branch}"
echo "container_tag ${container_tag}"
echo "arg_project ${arg_project}"
echo "repo_sha ${repo_sha}"
echo "arg_slug ${arg_slug}"
echo "arg_username ${arg_username}"

################################################################################
echo
echo "Assert Prequisites"
echo "--------------------------------------"
################################################################################

command -v module >/dev/null && module load git/2.27.0 && module list || :
git --version
[[ -f ~/pyenv/bin/activate ]] && source ~/pyenv/bin/activate
[[ -f ~/.secrets.sh ]] && source ~/.secrets.sh

command -v osf >/dev/null || ( echo "no osf client" && exit 1 )
test ${OSF_USERNAME} || ( echo "no OSF_USERNAME env var" && exit 1 )
test ${OSF_PASSWORD} || ( echo "no OSF_PASSWORD env var" && exit 1 )
test ${PUSHOVER_USER_TOKEN} || ( echo "no PUSHOVER_USER_TOKEN env var" && exit 1 )
test ${PUSHOVER_APP_TOKEN} || ( echo "no PUSHOVER_APP_TOKEN env var" && exit 1 )
test ${arg_project} || ( echo "no --project arg" && exit 1 )
test ${arg_slug} || ( echo "no --slug arg" && exit 1 )
test ${arg_username} || ( echo "no --username arg" && exit 1 )


################################################################################
echo
echo "Generate REPRO_ID"
echo "--------------------------------------"
################################################################################

export REPRO_ID="$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 16 | head -n 1)"
export REPRO_YEAR="$(date +'%Y')"
export REPRO_MONTH="$(date +'%m')"
export REPRO_DAY="$(date +'%d')"
export REPRO_HOUR="$(date +'%H')"

echo "REPRO_YEAR ${REPRO_YEAR}"
echo "REPRO_MONTH ${REPRO_MONTH}"
echo "REPRO_DAY ${REPRO_DAY}"
echo "REPRO_HOUR ${REPRO_HOUR}"
echo "REPRO_ID ${REPRO_ID}"

REPRO_PATH="a=repro+year=${REPRO_YEAR}/month=${REPRO_MONTH}/day=${REPRO_DAY}/hour=${REPRO_HOUR}/id=${REPRO_ID}"
echo "REPRO_PATH ${REPRO_PATH}"

################################################################################
echo
echo "Log Job Info"
echo "--------------------------------------"
################################################################################
echo "REPRO_ID ${REPRO_ID}"
echo "date $(date)"
echo "hostname $(hostname)"
echo "pwd $(pwd)"
command -v qstat >/dev/null && test ${SLURM_JOB_ID} && bash -c "qstat -f \"${SLURM_JOB_ID}\""

echo "stdin ${stdin}"
echo "log ${log}"
echo "context ${context}"
echo "rerun ${rerun}"
echo "output ${output}"
echo "manifest ${manifest}"
echo "runner ${runner}"

################################################################################
echo
echo "Setup Exit and Error Traps"
echo "--------------------------------------"
################################################################################

function on_exit {

  echo
  echo "Run Exit Trap"
  echo "-------------"

  echo "log job duration..."
  echo "  date $(date)"
  echo "  SECONDS ${SECONDS}"

  # echo "uploading stdin"
  # for retry in {1..3}; do
  #   osf -p "${arg_project}" upload \
  #     "${stdin}" \
  #     "${REPRO_PATH}/a=stdin+repro=${REPRO_ID}+ext=.txt" \
  #   && echo "  stdin upload success" \
  #   && break \
  #     || (echo "retrying stdin upload (${retry})" && sleep $((RANDOM % 10)))
  #   if ((${retry}==3)); then echo "upload stdin fail"; fi
  # done &

  echo "making rerun script"
  INPUT="$(cat $stdin)"
  echo "#!/bin/bash" > "${rerun}"
  echo "rm -rf ${REPRO_ID} && mkdir ${REPRO_ID} && cd ${REPRO_ID}" \
    >> "${rerun}"
  echo "mkdir ${arg_slug}" >> "${rerun}"
  echo "git -C ${arg_slug} init" >> "${rerun}"
  echo "git -C ${arg_slug} remote add origin https://github.com/${arg_username}/${arg_slug}.git" >> "${rerun}"
  echo "git -C ${arg_slug} fetch --depth 1 origin ${repo_sha}" >> "${rerun}"
  echo "git -C ${arg_slug} checkout FETCH_HEAD" >> "${rerun}"
  echo "git -C ${arg_slug} submodule update --init --recursive --depth 1" >> "${rerun}"
  echo "singularity shell docker://${arg_username}/${arg_slug}@${container_tag#*@} \
<< 'END_OF_HEREDOC'
${INPUT}
END_OF_HEREDOC" >> "${rerun}"
  chmod +x "${rerun}"

  echo "uploading rerun"
  for retry in {1..3}; do
    osf -p "${arg_project}" upload \
      "${rerun}" \
      "${REPRO_PATH}/a=rerun+repro=${REPRO_ID}+ext=.sh" \
    && echo "  rerun upload success" \
    && break \
      || (echo "retrying rerun upload (${retry})" && sleep $((RANDOM % 10)))
    if ((${retry}==3)); then echo "upload rerun fail"; fi
  done &

  echo "uploading log"
  for retry in {1..3}; do
    osf -p "${arg_project}" upload \
      "${log}" \
      "${REPRO_PATH}/a=log+repro=${REPRO_ID}+ext=.txt" \
    && echo "  log upload success" \
    && break \
      || (echo "retrying log upload (${retry})" && sleep $((RANDOM % 10)))
    if ((${retry}==3)); then echo "upload log fail"; fi
  done &

  # if user has created an output directory, upload it
  if [ -d "${WORK_DIRECTORY}/output" ]; then

    echo "uploading output manifest"
    ls "${WORK_DIRECTORY}/output" > "${manifest}"
    for retry in {1..3}; do
      osf -p "${arg_project}" upload \
        "${manifest}" \
        "${REPRO_PATH}/a=manifest+repro=${REPRO_ID}+ext=.txt" \
      && echo "  manifest upload success" \
      && break \
        || (echo "retrying manifest upload (${retry})" && sleep $((RANDOM % 10)))
      if ((${retry}==3)); then echo "upload manifest fail"; fi
    done &

    echo "uploading output"
    tar -czf "${output}" "${WORK_DIRECTORY}/output"
    for retry in {1..10}; do
      osf -p "${arg_project}" upload \
        "${output}" \
        "${REPRO_PATH}/a=output+repro=${REPRO_ID}+ext=.tar.gz" \
      && echo "  output upload success" \
      && break \
        || (echo "retrying output upload (${retry})" && sleep $((RANDOM % 10)))
      if ((${retry}==10)); then echo "upload output fail"; fi
    done

    raw_output_url=$(osf -p "${arg_project}" geturl \
      "${REPRO_PATH}/a=output+repro=${REPRO_ID}+ext=.tar.gz" \
    )
    output_url=$(curl -Ls -o /dev/null -w %{url_effective} $raw_output_url)
    echo "output uploaded to ${output_url}"
    echo "  download a copy: curl -L ${output_url}download --output ${REPRO_ID}.tar.gz"

  fi

  wait

  echo "Exit Trap Complete"

}

function on_error() {

  echo
  echo "Run Error Trap (FAIL)"
  echo "---------------------"

  echo "   EXIT STATUS ${1}"
  echo "   LINE NO ${2}"

  echo "---------------------"
  echo

  echo "squeue -u $(whoami) | wc -l" && squeue -u "$(whoami)" | wc -l
  echo "squeue -u $(whoami)" && squeue -u "$(whoami)"

  echo "hostname" && hostname
  echo "lscpu" && lscpu
  echo "date" && date

  echo "pwd" && pwd
  echo "ls" && ls
  echo "du -h .  --max-depth 1" && du -h . --max-depth 1
  echo "tree --du -h out*" && tree --du -h out* || :

  # memory info
  echo "cat /proc/meminfo" && cat /proc/meminfo
  echo "free -g" && free -g
  echo "vmstat" && vmstat

  # disk info
  echo "lsblk" && lsblk
  # echo "ioping -c /tmp" && ioping -c /tmp
  # echo "ioping -c ." && ioping -c .
  # echo "ioping -c ~" && ioping -c ~

  # network info
  echo "curl -I https://google.com" && curl -I https://google.com
  echo "ipcs" && ipcs
  echo "cat /proc/net/dev" && cat /proc/net/dev
  time curl -o /dev/null http://speedtest-blr1.digitalocean.com/10mb.test

  if (( ${1} == 123123 )); then
    command -v scontrol \
      && scontrol requeue "${SLURM_JOB_ID}" \
      && echo "error code 123123, job resubmit success, restart count ${SLURM_RESTART_COUNT}" \
      || echo "error code 123123, job resubmit failure, restart count ${SLURM_RESTART_COUNT}"
  fi

  # upload log files
  on_exit

  echo "Sending Pushover Notification"
  bash <(curl https://raw.githubusercontent.com/mmore500/pushover.sh/master/pushover.sh) \
   -T "$PUSHOVER_APP_TOKEN" -U "$PUSHOVER_USER_TOKEN" \
   -u $(osf -p "${arg_project}" geturl \
      "${REPRO_PATH}/a=log+repro=${REPRO_ID}+ext=.txt" \
    ) \
    "${SLURM_JOB_ID} ${SLURM_JOB_NAME} error code ${1}, restart count ${SLURM_RESTART_COUNT}"

  # reset exit trap
  trap '' EXIT

  echo "Error Trap Complete"

  exit ${1}

}

trap 'on_error $? $LINENO' ERR
trap on_exit EXIT

################################################################################
echo
echo "Setup Work Directory"
echo "--------------------------------------"
################################################################################
rm -rf "${REPRO_ID}" && mkdir "${REPRO_ID}" && cd "${REPRO_ID}"
export WORK_DIRECTORY="$(pwd)"
echo "WORK_DIRECTORY ${WORK_DIRECTORY}"

################################################################################
echo
echo "Get Assets"
echo "--------------------------------------"
################################################################################

# setup output folder
# no, user has to make output folder if they want it
# mkdir -p output

echo "repo_sha before asset get ${repo_sha}"

if [ -n "${repo_sha}" ]; then
  echo "setting up pinned project source at revision ${repo_sha}..."
  for retry in {1..20}; do
    rm -rf "${arg_slug}" \
    && mkdir "${arg_slug}" \
    && git -C "${arg_slug}" init \
    && git -C "${arg_slug}" remote add origin "https://github.com/${arg_username}/${arg_slug}.git" \
    && git -C "${arg_slug}" fetch --quiet --depth 1 origin "${repo_sha}" \
    && git -C "${arg_slug}" checkout FETCH_HEAD \
    && git -C "${arg_slug}" submodule update --quiet --init --recursive --depth 1 \
    && echo "  source setup success" \
    && break \
    || (echo "source setup failure (${retry})" && sleep $((RANDOM % 10)))

    if ((${retry}==20)); then echo "source setup fail" && exit 123123; fi
  done
else
  echo "setting up latest project source..."
  for retry in {1..20}; do
    rm -rf "${arg_slug}"
    git clone "https://github.com/${arg_username}/${arg_slug}.git" \
      --quiet --depth 1 --recursive --shallow-submodules \
      --branch "${arg_branch}" \
    && echo "  source clone success" \
    && break \
    || (echo "source clone failure (${retry})" && sleep $((RANDOM % 10)))

    if ((${retry}==20)); then echo "source clone fail" && exit 123123; fi
  done
  repo_sha=$(git -C "${arg_slug}" rev-parse HEAD)
fi

echo "repo_sha after asset get ${repo_sha}"

# get sha of latest docker container
if [[ "${container_tag}" != *"@sha256:"* ]]; then
  container_tag="${container_tag}@sha256:$(\
    singularity exec "docker://${arg_username}/${arg_slug}:${container_tag}" \
      bash -c 'echo ${SINGULARITY_NAME}' \
  )"
fi
echo "container_tag with sha256 ${container_tag}"

################################################################################
echo
echo "Do Work"
echo "--------------------------------------"
################################################################################

# Timeout with 5 minutes (300 seconds) left in job.
# If not a slurm job, set duration to 0 to disable the associated timeout.
TIMEOUT_SECONDS=$( \
  test ${SLURM_JOB_ID} \
  && squeue -j "${SLURM_JOB_ID}" -O "TimeLeft" \
    | tail -n 1 \
    | tr ":" "\n" \
    | tac \
    | tr "\n" " " \
    |  awk '{ print $1 + $2*60 + $3*3600 + $4*86400 }' \
    | awk '{print $1-300}' \
  || echo 0 \
)
echo "TIMEOUT_SECONDS ${TIMEOUT_SECONDS}"


# pipe input into the container, record a copy to $STDIN
# Docker references with both a tag and digest are currently not supported in
# singularity so we have to strip everything before @sha256:digest
cat "-" | timeout "${TIMEOUT_SECONDS}s" tee "${stdin}" | \
  singularity shell \
    --env "SECONDS=${SECONDS}" \
    -B "${HOME}:/home/user" \
    --dns 8.8.8.8,8.8.4.4 \
    "docker://${arg_username}/${arg_slug}@${container_tag#*@}"
