source "${BOSS_StarterKit}/setup/FunctionsPrint.sh"


# * ============================ * #
# * --== General parameters ==-- * #
# * ============================ * #
currentPath="$(pwd)"
extLibs="/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/ExternalLib"
sourceDir="/afs/ihep.ac.cn/bes3/offline/Boss"
targetDir="${BOSS_StarterKit}/ThirdParty/headers"

function AbortScript()
{
  local message="${1:-}"
  [[ "${message}" != "" ]] && PrintError "${message}"
  exit 1
}
function CheckDir()
{
  local dirPath="${1}"
  [[ ! -d "${dirPath}" ]] && AbortScript "Target directory \"${dirPath}\" does not exist"
}
function AttemptCd()
{
  { cd ${1}; } &> /dev/null || AbortScript "Directory \"${1}\" does not exist"
}

mkdir -p "${targetDir}"
CheckDir "${extLibs}"
CheckDir "${targetDir}"


# * ============== * #
# * --== BOSS ==-- * #
# * ============== * #
function FetchBOSS()
{
  local currentPath="$(pwd)"
  # * Determine version location
  mkdir -p "${targetDir}/versionsBOSS"
  AttemptCd "${targetDir}/versionsBOSS"
  rm -rf *
  for v in $(ls ${sourceDir}); do
    echo > "${v}"
  done
  local defaultVersion="7.0.4"
  read -e -p "Which version of BOSS do you want to load? " -i $defaultVersion versionBOSS
  AttemptCd "${targetDir}"
  rm -rf versionsBOSS
  # * Copy headers
  AttemptCd "${sourceDir}/${versionBOSS}/InstallArea/include"
  rm -rf "${targetDir}/BOSS"
  mkdir -p "${targetDir}/BOSS"
  PrintBold "Copying headers and source files from BOSS version ${versionBOSS}..."
  for package in $(ls); do
    cd $package
    for header in $(find -L -type f -regextype posix-extended -regex "^.*/[A-Za-z0-9]+\.(h|hh|icc)"); do
      mkdir -p "${targetDir}/BOSS/$(dirname ${header})"
      cp "${header}" "${targetDir}/BOSS/${header}"
    done
    cd - > /dev/null
  done
  # * Fetch icc files from source code
  AttemptCd "${sourceDir}/${versionBOSS}"
  for package in $(find -L -type d -regextype posix-extended -regex "^.*/[^/]+-[0-9][0-9]-[0-9][0-9]-[0-9][0-9]$"); do
    # Remove subdirs such as "Event"
    local packageDir="$(dirname ${package/$(dirname $(dirname "${package}"))\/})"
    for file in $(find $package -type f -regextype posix-extended -regex "^.*/src/.*\.(C|cc|cpp|cxx|icc)$"); do
      local targetFile="${targetDir}/BOSS/$packageDir/${file/${package}\/src\/}"
      mkdir -p "$(dirname "${targetFile}")"
      cp "${file}" "${targetFile}"
    done
  done
  # * Remove Gaudi dll's
  AttemptCd "${targetDir}/BOSS"
  rm -rf $(find -type f -regextype posix-extended -iregex ".*/(components|dll)/[^/]*(load|dll|entries)[^/]*$")
  cd "${currentPath}"
}
FetchBOSS

# * =============== * #
# * --== CLHEP ==-- * #
# * =============== * #
function FetchCLHEP()
{
  local currentPath="$(pwd)"
  # * Choose version
  mkdir -p "${targetDir}/versionsCLHEP"
  cd "${targetDir}/versionsCLHEP"
  rm -rf *
  local defaultVersion="2.0.4.5"
  local versionsCLHEP=(
    "clhep/1.9.4.7/x86_64-slc6-gcc46-opt"
    "clhep/2.0.4.5/x86_64-slc6-gcc46-opt"
    "CLHEP/2.4.0.2/x86_64-slc6-gcc494-opt"
  )
  for v in ${versionsCLHEP[@]}; do
    echo > "$(echo "${v}" | cut -d / -f 2)"
  done
  read -e -p "Which version of CLHEP do you want to load? " -i $defaultVersion versionCLHEP
  cd "${targetDir}"
  rm -rf versionsCLHEP
  # * Determine version location
  local versionPath=""
  for v in ${versionsCLHEP[@]}; do
    if [[ "$(echo "${v}" | cut -d / -f 2)" == "${versionCLHEP}" ]]; then
      versionPath="${v}"
      break
    fi
  done
  [[ "${versionPath}" == "" ]] && AbortScript "Version ${versionCLHEP} does not exist!"
  # * Copy headers
  mkdir -p "${targetDir}/CLHEP/"
  cd "${targetDir}/CLHEP/"
  rm -rf CLHEP
  PrintBold "Copying \"${versionPath}\"..."
  cp -Rf "${extLibs}/external/${versionPath}/include/CLHEP" .
  cd "${currentPath}"
}
FetchCLHEP


# * =============== * #
# * --== Gaudi ==-- * #
# * =============== * #
function FetchGaudi()
{
  local currentPath="$(pwd)"
  # * Choose version
  cd "${extLibs}/gaudi"
  local versionsGaudi=$(find -maxdepth 4 -type d -iwholename "*/InstallArea/x86_64-slc6-gcc46-opt/include")
  mkdir -p "${targetDir}/versionsGaudi"
  cd "${targetDir}/versionsGaudi"
  rm -rf *
  local defaultVersion="GAUDI_v23r9"
  for v in ${versionsGaudi[@]}; do
    echo > "$(echo "${v}" | cut -d / -f 2)"
  done
  read -e -p "Which version of Gaudi do you want to load? " -i $defaultVersion versionGaudi
  cd "${targetDir}"
  rm -rf versionsGaudi
  # * Determine version location
  local versionPath=""
  for v in ${versionsGaudi[@]}; do
    if [[ "$(echo "${v}" | cut -d / -f 2)" == "${versionGaudi}" ]]; then
      versionPath="${v}"
      break
    fi
  done
  [[ "${versionPath}" == "" ]] && AbortScript "Version ${versionGaudi} does not exist!"
  # * Copy headers
  mkdir -p "${targetDir}/Gaudi/"
  cd "${targetDir}/Gaudi/"
  rm -rf *
  PrintBold "Copying \"${versionPath}\"..."
  cp -Rf "${extLibs}/gaudi/${versionPath}/"* .
  cd "${currentPath}"
}
FetchGaudi


# * ================ * #
# * --== Geant4 ==-- * #
# * ================ * #
function FetchGeant4()
{
  local additionalPath="${extLibs}/external/BesGDML/2.8.0/x86_64-slc6-gcc46-opt/include"
  # * Copy separate header subfolders
  rm -rf "${targetDir}/geant4"
  mkdir -p "${targetDir}/geant4"
  PrintBold "Copying Geant4 headers..."
  for folder in $(ls "${additionalPath}/G4Binding"); do
    cp -Rf "${additionalPath}/G4Binding/${folder}/${folder}" "${targetDir}/geant4"
  done
  # * Copy separate hh files
  local extensions=(h hh icc)
  for ext in ${extensions[@]}; do
    cp ${extLibs}/external/geant4/4.9.3p01/x86_64-slc6-gcc46-opt/include/*.$ext "${targetDir}/geant4"
  done
}
FetchGeant4


# * ============== * #
# * --== ROOT ==-- * #
# * ============== * #
function FetchROOT()
{
  rm -rf "${targetDir}/root"
  mkdir -p "${targetDir}/root"
  PrintBold "Copying ROOT headers..."
  cp -Rf "${extLibs}/external/ROOT/5.34.09/x86_64-slc6-gcc46-opt/root/include/"* "${targetDir}/root/"
}
FetchROOT


# * ==================== * #
# * --== Additional ==-- * #
# * ==================== * #
function FetchAdditional()
{
  local additionalPath="${extLibs}/external"
  rm -rf "${targetDir}/Additional"
  mkdir -p "${targetDir}/Additional"
  PrintBold "Copying Saxana, Schema and Reflex headers..."
  toCopy=(
    "BesGDML/2.8.0/x86_64-slc6-gcc46-opt/include/Common/Saxana/Saxana"
    "BesGDML/2.8.0/x86_64-slc6-gcc46-opt/include/Common/Schema/Schema"
    "ROOT/5.34.09/x86_64-slc6-gcc46-opt/root/include/Reflex"
  )
  for i in ${toCopy[@]}; do
    cp -Rf "${additionalPath}/$i" "${targetDir}/Additional"
  done
}
FetchAdditional